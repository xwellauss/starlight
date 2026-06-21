#pragma once

#include <clay/clay.h>
#include <cglm/struct.h>

#include "../renderer/renderer.h"

void ui_font_init(const char* filepath, int atlas_w, int atlas_h, float backed_font_size);
//void ui_font_render_text(const char* text, float x, float y, float requested_scale, vec4s color);
void ui_font_bind_atlas_texture();
void ui_font_build_glyphs(const char* text, float x, float y, float requested_scale, vec4s color, Vertex2DQuad* vertex_data, size_t* current_glyph_count, size_t max_capacity);

void ui_font_destroy();

Clay_Dimensions ui_font_clay_measure_text(Clay_StringSlice glyph_vtx_array, Clay_TextElementConfig* config, void* user_data);
