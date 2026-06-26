#pragma once

#include <starlight/core/window.h>
#include <starlight/core/scene.h>

typedef struct
{
	bool enable_ui;
	bool enable_network;
	bool enable_audio;
	const char* font_path;
	const char* window_title;
	int window_width;
	int window_height;
} EngineConfig;

bool engine_init(EngineConfig config);
void engine_run();
void engine_destroy();

float engine_get_deltatime();

const Window* engine_window_get();
int engine_window_get_width();
int engine_window_get_height();

void engine_scene_register(Scene* scene);
void engine_scene_switch(char* scene_name);
const Scene* engine_scene_get_current();
