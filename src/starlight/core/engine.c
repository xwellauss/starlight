#include <starlight/core/engine.h>
#include <starlight/core/window/window.h>
#include <starlight/core/ecs.h>
#include <starlight/core/ui/ui.h>
#include <starlight/core/scene/scene.h>
#include "starlight/core/scene/scene_manager.h"
#include <starlight/network/network.h>
#include <starlight/audio/audio.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

#include "gl_platform.h"

#include <stb_ds.h>

typedef struct
{
	float deltatime;
} Engine;

// Delta Time
static float current_frame = 0.0f;
static float last_frame = 0.0f;

Engine engine = {0};
static EngineConfig engine_config;

float engine_get_deltatime()
{
	return engine.deltatime;
}

static void engine_render_frame()
{
	{
		current_frame = (float)window_get_time();
		engine.deltatime = current_frame - last_frame;
		last_frame = current_frame;
	}

	window_poll_events();

	if(!window_is_valid()) return;

	ui_process_input();
	scene_manager_scene_process_input();
	scene_manager_scene_update();

	window_clear();

	scene_manager_scene_render();

	ui_begin_frame();
	scene_manager_scene_build_ui();
	ui_render_frame();
	
	window_swap_buffers();
}

bool engine_init(EngineConfig e_config)
{
	engine_config = e_config;

	WindowConfig window_config;

	window_config.title = engine_config.window_title;
	window_config.width = engine_config.window_width;
	window_config.height = engine_config.window_height;

	window_init(window_config);
	window_change_bgcolor(hex_to_rgb("#333333", 0.5f));	

	ecs_init();

	if(engine_config.enable_ui) ui_init(engine_config.font_path);

	if(engine_config.enable_network) network_init();

	if(engine_config.enable_audio) audio_init();

	return true;
}

void engine_run()
{
#if !defined(_PLATFORM_WEB)
	while(!window_should_close())
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
	if(engine_config.enable_audio) audio_destroy();

	ecs_destroy();

	scene_manager_destroy_all_scenes();

	if(engine_config.enable_ui) ui_destroy();
	window_destroy();
}
