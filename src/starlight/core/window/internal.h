#pragma once

#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>

#include <GLFW/glfw3.h>

typedef struct
{
	bool keys[INPUT_KEY_COUNT];
	bool keys_prev[INPUT_KEY_COUNT];
	
	bool mouse_btns[INPUT_MOUSE_BUTTON_COUNT];
	bool mouse_btns_prev[INPUT_MOUSE_BUTTON_COUNT];

	bool mouse_moved;
	vec2s mouse_position;
	vec2s mouse_scroll_delta;
} InputSystem;

struct Window
{
	GLFWwindow* handle;
	WindowConfig config;

	InputSystem input_system;
};

extern Window window;

InputKey glfw_key_to_input_key(int glfw_key);
InputMouseButton glfw_mouse_btn_to_input_mouse_btn(int glfw_btn);
