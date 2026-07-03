#pragma once

#include <stdbool.h>

void ui_init(const char* font_path);
void ui_destroy();
void ui_process_input();
void ui_begin_frame();
void ui_render_frame();

void ui_begin_container(const char* label, bool* clicked);
void ui_end_container();

void ui_text(const char* label);
void ui_button(const char* label, bool* clicked);

