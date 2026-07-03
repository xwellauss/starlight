#pragma once

#include <starlight/core/renderer/renderer.h>

#include <stb_truetype.h>
#include <clay.h>

typedef enum
{
	UI_NODE_CONTAINER,
	UI_NODE_TEXT,
	UI_NODE_BUTTON
} UINodeType;

typedef struct
{
	Clay_Color bg_color;
	Clay_Color fg_color;

	Clay_Padding padding;
	Clay_CornerRadius corner_radius;
	// TODO: add size mode

	int font_size;
} UIBaseStyleResolved;

typedef struct
{
	Clay_Color bg_hover_color;
	Clay_Color bg_press_color;
	Clay_Color fg_hover_color;
	Clay_Color fg_press_color;
} UIInteractiveStyleResolved;

typedef struct
{
	UIBaseStyleResolved base;
	
	bool is_interactive;
	UIInteractiveStyleResolved interactive;
} UIStyleResolved;

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

void ui_style_init();
void ui_style_stack_reset();
UIStyleResolved* ui_style_get_current(UINodeType type);

void ui_font_init(const char* filepath, int atlas_w, int atlas_h, float backed_font_size);
//void ui_font_render_text(const char* text, float x, float y, float requested_scale, vec4s color);
void ui_font_destroy();

Clay_Dimensions ui_font_clay_measure_text(Clay_StringSlice glyph_vtx_array, Clay_TextElementConfig* config, void* user_data);

void ui_backend_init();
void ui_backend_render(Clay_RenderCommandArray cmds);
void ui_backend_destroy();
