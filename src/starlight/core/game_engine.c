#include <starlight/core/game_engine.h>
#include <starlight/core/window.h>
#include <starlight/core/ecs.h>
#include <starlight/core/ui/ui.h>
#include <starlight/core/scene.h>
#include <starlight/network/network.h>
#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

#include <stb_ds.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "gl_platform.h"

typedef struct
{
	Window current_window;
	Audio_Engine audio_engine;
	Scene* current_scene;
	float deltatime;
	struct { char* key; Scene* value; }* scenes;
} GameEngine;

// Delta Time
static float current_frame = 0.0f;
static float last_frame = 0.0f;

static GameEngine game_engine = (GameEngine){};
static EngineConfig engine_config;

void engine_scene_register(Scene* scene)
{
	if(!scene || !scene->scene_name) return;

	shput(game_engine.scenes, scene->scene_name, scene);
	// TODO: Make this better
	scene->init();
}

void engine_scene_switch(char* scene_name)
{
	Scene* target = shget(game_engine.scenes, scene_name);

	if (!target)
	{
		log_error("Scene %s not found!\n", scene_name);
		return;
	}

	if(game_engine.current_scene)
	{
		game_engine.current_scene->deactivate();
	}

	game_engine.current_scene = target;
	game_engine.current_scene->activate();
}

const Scene* engine_scene_get_current()
{
	return game_engine.current_scene;
}

static void engine_scene_destroy_all()
{
	for(int i = 0; i < shlen(game_engine.scenes); i++)
	{
		Scene* target = game_engine.scenes[i].value;
		target->deactivate();
		target->destroy();
	}
}

float engine_get_deltatime()
{
	return game_engine.deltatime;
}

const Window* engine_window_get()
{
	return &game_engine.current_window;
}

int engine_window_get_width()
{
	return game_engine.current_window.width;
}

int engine_window_get_height()
{
	return game_engine.current_window.height;
}

static void engine_render_frame()
{
	{
		current_frame = (float)window_get_time();
		game_engine.deltatime = current_frame - last_frame;
		last_frame = current_frame;
	}

	window_poll_events();

	if(!window_is_valid(&game_engine.current_window)) return;

	ui_process_input();
	game_engine.current_scene->process_input();
	game_engine.current_scene->update();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	game_engine.current_scene->render();

	ui_begin_frame();
	game_engine.current_scene->build_ui();
	ui_render_frame();
	
	window_swap_buffers(&game_engine.current_window);
}

bool engine_init(EngineConfig config)
{
	engine_config = config;
	game_engine.current_window.title = config.window_title;

#if defined(_PLATFORM_WEB)
	game_engine.current_window.width = 1920;
	game_engine.current_window.height = 1080;

	emscripten_request_fullscreen_strategy("canvas", true, &(EmscriptenFullscreenStrategy)
	{
		.scaleMode=EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT,
		.canvasResolutionScaleMode=EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE,
		.filteringMode=EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST
	});
#else
	game_engine.current_window.width = config.window_width;
	game_engine.current_window.height = config.window_height;
#endif

	window_init(&game_engine.current_window);
	window_change_bgcolor(hex_to_rgb("#333333", 0.5f));

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	ecs_init();

	if(config.enable_ui) ui_init(config.font_path);

	if(config.enable_network) network_init();

	if(config.enable_audio) audio_init(&game_engine.audio_engine);

	return true;
}

void engine_run()
{
#if !defined(_PLATFORM_WEB)
	while(!window_should_close(&game_engine.current_window))
	{
		engine_render_frame();
	}
#else
	emscripten_set_main_loop(engine_render_frame, 0, true);
#endif
}

void engine_destroy()
{
	if(engine_config.enable_network) network_destroy();
	if(engine_config.enable_audio) audio_destroy(&game_engine.audio_engine);

	ecs_destroy();
	engine_scene_destroy_all();

	if(engine_config.enable_ui) ui_destroy();
	window_destroy(&game_engine.current_window);
}
