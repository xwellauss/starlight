#pragma once

#include <starlight/core/renderer/renderer.h>
#include <stb_truetype.h>

typedef struct
{
	int width;
	int height;

	float baked_font_size;
	
	stbtt_packedchar glyph_ascii[96]; // ASCII 32-127
	
	float ascent;
	float descent;
	float line_gap;

	Texture2D texture;
} FontAtlas;

void font_atlas_init(FontAtlas* font_atlas, const char* filepath, int atlas_w, int atlas_h, float backed_font_size);
void font_atlas_destroy(FontAtlas* font_atlas);

float font_atlas_glyph_advance(FontAtlas* atlas, char c);
float font_atlas_measure_text_width(FontAtlas* atlas, const char* text, int count);
