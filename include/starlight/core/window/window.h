#pragma once

#include <stdbool.h>
#include <cglm/struct.h>

#include <starlight/core/window/input.h>

typedef enum
{
	CURSOR_MODE_NORMAL = 0,
	CURSOR_MODE_HIDDEN,
	CURSOR_MODE_DISABLED,
} WindowCursorMode;

typedef struct
{
	const char* title;
	int width;
	int height;
} WindowConfig;

typedef struct Window Window;

void window_init(WindowConfig window_config);
int window_should_close();
int window_is_valid();
void window_poll_events();
double window_get_time();
void window_clear();
void window_swap_buffers();
void window_change_bgcolor(vec4s color);
int window_get_width();
int window_get_height();
void window_destroy();

void window_set_cursor_mode(WindowCursorMode mode);
