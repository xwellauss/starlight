#pragma once

#include <stdbool.h>

void ui_init(const char* font_path);
void ui_process_input();
void ui_begin_frame();
void ui_render_frame();

void ui_draw_text(const char* text);
void ui_button(const char* label, bool* clicked);

void ui_destroy();
