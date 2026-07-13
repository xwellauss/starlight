#include <starlight/core/ui/style.h>
#include <starlight/utils/math_utils.h>
#include <starlight/utils/logger.h>

#include <cglm/struct.h>
#include <stdint.h>

#include "internal.h"

#define MAX_STACK_DEPTH 64

static UIStyleResolved style_stack[MAX_STACK_DEPTH];
static int style_stack_cursor;

static UIStyleResolved root_style_resolved;
static UIStyleResolved container_style_resolved;
static UIStyleResolved button_style_resolved;
static UIStyleResolved text_style_resolved;

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
	vec4s color_vec = hex_to_rgba(colorcode, alpha);

	return vec4_to_clay_color(color_vec);
}

static UIStyleResolved resolve_style(UIStyle* style)
{
	UIStyleResolved resolved = (UIStyleResolved)
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


		.base.border.color = vec4_to_clay_color(style->base.border.color),
		.base.border.width.left = (uint16_t)style->base.border.width.x,
		.base.border.width.right = (uint16_t)style->base.border.width.y,
		.base.border.width.top = (uint16_t)style->base.border.width.z,
		.base.border.width.bottom = (uint16_t)style->base.border.width.w,
		.base.border.width.betweenChildren = style->base.border.between_children,

		.is_interactive = style->is_interactive,
		.interactive.bg_hover_color = vec4_to_clay_color(style->interactive.bg_hover_color),
		.interactive.bg_press_color = vec4_to_clay_color(style->interactive.bg_press_color),
		.interactive.fg_hover_color = vec4_to_clay_color(style->interactive.fg_hover_color),
		.interactive.fg_press_color = vec4_to_clay_color(style->interactive.fg_press_color),
	};

	UISizingAxis size_x = style->base.layout.sizing.x;
	resolved.base.layout.sizing.width.type = size_x.type;

	if(size_x.type == UI_LAYOUT_SIZING_TYPE_PERCENT)
	{
		resolved.base.layout.sizing.width.size.percent = size_x.size.percent;
	}
	else
	{
		resolved.base.layout.sizing.width.size.minMax.min = size_x.size.min_max.min;
		resolved.base.layout.sizing.width.size.minMax.max = size_x.size.min_max.max;
	}

	UISizingAxis size_y = style->base.layout.sizing.y;
	resolved.base.layout.sizing.height.type = size_y.type;

	if(size_y.type == UI_LAYOUT_SIZING_TYPE_PERCENT)
	{
		resolved.base.layout.sizing.height.size.percent = size_y.size.percent;
	}
	else
	{
		resolved.base.layout.sizing.height.size.minMax.min = size_y.size.min_max.min;
		resolved.base.layout.sizing.height.size.minMax.max = size_y.size.min_max.max;
	}

	return resolved;
}

UIStyle ui_style_root_get_default()
{
	UIStyle style;

	style.base.bg_color = hex_to_rgba("#000000", 0.0f);
	style.base.fg_color = hex_to_rgba("#ffffff", 1.0f);
	style.base.corner_radius = (vec4s){0.0f, 0.0f, 0.0f, 0.0f};
	style.base.font_size = 16;
	style.base.layout.padding = (vec4s){8.0f, 8.0f, 8.0f, 8.0f};
	style.base.layout.sizing = (UISizing){ .x=ui_style_size_grow(0, 0), .y=ui_style_size_grow(0, 0)};
	style.base.layout.direction = UI_LAYOUT_TOP_TO_BOTTOM;
	style.base.layout.child_gap = 4;
	style.base.layout.child_alignment_x = UI_LAYOUT_ALIGN_X_LEFT;
	style.base.layout.child_alignment_y = UI_LAYOUT_ALIGN_Y_TOP;
	style.base.border.color = hex_to_rgba("#000000", 0.0f);
	style.base.border.width = (vec4s){0.0f, 0.0f, 0.0f, 0.0f};
	style.base.border.between_children = 0;
	style.is_interactive = false;

	return style;
}

UIStyle ui_style_container_get_default()
{
	UIStyle style;

	style.base.bg_color = hex_to_rgba("#000000", 0.0f);
	style.base.fg_color = hex_to_rgba("#ffffff", 1.0f);
	style.base.corner_radius = (vec4s){0.0f, 0.0f, 0.0f, 0.0f};
	style.base.font_size = 16;
	style.base.layout.padding = (vec4s){8.0f, 8.0f, 8.0f, 8.0f};
	style.base.layout.sizing = (UISizing){ .x=ui_style_size_fit(0, 0), .y=ui_style_size_fit(0, 0)};
	style.base.layout.direction = UI_LAYOUT_TOP_TO_BOTTOM;
	style.base.layout.child_gap = 4;
	style.base.layout.child_alignment_x = UI_LAYOUT_ALIGN_X_LEFT;
	style.base.layout.child_alignment_y = UI_LAYOUT_ALIGN_Y_TOP;
	style.base.border.color = hex_to_rgba("#000000", 0.0f);
	style.base.border.width = (vec4s){0.0f, 0.0f, 0.0f, 0.0f};
	style.base.border.between_children = 0;
	style.is_interactive = false;

	return style;
}

UIStyle ui_style_text_get_default()
{
	return ui_style_container_get_default();
}

UIStyle ui_style_button_get_default()
{
	UIStyle style;

	style.base.bg_color = hex_to_rgba("#9a8040", 1.0f);
	style.base.fg_color = hex_to_rgba("#ffffff", 1.0f);
	style.base.corner_radius = (vec4s){4.0f, 4.0f, 4.0f, 4.0f};
	style.base.font_size = 16;
	style.base.layout.padding = (vec4s){8.0f, 8.0f, 8.0f, 8.0f};
	style.base.layout.sizing = (UISizing){ .x=ui_style_size_fit(0, 0), .y=ui_style_size_fit(0, 0)};
	style.base.layout.child_gap = 0;
	style.base.layout.direction = UI_LAYOUT_TOP_TO_BOTTOM;
	style.base.layout.child_alignment_x = UI_LAYOUT_ALIGN_X_LEFT;
	style.base.layout.child_alignment_y = UI_LAYOUT_ALIGN_Y_TOP;
	style.base.border.color = hex_to_rgba("#000000", 0.0f);
	style.base.border.width = (vec4s){0.0f, 0.0f, 0.0f, 0.0f};
	style.base.border.between_children = 0;
	style.is_interactive = true;
	style.interactive.bg_hover_color = hex_to_rgba("#746030", 1.0f);
	style.interactive.bg_press_color = hex_to_rgba("#746055", 1.0f);
	style.interactive.fg_hover_color = hex_to_rgba("#cccccc", 1.0f);
	style.interactive.fg_press_color = hex_to_rgba("#aaaaaa", 1.0f);

	return style;
}

void ui_style_root_set(UIStyle* style)
{
	root_style_resolved = resolve_style(style);
}

void ui_style_container_set(UIStyle* style)
{
	container_style_resolved = resolve_style(style);
}

void ui_style_text_set(UIStyle* style)
{
	text_style_resolved = resolve_style(style);
}

void ui_style_button_set(UIStyle* style)
{
	button_style_resolved = resolve_style(style);
}

void ui_style_init()
{
}

void ui_style_stack_reset()
{
	style_stack_cursor = 0;

	UIStyle s1, s2, s3, s4;
	s1 = ui_style_root_get_default();
	s2 = ui_style_container_get_default();
	s3 = ui_style_text_get_default();
	s4 = ui_style_button_get_default();

	root_style_resolved = resolve_style(&s1);
	container_style_resolved = resolve_style(&s2);
	text_style_resolved = resolve_style(&s3);
	button_style_resolved = resolve_style(&s4);
}

UIStyleResolved ui_style_get_current(UINodeType type)
{
	if(style_stack_cursor > 0)
	{
		return style_stack[style_stack_cursor];
	}

	switch(type)
	{
		case UI_NODE_ROOT: return root_style_resolved;
		case UI_NODE_CONTAINER: return container_style_resolved;
		case UI_NODE_TEXT: return text_style_resolved;
		case UI_NODE_BUTTON: return button_style_resolved;
		default: return container_style_resolved;
	}
}

void ui_style_push(UIStyle* style)
{
	if(style_stack_cursor < MAX_STACK_DEPTH - 1)
	{
		style_stack_cursor++;

		style_stack[style_stack_cursor] = resolve_style(style);
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
