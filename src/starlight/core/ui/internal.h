#pragma once

#include <starlight/core/renderer/renderer.h>

#include <clay.h>

typedef enum
{
	UI_NODE_CONTAINER,
	UI_NODE_TEXT,
	UI_NODE_BUTTON
} UINodeType;

typedef struct
{
	Clay_Color bg_color;
	Clay_Color fg_color;

	Clay_Padding padding;
	Clay_CornerRadius corner_radius;
	// TODO: add size mode

	int font_size;
} UIBaseStyleResolved;

typedef struct
{
	Clay_Color bg_hover_color;
	Clay_Color bg_press_color;
	Clay_Color fg_hover_color;
	Clay_Color fg_press_color;
} UIInteractiveStyleResolved;

typedef struct
{
	UIBaseStyleResolved base;
	
	bool is_interactive;
	UIInteractiveStyleResolved interactive;
} UIStyleResolved;

void ui_style_init();
void ui_style_stack_reset();
UIStyleResolved* ui_style_get_current(UINodeType type);



void ui_backend_init(const char* font_path);
void ui_backend_render(Clay_RenderCommandArray cmds);
void ui_backend_destroy();
