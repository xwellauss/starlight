#pragma once

#include <stdbool.h>

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

bool engine_init(EngineConfig e_config);
void engine_run();
void engine_destroy();

float engine_get_deltatime();

