#include <starlight/core/window/input.h>

#include "internal.h"

InputKey glfw_key_to_input_key(int glfw_key)
{
	switch(glfw_key)
	{
		case GLFW_KEY_A: return INPUT_KEY_A;
		default: return INPUT_KEY_UNKNOWN;
	}
}

InputMouseButton glfw_mouse_btn_to_input_mouse_btn(int glfw_btn)
{
	switch(glfw_btn)
	{
		case GLFW_MOUSE_BUTTON_1: return INPUT_MOUSE_BUTTON_1;
		case GLFW_MOUSE_BUTTON_2: return INPUT_MOUSE_BUTTON_2;
		case GLFW_MOUSE_BUTTON_3: return INPUT_MOUSE_BUTTON_3;
		case GLFW_MOUSE_BUTTON_4: return INPUT_MOUSE_BUTTON_4;
		case GLFW_MOUSE_BUTTON_5: return INPUT_MOUSE_BUTTON_5;
		case GLFW_MOUSE_BUTTON_6: return INPUT_MOUSE_BUTTON_6;
		case GLFW_MOUSE_BUTTON_7: return INPUT_MOUSE_BUTTON_7;
		case GLFW_MOUSE_BUTTON_8: return INPUT_MOUSE_BUTTON_8;
	}
}

bool window_input_key_is_down(InputKey key)
{
	return window.input_system.keys_held_down[key];
}

bool window_input_key_just_pressed(InputKey key)
{
	return (window.input_system.keys_held_down[key] && !window.input_system.previous_keys_held[key]);
}

bool window_input_key_just_released(InputKey key)
{
	return (!window.input_system.keys_held_down[key] && window.input_system.previous_keys_held[key]);
}

bool window_input_mouse_btn_is_down(InputMouseButton btn)
{
	return window.input_system.mouse_btns_held_down[btn];
}

bool window_input_mouse_btn_just_pressed(InputMouseButton btn)
{
	return (window.input_system.mouse_btns_held_down[btn] && !window.input_system.previous_mouse_btns_held[btn]);
}

bool window_input_mouse_btn_just_released(InputMouseButton btn)
{
	return (!window.input_system.mouse_btns_held_down[btn] && window.input_system.previous_mouse_btns_held[btn]);
}

vec2s window_input_mouse_get_position()
{
	return window.input_system.mouse_position;
}
vec2s window_input_mouse_get_scroll()
{
	return window.input_system.mouse_scroll_delta;
}
