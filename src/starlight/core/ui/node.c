#include <starlight/core/ui/ui.h>
#include <starlight/core/ui/style.h>
#include <starlight/core/window/input.h>
#include <starlight/utils/logger.h>

#include "internal.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MAX_UI_NODES 512
#define MAX_STACK_DEPTH 32

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
				.cornerRadius = node_style->base.corner_radius,
				.border = node_style->base.border,
				.clip.horizontal = node_style->base.clip.horizontal,
				.clip.vertical = node_style->base.clip.vertical,
				.clip.childOffset = Clay_GetScrollOffset(),
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
				.cornerRadius = node_style->base.corner_radius,
				.border = node_style->base.border,
				.clip.horizontal = node_style->base.clip.horizontal,
				.clip.vertical = node_style->base.clip.vertical,
				.clip.childOffset = Clay_GetScrollOffset(),
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
				.cornerRadius = node_style->base.corner_radius,
				.border = node_style->base.border,
				.clip.horizontal = node_style->base.clip.horizontal,
				.clip.vertical = node_style->base.clip.vertical,
				.clip.childOffset = Clay_GetScrollOffset(),
				.backgroundColor = node_style->base.bg_color,
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
					.cornerRadius = node_style->base.corner_radius,
					.border = node_style->base.border,
					.clip.horizontal = node_style->base.clip.horizontal,
					.clip.vertical = node_style->base.clip.vertical,
					.clip.childOffset = Clay_GetScrollOffset(),
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
		case UI_NODE_TEXT_INPUT:
		{
			CLAY(Clay_GetElementId(clay_label), {
				.layout = node_style->base.layout,
				.border.width = node_style->base.border.width,
				.border.color = (Clay_Hovered() ? node_style->interactive.fg_hover_color : node_style->base.border.color),
				.backgroundColor = node_style->base.bg_color,
				.cornerRadius = node_style->base.corner_radius,
				.clip.horizontal = node_style->base.clip.horizontal,
				.clip.vertical = node_style->base.clip.vertical,
				.clip.childOffset = Clay_GetScrollOffset(),
			})
			{
				UITextInputState* s = (UITextInputState*)(node->user_data);
				bool hovered = Clay_Hovered();
				bool mouse_just_pressed = window_input_mouse_btn_just_pressed(INPUT_MOUSE_BUTTON_LEFT);

				if(mouse_just_pressed)
				{
					if(hovered)
					{
						Clay_ElementData element_data = Clay_GetElementData(Clay_GetElementId(clay_label));
						float local_x = window_input_mouse_get_position().x - element_data.boundingBox.x;
						ui_text_input_focus(s);
						ui_text_input_click(s, local_x);
					}
					else
					{
						ui_text_input_unfocus(s);
					}
				}

				if(s->length == 0 && !s->focused)
				{
					CLAY_TEXT(clay_label, CLAY_TEXT_CONFIG({
						.fontId = 0,
						.fontSize = node_style->base.font_size,
						.textColor = node_style->base.fg_color,
						.lineHeight = 25.0f,
					}));
				}
				else
				{
					Clay_String clay_input_text = (Clay_String){ .length = s->length, .chars = s->buffer};

					CLAY_TEXT(clay_input_text, CLAY_TEXT_CONFIG({
						.fontId = 0,
						.fontSize = node_style->base.font_size,
						.textColor = node_style->base.fg_color,
					}));
				}

				if(s->focused && (int)(s->blink_timer * 1.5f) % 2 == 0)
				{
					int cursor_index = ui_text_input_get_cursor_index(s);
					float cursor_offset_x = ui_backend_font_measure_text_width(s->buffer, cursor_index);
					float line_height = 25.0f;
					float cursor_w, cursor_h, cursor_offset_y;

					// Line
					cursor_w = ui_backend_font_glyph_advance(cursor_index < s->length ? s->buffer[cursor_index] : ' ');
					cursor_h = line_height;
					cursor_offset_y = 0.0f;

					CLAY_AUTO_ID({
						.floating = {
							.attachTo = CLAY_ATTACH_TO_PARENT,
							.attachPoints = {
								.element = CLAY_ATTACH_POINT_LEFT_TOP,
								.parent = CLAY_ATTACH_POINT_LEFT_TOP,
							},
							.offset = { .x=cursor_offset_x, .y=cursor_offset_y },
							.zIndex = 1,
						},
						.layout = {
							.sizing = {
								.width = CLAY_SIZING_FIXED(cursor_w),
								.height = CLAY_SIZING_FIXED(cursor_h),
							}
						},
						.backgroundColor = (Clay_Color){255.0f, 255.0f, 255.0f, 100.0f},
					}){}
				}
			}

			break;
		}
		default: break;
	}
}

UINode* ui_node_append(UINodeType type, const char* label)
{
	if(node_count >= MAX_UI_NODES)
	{
		log_error("UI: node pool limit exceeded\n");
		return NULL;
	}

	UINode* parent = get_current_parent();
	if(parent && parent->child_count >= MAX_NODE_CHILDREN)
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

void ui_node_stack_reset()
{
	node_count = 0;
	stack_cursor = 0;

	root_node.type = UI_NODE_ROOT;
	root_node.label = "RootNode";
	root_node.child_count = 0;
	root_node.user_data = NULL;
	root_node.style = ui_style_get_current(root_node.type);

	parent_stack[stack_cursor] = &root_node;
}

void ui_node_create_tree()
{
	execute_node(&root_node);
}

bool ui_node_push_parent(UINode* node)
{
	if(stack_cursor < MAX_STACK_DEPTH - 1)
	{
		parent_stack[++stack_cursor] = node;
		return true;
	}
	else
	{
		log_error("UI: node stack overflow!\n");
		return false;
	}
}

bool ui_node_pop_parent()
{
	if(stack_cursor > 0)
	{
		stack_cursor--;
		return true;
	}
	else
	{
		log_error("UI: Mismatched ui_node_pop_parent!\n");
		return false;
	}
}
