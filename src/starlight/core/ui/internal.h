#pragma once

#include <starlight/core/renderer/renderer.h>
#include <starlight/core/resources/font_atlas.h>
#include <starlight/core/ui/widget.h>

#include <clay.h>

#define MAX_NODE_CHILDREN 32

typedef enum
{
	UI_NODE_ROOT,
	UI_NODE_CONTAINER,
	UI_NODE_TEXT,
	UI_NODE_BUTTON,
	UI_NODE_TEXT_INPUT,
} UINodeType;

typedef struct
{
	Clay_Color bg_color;
	Clay_Color fg_color;

	Clay_CornerRadius corner_radius;
	Clay_LayoutConfig layout;
	Clay_BorderElementConfig border;
	Clay_ClipElementConfig clip;

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

typedef struct UINode
{
	UINodeType type;
	const char* label;

	struct UINode* children[MAX_NODE_CHILDREN];
	int child_count;

	UIStyleResolved style;

	void* user_data;
} UINode;

void ui_node_stack_reset();
void ui_node_create_tree();
UINode* ui_node_append(UINodeType type, const char* label);
bool ui_node_push_parent(UINode* node);
bool ui_node_pop_parent();

void ui_text_input_char_queue_reset();
void ui_text_input_focus(UITextInputState* state);
void ui_text_input_unfocus(UITextInputState* state);
bool ui_text_input_is_focused(UITextInputState* state);
int ui_text_input_get_cursor_index(UITextInputState* s);
void ui_text_input_click(UITextInputState* state, float local_x);
void ui_text_input_update(UITextInputState* state);

void ui_style_init();
void ui_style_stack_reset();
UIStyleResolved ui_style_get_current(UINodeType type);

void ui_backend_init(const char* font_path);
void ui_backend_render(Clay_RenderCommandArray cmds);
void ui_backend_destroy();
FontAtlas* ui_backend_font_get_current();
