#pragma once

#include <cglm/struct.h>

void font_renderer_init(const char* font_path, int character_size);
void font_renderer_render_text(char* text, float x, float y, float scale, char* hex_color, float opacity);
void font_renderer_destroy();
