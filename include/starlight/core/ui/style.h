#pragma once

#include <cglm/struct.h>

typedef struct
{
	vec4s bg_color;
	vec4s fg_color;

	vec4s padding;
	vec4s corner_radius;
	// TODO: add size mode

	int font_size;
} UIBaseStyle;

typedef struct
{
	vec4s bg_hover_color;
	vec4s bg_press_color;
	vec4s fg_hover_color;
	vec4s fg_press_color;
} UIInteractiveStyle;

typedef struct
{
	UIBaseStyle base;

	bool is_interactive;
	UIInteractiveStyle interactive;
} UIStyle;

void ui_style_push(UIStyle* style);
void ui_style_pop();

