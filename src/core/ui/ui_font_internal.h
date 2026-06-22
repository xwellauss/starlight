#pragma once

#include "../renderer/renderer.h"

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

extern FontAtlas font_atlas;
