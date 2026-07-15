#include <starlight/core/window/input.h>

#include "internal.h"

InputKey glfw_key_to_input_key(int glfw_key)
{
	switch(glfw_key)
	{
		case GLFW_KEY_SPACE: return INPUT_KEY_SPACE;
		case GLFW_KEY_APOSTROPHE: return INPUT_KEY_APOSTROPHE;
		case GLFW_KEY_COMMA: return INPUT_KEY_COMMA;
		case GLFW_KEY_MINUS: return INPUT_KEY_MINUS;
		case GLFW_KEY_PERIOD: return INPUT_KEY_PERIOD;
		case GLFW_KEY_SLASH: return INPUT_KEY_SLASH;
		case GLFW_KEY_0: return INPUT_KEY_0;
		case GLFW_KEY_1: return INPUT_KEY_1;
		case GLFW_KEY_2: return INPUT_KEY_2;
		case GLFW_KEY_3: return INPUT_KEY_3;
		case GLFW_KEY_4: return INPUT_KEY_4;
		case GLFW_KEY_5: return INPUT_KEY_5;
		case GLFW_KEY_6: return INPUT_KEY_6;
		case GLFW_KEY_7: return INPUT_KEY_7;
		case GLFW_KEY_8: return INPUT_KEY_8;
		case GLFW_KEY_9: return INPUT_KEY_9;
		case GLFW_KEY_SEMICOLON: return INPUT_KEY_SEMICOLON;
		case GLFW_KEY_EQUAL: return INPUT_KEY_EQUAL;
		case GLFW_KEY_A: return INPUT_KEY_A;
		case GLFW_KEY_B: return INPUT_KEY_B;
		case GLFW_KEY_C: return INPUT_KEY_C;
		case GLFW_KEY_D: return INPUT_KEY_D;
		case GLFW_KEY_E: return INPUT_KEY_E;
		case GLFW_KEY_F: return INPUT_KEY_F;
		case GLFW_KEY_G: return INPUT_KEY_G;
		case GLFW_KEY_H: return INPUT_KEY_H;
		case GLFW_KEY_I: return INPUT_KEY_I;
		case GLFW_KEY_J: return INPUT_KEY_J;
		case GLFW_KEY_K: return INPUT_KEY_K;
		case GLFW_KEY_L: return INPUT_KEY_L;
		case GLFW_KEY_M: return INPUT_KEY_M;
		case GLFW_KEY_N: return INPUT_KEY_N;
		case GLFW_KEY_O: return INPUT_KEY_O;
		case GLFW_KEY_P: return INPUT_KEY_P;
		case GLFW_KEY_Q: return INPUT_KEY_Q;
		case GLFW_KEY_R: return INPUT_KEY_R;
		case GLFW_KEY_S: return INPUT_KEY_S;
		case GLFW_KEY_T: return INPUT_KEY_T;
		case GLFW_KEY_U: return INPUT_KEY_U;
		case GLFW_KEY_V: return INPUT_KEY_V;
		case GLFW_KEY_W: return INPUT_KEY_W;
		case GLFW_KEY_X: return INPUT_KEY_X;
		case GLFW_KEY_Y: return INPUT_KEY_Y;
		case GLFW_KEY_Z: return INPUT_KEY_Z;
		case GLFW_KEY_LEFT_BRACKET: return INPUT_KEY_LEFT_BRACKET;
		case GLFW_KEY_BACKSLASH: return INPUT_KEY_BACKSLASH;
		case GLFW_KEY_RIGHT_BRACKET: return INPUT_KEY_RIGHT_BRACKET;
		case GLFW_KEY_GRAVE_ACCENT: return INPUT_KEY_GRAVE_ACCENT;
		case GLFW_KEY_WORLD_1: return INPUT_KEY_WORLD_1;
		case GLFW_KEY_WORLD_2: return INPUT_KEY_WORLD_2;
		case GLFW_KEY_ESCAPE: return INPUT_KEY_ESCAPE;
		case GLFW_KEY_ENTER: return INPUT_KEY_ENTER;
		case GLFW_KEY_TAB: return INPUT_KEY_TAB;
		case GLFW_KEY_BACKSPACE: return INPUT_KEY_BACKSPACE;
		case GLFW_KEY_INSERT: return INPUT_KEY_INSERT;
		case GLFW_KEY_DELETE: return INPUT_KEY_DELETE;
		case GLFW_KEY_RIGHT: return INPUT_KEY_RIGHT;
		case GLFW_KEY_LEFT: return INPUT_KEY_LEFT;
		case GLFW_KEY_DOWN: return INPUT_KEY_DOWN;
		case GLFW_KEY_UP: return INPUT_KEY_UP;
		case GLFW_KEY_PAGE_UP: return INPUT_KEY_PAGE_UP;
		case GLFW_KEY_PAGE_DOWN: return INPUT_KEY_PAGE_DOWN;
		case GLFW_KEY_HOME: return INPUT_KEY_HOME;
		case GLFW_KEY_END: return INPUT_KEY_END;
		case GLFW_KEY_CAPS_LOCK: return INPUT_KEY_CAPS_LOCK;
		case GLFW_KEY_SCROLL_LOCK: return INPUT_KEY_SCROLL_LOCK;
		case GLFW_KEY_NUM_LOCK: return INPUT_KEY_NUM_LOCK;
		case GLFW_KEY_PRINT_SCREEN: return INPUT_KEY_PRINT_SCREEN;
		case GLFW_KEY_PAUSE: return INPUT_KEY_PAUSE;
		case GLFW_KEY_F1: return INPUT_KEY_F1;
		case GLFW_KEY_F2: return INPUT_KEY_F2;
		case GLFW_KEY_F3: return INPUT_KEY_F3;
		case GLFW_KEY_F4: return INPUT_KEY_F4;
		case GLFW_KEY_F5: return INPUT_KEY_F5;
		case GLFW_KEY_F6: return INPUT_KEY_F6;
		case GLFW_KEY_F7: return INPUT_KEY_F7;
		case GLFW_KEY_F8: return INPUT_KEY_F8;
		case GLFW_KEY_F9: return INPUT_KEY_F9;
		case GLFW_KEY_F10: return INPUT_KEY_F10;
		case GLFW_KEY_F11: return INPUT_KEY_F11;
		case GLFW_KEY_F12: return INPUT_KEY_F12;
		case GLFW_KEY_F13: return INPUT_KEY_F13;
		case GLFW_KEY_F14: return INPUT_KEY_F14;
		case GLFW_KEY_F15: return INPUT_KEY_F15;
		case GLFW_KEY_F16: return INPUT_KEY_F16;
		case GLFW_KEY_KP_0: return INPUT_KEY_KP_0;
		case GLFW_KEY_KP_1: return INPUT_KEY_KP_1;
		case GLFW_KEY_KP_2: return INPUT_KEY_KP_2;
		case GLFW_KEY_KP_3: return INPUT_KEY_KP_3;
		case GLFW_KEY_KP_4: return INPUT_KEY_KP_4;
		case GLFW_KEY_KP_5: return INPUT_KEY_KP_5;
		case GLFW_KEY_KP_6: return INPUT_KEY_KP_6;
		case GLFW_KEY_KP_7: return INPUT_KEY_KP_7;
		case GLFW_KEY_KP_8: return INPUT_KEY_KP_8;
		case GLFW_KEY_KP_9: return INPUT_KEY_KP_9;
		case GLFW_KEY_KP_DECIMAL: return INPUT_KEY_KP_DECIMAL;
		case GLFW_KEY_KP_DIVIDE: return INPUT_KEY_KP_DIVIDE;
		case GLFW_KEY_KP_MULTIPLY: return INPUT_KEY_KP_MULTIPLY;
		case GLFW_KEY_KP_SUBTRACT: return INPUT_KEY_KP_SUBTRACT;
		case GLFW_KEY_KP_ADD: return INPUT_KEY_KP_ADD;
		case GLFW_KEY_KP_ENTER: return INPUT_KEY_KP_ENTER;
		case GLFW_KEY_KP_EQUAL: return INPUT_KEY_KP_EQUAL;
		case GLFW_KEY_LEFT_SHIFT: return INPUT_KEY_LEFT_SHIFT;
		case GLFW_KEY_LEFT_CONTROL: return INPUT_KEY_LEFT_CONTROL;
		case GLFW_KEY_LEFT_ALT: return INPUT_KEY_LEFT_ALT;
		case GLFW_KEY_LEFT_SUPER: return INPUT_KEY_LEFT_SUPER;
		case GLFW_KEY_RIGHT_SHIFT: return INPUT_KEY_RIGHT_SHIFT;
		case GLFW_KEY_RIGHT_CONTROL: return INPUT_KEY_RIGHT_CONTROL;
		case GLFW_KEY_RIGHT_ALT: return INPUT_KEY_RIGHT_ALT;
		case GLFW_KEY_RIGHT_SUPER: return INPUT_KEY_RIGHT_SUPER;
		case GLFW_KEY_MENU: return INPUT_KEY_MENU;
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
		default: return INPUT_MOUSE_BUTTON_UNKNOWN;
	}
}

bool window_input_mod_active(InputMod mod)
{
	return (window.input_system.mods & mod) != 0;
}

bool window_input_mods_active(int mask)
{
	return (window.input_system.mods & mask) == mask;
}

int window_input_get_key_char_queue(uint32_t* out, int max)
{
	int count = (window.input_system.key_char_queue_len < max) ? window.input_system.key_char_queue_len : max;

	for(int i = 0; i < count; i++)
	{
		out[i] = window.input_system.key_char_queue[i];
	}

	return count;
}

void window_input_clear_char_queue()
{
	window.input_system.key_char_queue_len = 0;
}

bool window_input_key_is_down(InputKey key)
{
	return window.input_system.keys[key];
}

bool window_input_key_just_pressed(InputKey key)
{
	return (window.input_system.keys[key] && !window.input_system.keys_prev[key]);
}

bool window_input_key_just_released(InputKey key)
{
	return (!window.input_system.keys[key] && window.input_system.keys_prev[key]);
}

bool window_input_mouse_btn_is_down(InputMouseButton btn)
{
	return window.input_system.mouse_btns[btn];
}

bool window_input_mouse_btn_just_pressed(InputMouseButton btn)
{
	return (window.input_system.mouse_btns[btn] && !window.input_system.mouse_btns_prev[btn]);
}

bool window_input_mouse_btn_just_released(InputMouseButton btn)
{
	return (!window.input_system.mouse_btns[btn] && window.input_system.mouse_btns_prev[btn]);
}

vec2s window_input_mouse_get_position()
{
	return window.input_system.mouse_position;
}
vec2s window_input_mouse_get_scroll()
{
	return window.input_system.mouse_scroll_delta;
}

bool window_input_mouse_has_moved()
{
	return window.input_system.mouse_moved;
}
