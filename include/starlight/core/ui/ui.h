#pragma once

#include <starlight/core/ui/style.h>
#include <starlight/core/ui/widget.h>

void ui_init(const char* font_path);
void ui_destroy();
void ui_process_input();
void ui_begin_frame();
void ui_render_frame();

