#include <starlight/core/ui/style.h>
#include "internal.h"

#include <starlight/utils/math_utils.h>
#include <starlight/utils/logger.h>

#include <cglm/struct.h>

#define MAX_STACK_DEPTH 64

static UIStyleResolved style_stack[MAX_STACK_DEPTH];
static int style_stack_cursor;

static UIStyleResolved default_container_style;
static UIStyleResolved default_button_style;
static UIStyleResolved default_text_style;

static Clay_Color vec4_to_clay_color(vec4s color_vec)
{
	Clay_Color clay_color;
	clay_color.r = color_vec.r * 255.0f;
	clay_color.g = color_vec.g * 255.0f;
	clay_color.b = color_vec.b * 255.0f;
	clay_color.a = color_vec.a * 255.0f;

	return clay_color;
}

static Clay_Color hex_to_clay_color(char colorcode[7], float alpha)
{
	vec4s color_vec = hex_to_rgb(colorcode, alpha);

	return vec4_to_clay_color(color_vec);
}

void ui_style_init()
{
	default_container_style = (UIStyleResolved)
	{
		.base.bg_color = hex_to_clay_color("#000000", 0.0f),
		.base.fg_color = hex_to_clay_color("#ffffff", 1.0f),
		.base.corner_radius = CLAY_CORNER_RADIUS(0),
		.base.font_size = 16,
		.base.layout.padding = CLAY_PADDING_ALL(0),
		.base.layout.sizing = { .width=CLAY_SIZING_FIT(0, 0), .height=CLAY_SIZING_FIT(0, 0)},
		.base.layout.layoutDirection = CLAY_LEFT_TO_RIGHT,
		.base.layout.childGap = 0,
		.base.layout.childAlignment.x = CLAY_ALIGN_X_LEFT,
		.base.layout.childAlignment.y = CLAY_ALIGN_Y_TOP,
		.is_interactive = false,
	};

	default_text_style = default_container_style;

	default_button_style = (UIStyleResolved)
	{
		.base.bg_color = hex_to_clay_color("#9a8040", 1.0f),
		.base.fg_color = hex_to_clay_color("#ffffff", 1.0f),
		.base.corner_radius = CLAY_CORNER_RADIUS(4),
		.base.font_size = 16,
		.base.layout.padding = CLAY_PADDING_ALL(0),
		.base.layout.sizing = { .width=CLAY_SIZING_FIT(0, 0), .height=CLAY_SIZING_FIT(0, 0)},
		.base.layout.layoutDirection = CLAY_LEFT_TO_RIGHT,
		.base.layout.childGap = 0,
		.base.layout.childAlignment.x = CLAY_ALIGN_X_LEFT,
		.base.layout.childAlignment.y = CLAY_ALIGN_Y_TOP,
		.is_interactive = true,
		.interactive.bg_hover_color = hex_to_clay_color("#746030", 1.0f),
		.interactive.bg_press_color = hex_to_clay_color("#746055", 1.0f),
		.interactive.fg_hover_color = hex_to_clay_color("#cccccc", 1.0f),
		.interactive.fg_press_color = hex_to_clay_color("#aaaaaa", 1.0f),
	};

}

void ui_style_stack_reset()
{
	style_stack_cursor = 0;
	style_stack[style_stack_cursor] = default_container_style;
}

UIStyleResolved* ui_style_get_current(UINodeType type)
{
	if(style_stack_cursor > 0)
	{
		return &style_stack[style_stack_cursor];
	}

	switch(type)
	{
		case UI_NODE_BUTTON:    return &default_button_style;
		case UI_NODE_TEXT:      return &default_text_style;
		case UI_NODE_CONTAINER: return &default_container_style;
	}
}

void ui_style_push(UIStyle* style)
{
	if(style_stack_cursor < MAX_STACK_DEPTH - 1)
	{
		style_stack_cursor++;

		UIStyleResolved* resolved = &style_stack[style_stack_cursor];

		*resolved = (UIStyleResolved)
		{
			.base.bg_color = vec4_to_clay_color(style->base.bg_color),
			.base.fg_color = vec4_to_clay_color(style->base.fg_color),

			.base.font_size = style->base.font_size,	

			.base.corner_radius.topLeft = (uint16_t)style->base.corner_radius.x,
			.base.corner_radius.topRight = (uint16_t)style->base.corner_radius.y,
			.base.corner_radius.bottomLeft = (uint16_t)style->base.corner_radius.z,
			.base.corner_radius.bottomRight = (uint16_t)style->base.corner_radius.w,

			.base.layout.padding.left = (uint16_t)style->base.layout.padding.x,
			.base.layout.padding.right = (uint16_t)style->base.layout.padding.y,
			.base.layout.padding.top = (uint16_t)style->base.layout.padding.z,
			.base.layout.padding.bottom = (uint16_t)style->base.layout.padding.w,
			.base.layout.childGap = style->base.layout.child_gap,
			.base.layout.layoutDirection = style->base.layout.direction,
			.base.layout.childAlignment.x = style->base.layout.child_alignment_x,
			.base.layout.childAlignment.y = style->base.layout.child_alignment_y,
			.base.layout.sizing.width.type = style->base.layout.sizing.x.type,
			.base.layout.sizing.height.type = style->base.layout.sizing.y.type,

			.is_interactive = style->is_interactive,
			.interactive.bg_hover_color = vec4_to_clay_color(style->interactive.bg_hover_color),
			.interactive.bg_press_color = vec4_to_clay_color(style->interactive.bg_press_color),
			.interactive.fg_hover_color = vec4_to_clay_color(style->interactive.fg_hover_color),
			.interactive.fg_press_color = vec4_to_clay_color(style->interactive.fg_press_color),
		};

		UISizingAxis size_x = style->base.layout.sizing.x;
		resolved->base.layout.sizing.width.type = size_x.type;

		if(size_x.type == UI_LAYOUT_SIZING_TYPE_PERCENT)
		{
			resolved->base.layout.sizing.width.size.percent = size_x.size.percent;
		}
		else
		{
			resolved->base.layout.sizing.width.size.minMax.min = size_x.size.min_max.min;
			resolved->base.layout.sizing.width.size.minMax.max = size_x.size.min_max.max;
		}

		UISizingAxis size_y = style->base.layout.sizing.y;
		resolved->base.layout.sizing.height.type = size_y.type;

		if(size_y.type == UI_LAYOUT_SIZING_TYPE_PERCENT)
		{
			resolved->base.layout.sizing.height.size.percent = size_y.size.percent;
		}
		else
		{
			resolved->base.layout.sizing.height.size.minMax.min = size_y.size.min_max.min;
			resolved->base.layout.sizing.height.size.minMax.max = size_y.size.min_max.max;
		}
	}
}

void ui_style_pop()
{
	if(style_stack_cursor > 0)
	{
		style_stack_cursor--;
	}
	else
	{
		log_error("UI: Mismatched ui_style_pop!\n");
	}
}

