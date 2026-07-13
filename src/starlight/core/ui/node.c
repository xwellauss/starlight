#include <starlight/core/ui/ui.h>
#include <starlight/core/ui/style.h>
#include <starlight/core/window/input.h>
#include <starlight/utils/logger.h>

#include "internal.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <clay.h>

#define MAX_UI_NODES 512
#define MAX_STACK_DEPTH 32
#define MAX_CHILDREN 32

typedef struct UINode
{
	UINodeType type;
	const char* label;

	struct UINode* children[MAX_CHILDREN];
	int child_count;

	UIStyleResolved style;

	void* user_data;
} UINode;

static UINode node_pool[MAX_UI_NODES];
static int node_count;

static UINode* parent_stack[MAX_STACK_DEPTH]; // stack for tracking parent containers
static int stack_cursor;

static UINode root_node;

static UINode* get_current_parent()
{
	if(stack_cursor < 0) return NULL;
	return parent_stack[stack_cursor];
}

static void interactive_hover_callback(Clay_ElementId id, Clay_PointerData pointer, void* user_data)
{
	if(!user_data) return;

	UINode* node = (UINode*)user_data;

	if(pointer.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
	{
		bool* clicked = (bool*)node->user_data;
		if(!clicked) return;

		*clicked = true;
	}
}

static UINode* append_node(UINodeType type, const char* label)
{
	if(node_count >= MAX_UI_NODES)
	{
		log_error("UI: node pool limit exceeded\n");
		return NULL;
	}

	UINode* parent = get_current_parent();
	if(parent && parent->child_count >= MAX_CHILDREN)
	{
		log_error("UI: node child limit exceeded\n");
		return NULL;
	}

	UINode* node = &node_pool[node_count++];
	node->type = type;
	node->label = label;
	node->child_count = 0;
	node->style = ui_style_get_current(type);
	node->user_data = NULL;

	if(parent)
	{
		parent->children[parent->child_count++] = node;
	}

	return node;
}

static void execute_node(UINode* node)
{
	if(!node) return;

	Clay_String clay_label = { .length = strlen(node->label), .chars = node->label};
	UIStyleResolved* node_style = &node->style;

	switch(node->type)
	{
		case UI_NODE_ROOT:
		{
			CLAY(Clay_GetElementId(clay_label), {
				.layout = node_style->base.layout,
				.backgroundColor = node_style->base.bg_color,
				.cornerRadius = node_style->base.corner_radius
			})
			{
				for(int i = 0; i < node->child_count; i++)
				{
					execute_node(node->children[i]);
				}
			}
			break;
		}
		case UI_NODE_CONTAINER:
		{
			CLAY(Clay_GetElementId(clay_label), {
				.layout = node_style->base.layout,
				.backgroundColor = node_style->is_interactive ? ((Clay_Hovered()) ? (window_input_mouse_btn_is_down(INPUT_MOUSE_BUTTON_LEFT) ? node_style->interactive.bg_press_color : node_style->interactive.bg_hover_color) : node_style->base.bg_color) : node_style->base.bg_color,
				.cornerRadius = node_style->base.corner_radius
			})
			{
				if(node_style->is_interactive)
				{
					if(node->user_data) *(bool*)node->user_data = false;
					Clay_OnHover(interactive_hover_callback, node);
				}

				for(int i = 0; i < node->child_count; i++)
				{
					execute_node(node->children[i]);
				}
			}
			break;
		}
		case UI_NODE_TEXT:
		{
			CLAY_AUTO_ID({
				.layout = node_style->base.layout,
			})
			{
				CLAY_TEXT(clay_label, CLAY_TEXT_CONFIG({
					.fontId = 0,
					.fontSize = node_style->base.font_size,
					.textColor = node_style->is_interactive ? ((Clay_Hovered()) ? (window_input_mouse_btn_is_down(INPUT_MOUSE_BUTTON_LEFT) ? node_style->interactive.fg_press_color : node_style->interactive.fg_hover_color) : node_style->base.fg_color) : node_style->base.fg_color,
				}));
			}

			break;
		}
		case UI_NODE_BUTTON:
		{
			if(node->user_data) *(bool*)node->user_data = false;

			CLAY_AUTO_ID({
					.layout = node_style->base.layout,
					.backgroundColor = ((Clay_Hovered()) ? (window_input_mouse_btn_is_down(INPUT_MOUSE_BUTTON_LEFT) ? node_style->interactive.bg_press_color : node_style->interactive.bg_hover_color) : node_style->base.bg_color),
					.cornerRadius = node_style->base.corner_radius
				})
				{
					Clay_OnHover(interactive_hover_callback, node);

					CLAY_TEXT(clay_label, CLAY_TEXT_CONFIG({
						.fontId = 0,
						.fontSize = node_style->base.font_size,
						.textColor = ((Clay_Hovered()) ? (window_input_mouse_btn_is_down(INPUT_MOUSE_BUTTON_LEFT) ? node_style->interactive.fg_press_color : node_style->interactive.fg_hover_color) : node_style->base.fg_color),
					}));
				}

			break;
		}
		default: break;
	}
}

void ui_node_stack_reset()
{
	node_count = 0;
	stack_cursor = 0;

	root_node.type = UI_NODE_ROOT;
	root_node.label = "RootNode";
	root_node.child_count = 0;
	root_node.user_data = NULL;
	root_node.style = ui_style_get_current(UI_NODE_ROOT);

	parent_stack[stack_cursor] = &root_node;
}

void ui_node_create_tree()
{
	execute_node(&root_node);
}

// Widgets and Containers
void ui_container_begin(const char* label, bool* clicked)
{
	UINode* node = append_node(UI_NODE_CONTAINER, label);
	if(!node)
	{
		log_error("UI: Container: could not append node");
		return;
	}

	node->user_data = clicked;

	if(stack_cursor < MAX_STACK_DEPTH - 1)
	{
		stack_cursor++;
		parent_stack[stack_cursor] = node;
	}
}

void ui_container_end()
{
	if(stack_cursor > 0)
	{
		stack_cursor--;
	}
	else
	{
		log_error("UI: Mismatched ui_end_contained!\n");
	}
}

void ui_widget_button(const char* label, bool* clicked)
{
	UINode* node = append_node(UI_NODE_BUTTON, label);
	if(!node)
	{
		log_error("UI: Button: could not append node");
		return;
	}

	node->user_data = clicked;
}

void ui_widget_text(const char* label)
{
	UINode* node = append_node(UI_NODE_TEXT, label);
	if(!node)
	{
		log_error("UI: Text: could not append node");
		return;
	}
}
