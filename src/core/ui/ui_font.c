#include "ui_font.h"
#include "../game_engine.h"
#include "../renderer/renderer.h"
#include "../../utils/utils.h"
#include "../platform.h"

#include <stb_rect_pack.h>
#include <stb_truetype.h>
#include <stb_ds.h>

#include <cglm/struct.h>


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

static Window* current_window;

static FontAtlas font_atlas;

void ui_font_init(const char* filepath, int atlas_w, int atlas_h, float backed_font_size)
{
	current_window = &game_engine.current_window;

	unsigned char* font_file_buffer = (unsigned char*)read_file(filepath, "rb");

	font_atlas.width = atlas_w;
	font_atlas.height = atlas_h;
	font_atlas.baked_font_size = backed_font_size;
	unsigned char* atlas_buffer = (unsigned char*)malloc(font_atlas.width * font_atlas.height);

	stbtt_fontinfo font_info;
	stbtt_InitFont(&font_info, font_file_buffer, 0);


	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

	float scale_factor = stbtt_ScaleForPixelHeight(&font_info, font_atlas.baked_font_size);
	font_atlas.ascent = ascent * scale_factor;
	font_atlas.descent = descent * scale_factor;
	font_atlas.line_gap = line_gap * scale_factor;

	
	
	stbtt_pack_context font_pack_ctx;
	stbtt_PackBegin(&font_pack_ctx, atlas_buffer, font_atlas.width, font_atlas.height, 0, 1, NULL);

	// ASCII 32-127
	//stbtt_PackSetOversampling(&font_pack_ctx, 2, 2);
	stbtt_PackFontRange(&font_pack_ctx, font_file_buffer, 0, font_atlas.baked_font_size, 32, 96, font_atlas.glyph_ascii);
	
	stbtt_PackEnd(&font_pack_ctx);

	// Upload to Textures
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	font_atlas.texture.width = font_atlas.width;
	font_atlas.texture.height = font_atlas.height;
	font_atlas.texture.texture_config.is_init = true;
	font_atlas.texture.texture_config.wrap_s = GL_CLAMP_TO_EDGE;
	font_atlas.texture.texture_config.wrap_t = GL_CLAMP_TO_EDGE;
	font_atlas.texture.texture_config.min_filter = GL_NEAREST;
	font_atlas.texture.texture_config.mag_filter = GL_NEAREST;
	
	texture2d_init_from_data(&font_atlas.texture, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, atlas_buffer);

	free(atlas_buffer);
	free(font_file_buffer);
}

/*
void ui_font_render_text(const char* text, float x, float y, float requested_scale, vec4s color)
{
	shader_bind(&glyph_shader);
	
	shader_uniform_mat4(&glyph_shader, "projection", projection);
	shader_uniform_vec4(&glyph_shader, "text_color", color);

	Vertex2DQuad vertex_data[MAX_GLYPHS];
	size_t glyph_count = 0;
	vertex_buffer_bind(&glyph_vertex_buffer, BUFFER_VAO);
	
	float render_scale = requested_scale / font_atlas.baked_font_size;
	y += font_atlas.ascent * render_scale;
	for(const char* c = text; *c != '\0'; c++)
	{
		unsigned char ch = *c;
		if(ch < 32 || ch > 127) continue;

		stbtt_packedchar* pc = &font_atlas.glyph_ascii[ch - 32];

		float x0 = x + pc->xoff * render_scale;
		float y0 = y + pc->yoff * render_scale;
		float x1 = x + pc->xoff2 * render_scale;
		float y1 = y + pc->yoff2 * render_scale;

		float u0 = pc->x0 / (float)font_atlas.width;
		float v0 = pc->y0 / (float)font_atlas.height;
		float u1 = pc->x1 / (float)font_atlas.width;
		float v1 = pc->y1 / (float)font_atlas.height;
		
		if(glyph_count >= MAX_GLYPHS) break;

		vertex_data[glyph_count] = (Vertex2DQuad)
		{{
			{{x0, y0}, {0.0f, 0.0f, 0.0f, 0.0f}, {u0, v0}},
			{{x1, y0}, {0.0f, 0.0f, 0.0f, 0.0f}, {u1, v0}},
			{{x1, y1}, {0.0f, 0.0f, 0.0f, 0.0f}, {u1, v1}},
			{{x0, y1}, {0.0f, 0.0f, 0.0f, 0.0f}, {u0, v1}}
		}};
		
		x += pc->xadvance * render_scale;

		glyph_count++;
	}

	texture_active_slot(GL_TEXTURE0);
	texture2d_bind(&font_atlas.texture);
	
	vertex_buffer_bind(&glyph_vertex_buffer, BUFFER_VBO);
	vertex_buffer_update(&glyph_vertex_buffer, vertex_data, glyph_count*sizeof(Vertex2DQuad), 0);
	vertex_buffer_draw_indexed(&glyph_vertex_buffer, GL_TRIANGLES, GL_UNSIGNED_SHORT, glyph_count*6, 0);
}
*/

void ui_font_bind_atlas_texture()
{
	texture_active_slot(GL_TEXTURE0);
	texture2d_bind(&font_atlas.texture);
}

void ui_font_build_glyphs(const char* text, float x, float y, float requested_scale, vec4s color, Vertex2DQuad* vertex_data, size_t* current_glyph_count, size_t max_capacity)
{
	float render_scale = requested_scale / font_atlas.baked_font_size;
	y += font_atlas.ascent * render_scale;

	for(const char* c = text; *c != '\0'; c++)
	{
		if(*current_glyph_count >= max_capacity) break;

		unsigned char ch = *c;
		if(ch < 32 || ch > 127) continue;

		stbtt_packedchar* pc = &font_atlas.glyph_ascii[ch - 32];

		float x0 = x + pc->xoff * render_scale;
		float y0 = y + pc->yoff * render_scale;
		float x1 = x + pc->xoff2 * render_scale;
		float y1 = y + pc->yoff2 * render_scale;

		float u0 = pc->x0 / (float)font_atlas.width;
		float v0 = pc->y0 / (float)font_atlas.height;
		float u1 = pc->x1 / (float)font_atlas.width;
		float v1 = pc->y1 / (float)font_atlas.height;	

		vertex_data[(*current_glyph_count)++] = (Vertex2DQuad)
		{{
			{{x0, y0}, color, {u0, v0}},
			{{x1, y0}, color, {u1, v0}},
			{{x1, y1}, color, {u1, v1}},
			{{x0, y1}, color, {u0, v1}}
		}};
		
		x += pc->xadvance * render_scale;
	}
}

Clay_Dimensions ui_font_clay_measure_text(Clay_StringSlice glyph_vtx_array, Clay_TextElementConfig* config, void* user_data)
{
	//FontAtlas* font_data = (FontAtlas*)user_data;
	FontAtlas* font_data = &font_atlas;

	if(font_data->texture.texture_id == 0)
	{
		log_error("MeasureText cannot do anything when texture is not loaded\n");
		return (Clay_Dimensions){.width=0, .height=0};
	}

	float x = 0.0f;
	float y = 0.0f;

	const char* str = glyph_vtx_array.chars;
	int len = glyph_vtx_array.length;

	float scale = config->fontSize / font_data->baked_font_size;
	float letter_spacing = (float)config->letterSpacing;
	float line_height = (config->lineHeight > 0) ? (float)config->lineHeight : font_data->baked_font_size;

	for(int i = 0; i < len; i++)
	{
		unsigned char c = str[i];

		if(c < 32 || c > 127)
		{
			log_error("Illegal char %d\n", (int)c);
			x += font_data->baked_font_size * 0.25f;
			continue;
		}
        
		stbtt_packedchar* pc = &font_data->glyph_ascii[c - 32];
        
		x += pc->xadvance * scale + letter_spacing;
	}

	float line_h = (font_data->ascent - font_data->descent) * scale;
	return (Clay_Dimensions){.width=x, .height=y+line_h};
}


void ui_font_destroy()
{
	texture2d_destroy(&font_atlas.texture);
}


