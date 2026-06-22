#pragma once

#include <clay/clay.h>
#include <cglm/struct.h>

#include "../renderer/renderer.h"

void ui_font_init(const char* filepath, int atlas_w, int atlas_h, float backed_font_size);
//void ui_font_render_text(const char* text, float x, float y, float requested_scale, vec4s color);
void ui_font_destroy();

Clay_Dimensions ui_font_clay_measure_text(Clay_StringSlice glyph_vtx_array, Clay_TextElementConfig* config, void* user_data);
