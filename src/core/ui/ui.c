#include "ui.h"
#include "ui_font.h"
#include "ui_backend.h"
#include "../game_engine.h"
#include "../../utils/utils.h"
#include "../platform.h"

#include <stdint.h>

#define CLAY_IMPLEMENTATION
#include <clay/clay.h>
#include "clay/examples/shared-layouts/clay-video-demo.c"

#define UI_FONT_FILE "fonts/monocraft.ttf"

static ClayVideoDemo_Data g_demo_data;

void handle_clay_errors(Clay_ErrorData error_data)
{
	log_debug("Clay Error: %s", error_data.errorText.chars);
}


void ui_init()
{
	size_t clay_required_memory = Clay_MinMemorySize();
	Clay_Arena clay_arena = Clay_CreateArenaWithCapacityAndMemory(clay_required_memory, malloc(clay_required_memory));

	Clay_Context* clay_ctx = Clay_Initialize(clay_arena, (Clay_Dimensions){ .width=(float)game_engine.current_window.width, .height=(float)game_engine.current_window.height}, (Clay_ErrorHandler){ .errorHandlerFunction=handle_clay_errors });

	Clay_SetCurrentContext(clay_ctx);
	Clay_SetMeasureTextFunction(ui_font_clay_measure_text, NULL);
	
	ui_font_init(UI_FONT_FILE, 1024, 1024, 32.0f);
	ui_backend_init();

	Clay_SetDebugModeEnabled(true);

	g_demo_data = ClayVideoDemo_Initialize();
}

void ui_process_input()
{
	Clay_Vector2 mouse_pos = {game_engine.current_window.input_system.mouse_position.x, game_engine.current_window.input_system.mouse_position.y};

    bool mouse_pressed = game_engine.current_window.input_system.mouse_clicked_data[GLFW_MOUSE_BUTTON_LEFT];	

	Clay_SetPointerState(mouse_pos, mouse_pressed);

	Clay_UpdateScrollContainers(true, (Clay_Vector2){game_engine.current_window.input_system.mouse_scroll_delta.x, game_engine.current_window.input_system.mouse_scroll_delta.y}, game_engine.deltatime*1000.0f);
    
	Clay_SetLayoutDimensions((Clay_Dimensions){(float)game_engine.current_window.width, (float)game_engine.current_window.height});
}

void ui_render_frame()
{
    Clay_RenderCommandArray cmds = ClayVideoDemo_CreateLayout(&g_demo_data);
	ui_backend_render(cmds);
}

void ui_destroy()
{
	ui_font_destroy();
	ui_backend_destroy();
}
