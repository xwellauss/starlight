#include <starlight/core/ui/ui.h>
#include <starlight/core/game_engine.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

#include "ui_font.h"
#include "ui_backend.h"
#include "../gl_platform.h"

#include <stdint.h>

#include <clay.h>

static const Window* current_window;
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

void handle_clay_errors(Clay_ErrorData error_data)
{
	log_debug("Clay Error: %s", error_data.errorText.chars);
}


void ui_init(const char* font_path)
{
	current_window = engine_window_get();
	window_width = (float)engine_window_get_width();
	window_height = (float)engine_window_get_height();

	size_t clay_required_memory = Clay_MinMemorySize();
	Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(clay_required_memory, malloc(clay_required_memory));

	Clay_Context* clay_ctx = Clay_Initialize(clay_arena, (Clay_Dimensions){ .width=window_width, .height=window_height}, (Clay_ErrorHandler){ .errorHandlerFunction=handle_clay_errors });

	Clay_SetCurrentContext(clay_ctx);
	Clay_SetMeasureTextFunction(ui_font_clay_measure_text, NULL);
	
	ui_font_init(font_path, 1024, 1024, 45.0f);
	ui_backend_init();

	Clay_SetDebugModeEnabled(true);
}

void ui_process_input()
{
	Clay_Vector2 mouse_pos = {current_window->input_system.mouse_position.x, current_window->input_system.mouse_position.y};

    bool mouse_pressed = current_window->input_system.mouse_clicked_data[GLFW_MOUSE_BUTTON_LEFT];	

	Clay_SetPointerState(mouse_pos, mouse_pressed);

	Clay_UpdateScrollContainers(true, (Clay_Vector2){current_window->input_system.mouse_scroll_delta.x, current_window->input_system.mouse_scroll_delta.y}, engine_get_deltatime()*1000.0f);
    
	Clay_SetLayoutDimensions((Clay_Dimensions){window_width, window_height});
}

void ui_begin_frame()
{
	Clay_BeginLayout();

	CLAY(CLAY_ID("Button"), { .layout = { .padding = CLAY_PADDING_ALL(8) }, .backgroundColor = Clay_Hovered() ? hex_to_clay_color("#0000ff", 1.0f) : hex_to_clay_color("#ff0000", 1.0f), .cornerRadius = CLAY_CORNER_RADIUS(4)})
	{
    	CLAY_TEXT(Clay_Hovered() ? CLAY_STRING("Hovered") : CLAY_STRING("Hover me!"), CLAY_TEXT_CONFIG({ .fontId = 0, .fontSize = 16, .textColor = hex_to_clay_color("#ffffff", 1.0f)}));
	}
}

void ui_render_frame()
{
	ui_backend_render(Clay_EndLayout(engine_get_deltatime()*1000.0f));
}

void ui_destroy()
{
	ui_font_destroy();
	ui_backend_destroy();
}
