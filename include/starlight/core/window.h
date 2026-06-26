#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <cglm/struct.h>

typedef struct
{
	bool left;
	bool right;
	bool up;
	bool down;
	bool space;
	bool l_ctrl;
	bool key_press;
} InputState;

typedef struct
{
	bool key_pressed;
	bool key_pressed_data[GLFW_KEY_LAST];

	bool mouse_moved;
	vec2s mouse_position;

	bool mouse_clicked;
	bool mouse_clicked_data[GLFW_MOUSE_BUTTON_LAST];

	vec2s mouse_scroll_delta;
} InputSystem;

typedef struct
{
	GLFWwindow* handle;
	int width;
	int height;
	const char* title;

	InputSystem input_system;
} Window;

void window_init(Window* window);
int window_should_close(Window* window);
int window_is_valid(Window* window);
void window_poll_events();
double window_get_time();
void window_swap_buffers(Window* window);
void window_change_bgcolor(vec4s color);
void window_destroy(Window* window);

