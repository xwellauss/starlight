#include <starlight/core/ui/ui.h>
#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>
#include <starlight/core/engine.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

#include "ui_font.h"
#include "ui_backend.h"

#include <stdint.h>
#include <string.h>

#include <clay.h>

static float window_width, window_height;

static Clay_Color hex_to_clay_color(char colorcode[7], float alpha)
{
	vec4s color_vec = hex_to_rgb(colorcode, alpha);

	Clay_Color clay_color;
	clay_color.r = color_vec.r * 255.0f;
	clay_color.g = color_vec.g * 255.0f;
	clay_color.b = color_vec.b * 255.0f;
	clay_color.a = color_vec.a * 255.0f;

	return clay_color;
}

static void handle_clay_errors(Clay_ErrorData error_data)
{
	log_debug("Clay Error: %s", error_data.errorText.chars);
}

static void toggle_debug_mode()
{
	static bool enabled = false;
	enabled = !enabled;

	Clay_SetDebugModeEnabled(enabled);
}

static void button_on_hover_callback(Clay_ElementId id, Clay_PointerData pointer, void* user_data)
{
	bool* clicked = (bool*)user_data;
	if(pointer.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
	{
		*clicked = true;
	}
}


void ui_init(const char* font_path)
{
	window_width = (float)window_get_width();
	window_height = (float)window_get_height();

	size_t clay_required_memory = Clay_MinMemorySize();
	Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(clay_required_memory, malloc(clay_required_memory));

	Clay_Context* clay_ctx = Clay_Initialize(clay_arena, (Clay_Dimensions){ .width=window_width, .height=window_height}, (Clay_ErrorHandler){ .errorHandlerFunction=handle_clay_errors });

	Clay_SetCurrentContext(clay_ctx);
	Clay_SetMeasureTextFunction(ui_font_clay_measure_text, NULL);
	
	ui_font_init(font_path, 1024, 1024, 45.0f);
	ui_backend_init();
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
	Clay_BeginLayout();
}

void ui_render_frame()
{
	ui_backend_render(Clay_EndLayout(engine_get_deltatime()*1000.0f));
}

void ui_button(const char* label, bool* clicked)
{
	Clay_String clay_label = { .length = strlen(label), .chars = label};

	CLAY_AUTO_ID({
		.layout = {.padding=CLAY_PADDING_ALL(8)},
		.backgroundColor = Clay_Hovered() ? hex_to_clay_color("#746030", 1.0f) : hex_to_clay_color("#9a8040", 1.0f),
		.cornerRadius = CLAY_CORNER_RADIUS(4)
	})
	{
		Clay_OnHover(button_on_hover_callback, clicked);
		bool pressing = Clay_Hovered() && window_input_mouse_btn_is_down(INPUT_MOUSE_BUTTON_LEFT);
		CLAY_TEXT(clay_label, CLAY_TEXT_CONFIG({
			.fontId = 0,
			.fontSize = 16,
			.textColor = pressing ? hex_to_clay_color("#aaaaaa", 1.0f) : hex_to_clay_color("#ffffff", 1.0f),
		}));
	}
}

void ui_destroy()
{
	ui_font_destroy();
	ui_backend_destroy();
}
