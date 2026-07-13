#pragma once

#include <starlight/core/ui/style.h>

#include <stdbool.h>

void ui_init(const char* font_path);
void ui_destroy();
void ui_process_input();
void ui_begin_frame();
void ui_render_frame();

void ui_container_begin(const char* label, bool* clicked);
void ui_container_end();

void ui_widget_text(const char* label);
void ui_widget_button(const char* label, bool* clicked);

