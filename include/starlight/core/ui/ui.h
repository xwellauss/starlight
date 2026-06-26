#pragma once

void ui_init(const char* font_path);
void ui_process_input();
void ui_begin_frame();
void ui_render_frame();

void ui_draw_text(const char* text);

void ui_destroy();
