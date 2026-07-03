#include <starlight/core/ui/ui.h>
#include <starlight/core/ui/style.h>
#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>
#include <starlight/core/engine.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

#include "internal.h"

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

	UIStyleResolved* style;

	void* user_data;
} UINode;

static UINode node_pool[MAX_UI_NODES];
static int node_count;

static UINode* parent_stack[MAX_STACK_DEPTH]; // stack for tracking parent containers
static int stack_cursor;

static UINode root_node;

static float window_width, window_height;

static void handle_clay_errors(Clay_ErrorData error_data)
{
	log_debug("UI: Clay Error: %s\n", error_data.errorText.chars);
}

static void toggle_debug_mode()
{
	static bool enabled = false;
	enabled = !enabled;

	Clay_SetDebugModeEnabled(enabled);
}

static UINode* get_current_parent()
{
	if(stack_cursor < 0) return NULL;
	return parent_stack[stack_cursor];
}

static UINode* append_node(UINodeType type, const char* label)
{
	if(node_count >= MAX_UI_NODES) return NULL;

	UINode* parent = get_current_parent();
	if(parent && parent->child_count >= MAX_CHILDREN)
	{
		return NULL;
	}

	UINode* node = &node_pool[node_count++];
	node->type = type;
	node->label = label;
	node->child_count = 0;
	node->style = ui_style_get_current(type);

	if(parent)
	{
		parent->children[parent->child_count++] = node;
	}

	return node;
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

static void execute_clay_node(UINode* node)
{
	if(!node) return;

	Clay_String clay_label = { .length = strlen(node->label), .chars = node->label};
	UIStyleResolved* node_style = node->style;

	switch(node->type)
	{
		case UI_NODE_CONTAINER:
		{
			CLAY(Clay_GetElementId(clay_label), {
				.layout = {.padding = node_style->base.padding},
				.backgroundColor = node_style->is_interactive ? ((Clay_Hovered()) ? (window_input_mouse_btn_is_down(INPUT_MOUSE_BUTTON_LEFT) ? node_style->interactive.bg_press_color : node_style->interactive.bg_hover_color) : node_style->base.bg_color) : node_style->base.bg_color,
				.cornerRadius = node_style->base.corner_radius
			})
			{
				if(node_style->is_interactive)
				{
					Clay_OnHover(interactive_hover_callback, node);
				}

				for(int i = 0; i < node->child_count; i++)
				{
					execute_clay_node(node->children[i]);
				}
			}
			break;
		}
		case UI_NODE_TEXT:
		{
			CLAY_AUTO_ID({
				.layout = {.padding=node_style->base.padding},
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
			CLAY_AUTO_ID({
					.layout = {.padding=node_style->base.padding},
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


void ui_init(const char* font_path)
{
	window_width = (float)window_get_width();
	window_height = (float)window_get_height();

	ui_style_init();

	size_t clay_required_memory = Clay_MinMemorySize();
	Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(clay_required_memory, malloc(clay_required_memory));

	Clay_Context* clay_ctx = Clay_Initialize(clay_arena, (Clay_Dimensions){ .width=window_width, .height=window_height}, (Clay_ErrorHandler){ .errorHandlerFunction=handle_clay_errors });

	Clay_SetCurrentContext(clay_ctx);
	Clay_SetMeasureTextFunction(ui_font_clay_measure_text, NULL);
	
	ui_font_init(font_path, 1024, 1024, 45.0f);
	ui_backend_init();
}

void ui_destroy()
{
	ui_font_destroy();
	ui_backend_destroy();
}

void ui_process_input()
{
	if(window_input_key_just_pressed(INPUT_KEY_B)) toggle_debug_mode();

	Clay_Vector2 mouse_pos = {window_input_mouse_get_position().x, window_input_mouse_get_position().y};
	vec2s mouse_scroll_delta = window_input_mouse_get_scroll();
    bool mouse_pressed = window_input_mouse_btn_is_down(INPUT_MOUSE_BUTTON_LEFT);	

	Clay_SetPointerState(mouse_pos, mouse_pressed);

	Clay_UpdateScrollContainers(true, (Clay_Vector2){mouse_scroll_delta.x, mouse_scroll_delta.y}, engine_get_deltatime()*1000.0f);
    
	Clay_SetLayoutDimensions((Clay_Dimensions){window_width, window_height});
}

void ui_begin_frame()
{
	node_count = 0;
	stack_cursor = 0;

	root_node.child_count = 0;

	parent_stack[stack_cursor] = &root_node;

	ui_style_stack_reset();
}

void ui_render_frame()
{
	Clay_BeginLayout();

	for(int i = 0; i < root_node.child_count; i++)
	{
		execute_clay_node(root_node.children[i]);
	}

	ui_backend_render(Clay_EndLayout(engine_get_deltatime()*1000.0f));
}

void ui_begin_container(const char* label, bool* clicked)
{
	UINode* node = append_node(UI_NODE_CONTAINER, label);
	node->user_data = clicked;
	
	if(stack_cursor < MAX_STACK_DEPTH - 1)
	{
		stack_cursor++;
		parent_stack[stack_cursor] = node;
	}
}

void ui_end_container()
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

void ui_button(const char* label, bool* clicked)
{
	UINode* node = append_node(UI_NODE_BUTTON, label);
	node->user_data = clicked;
}

void ui_text(const char* label)
{
	UINode* node = append_node(UI_NODE_TEXT, label);
}


