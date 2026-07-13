#pragma once

#include <cglm/struct.h>
#include <stdint.h>

typedef enum
{
	UI_LAYOUT_LEFT_TO_RIGHT = 0,
	UI_LAYOUT_TOP_TO_BOTTOM
} UILayoutDirection;

typedef enum
{
	UI_LAYOUT_ALIGN_X_LEFT = 0,
	UI_LAYOUT_ALIGN_X_RIGHT,
	UI_LAYOUT_ALIGN_X_CENTER,
} UILayoutAlignmentX;

typedef enum
{
	UI_LAYOUT_ALIGN_Y_TOP = 0,
	UI_LAYOUT_ALIGN_Y_BOTTOM,
	UI_LAYOUT_ALIGN_Y_CENTER,
} UILayoutAlignmentY;

typedef enum
{
	UI_LAYOUT_SIZING_TYPE_FIT = 0,
	UI_LAYOUT_SIZING_TYPE_GROW,
	UI_LAYOUT_SIZING_TYPE_PERCENT,
	UI_LAYOUT_SIZING_TYPE_FIXED,
} UILayoutSizingType;

typedef struct
{
	UILayoutSizingType type;
	union
	{
		struct { float min; float max; } min_max;
		float percent;
	} size;
} UISizingAxis;

typedef struct
{
	UISizingAxis x; // along the width
	UISizingAxis y; // along the height
} UISizing;

typedef struct
{
	vec4s padding;
	uint16_t child_gap;
	UILayoutDirection direction;
	UILayoutAlignmentX child_alignment_x;
	UILayoutAlignmentY child_alignment_y;
	UISizing sizing;
} UILayout;

typedef struct
{
	vec4s bg_color;
	vec4s fg_color;

	vec4s corner_radius;

	UILayout layout;

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

UIStyle ui_style_root_default_style();
UIStyle ui_style_container_default_style();
UIStyle ui_style_text_default_style();
UIStyle ui_style_button_default_style();

static inline UISizingAxis ui_style_size_fit(float min, float max)
{
	return (UISizingAxis){ .type = UI_LAYOUT_SIZING_TYPE_FIT, .size.min_max = {min, max} };
}

static inline UISizingAxis ui_style_size_grow(float min, float max)
{
	return (UISizingAxis){ .type = UI_LAYOUT_SIZING_TYPE_GROW, .size.min_max = {min, max} };
}

static inline UISizingAxis ui_style_size_fixed(float value)
{
	return (UISizingAxis){ .type = UI_LAYOUT_SIZING_TYPE_FIXED, .size.min_max = {value, value} };
}

static inline UISizingAxis ui_style_size_percent(float value)
{
	return (UISizingAxis){ .type = UI_LAYOUT_SIZING_TYPE_PERCENT, .size.percent = value };
}
