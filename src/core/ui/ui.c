#include "ui.h"
#include "../game_engine.h"
#include "../font_renderer.h"
#include "../../utils/utils.h"

#include <stdint.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"

static uint64_t total_memory_size = 0;
static Clay_Arena clay_arena;

void handle_clay_errors(Clay_ErrorData error_data)
{
	log_debug("Clay Error: %s", error_data.errorText.chars);
}


void ui_init()
{
	total_memory_size = Clay_MinMemorySize();

	clay_arena = Clay_CreateArenaWithCapacityAndMemory(total_memory_size, malloc(total_memory_size));

	Clay_Initialize(clay_arena, (Clay_Dimensions){game_engine.current_window.width, game_engine.current_window.height}, (Clay_ErrorHandler){ handle_clay_errors });
}

void ui_render()
{
}

void ui_destroy()
{
}
