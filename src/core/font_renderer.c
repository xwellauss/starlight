#include "font_renderer.h"
#include "game_engine.h"
#include "renderer/renderer.h"
#include "../utils/utils.h"

#include "platform.h"

#include <stb_truetype.h>
#include <stb_ds.h>

#include <cglm/struct.h>

#define FONT_VERTEX_SHADER_PATH "shaders/font-vertex-shader.glsl"
#define FONT_FRAGMENT_SHADER_PATH "shaders/font-fragment-shader.glsl"

typedef struct
{
	Texture2D texture;
	vec2s size;
	unsigned int advance;
} Character;

static struct { char key; Character value; }* characters = NULL;

static stbtt_fontinfo font;
static unsigned char* buffer;

static Shader shader;
static VertexBuffer vertex_buffer;

static Window* current_window;

static GLushort indices[6] =
{
	0, 1, 2, 0, 2, 3
};

mat4s projection = GLMS_MAT4_IDENTITY_INIT;

void font_renderer_init(const char* font_path, int character_size)
{
	current_window = &game_engine.current_window;

	shader_init(&shader, FONT_VERTEX_SHADER_PATH, FONT_FRAGMENT_SHADER_PATH);
	vertex_buffer_init(&vertex_buffer, NULL, sizeof(VertexQuad), indices, sizeof(indices), true);

	//  Loading the .ttf file
	buffer = (unsigned char*)read_file(font_path, "rb");
	stbtt_InitFont(&font, buffer, 0);
	float scale = stbtt_ScaleForPixelHeight(&font, character_size);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	
	for (unsigned char c = 0; c < 128; c++)
	{
		int glyph_index = stbtt_FindGlyphIndex(&font, c);
		int width, height;
		unsigned char* bitmap = stbtt_GetGlyphBitmap(&font, 0, scale, glyph_index, &width, &height, NULL, NULL);

		// generate texture
		Texture2D texture;
		texture.width = width;
		texture.height = height;
		texture.texture_config.is_init = true;
		texture.texture_config.wrap_s = GL_CLAMP_TO_EDGE;
		texture.texture_config.wrap_t = GL_CLAMP_TO_EDGE;
		texture.texture_config.min_filter = GL_LINEAR;
		texture.texture_config.mag_filter = GL_LINEAR;

		texture2d_init_from_data(&texture, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, bitmap);

		stbtt_FreeBitmap(bitmap, NULL);
		texture2d_unbind();
	
		// now store character for later use
		int advance_width, lsb;
    	stbtt_GetGlyphHMetrics(&font, glyph_index, &advance_width, &lsb);

	    Character character = 
		{
    	    texture, 
			(vec2s){width, height},
			(advance_width * scale)
    	};
		hmput(characters, c, character);
	}
	texture2d_unbind();
}

// BUG: Blurry text if scale is not 1.0
void font_renderer_render_text(char* text, float x, float y, float scale, char* hex_color, float opacity)
{
	shader_bind(&shader);
	
	projection = glms_ortho(0.0f, current_window->width, 0.0f, current_window->height, -1.0f, 1.0f);


	shader_uniform_mat4(&shader, "projection", projection);
	shader_uniform_vec4(&shader, "text_color", hex_to_rbg(hex_color, opacity));

	vertex_buffer_bind(&vertex_buffer, BUFFER_VAO);

	for(char* c = text; *c != '\0'; c++)
	{
		Character ch = hmget(characters, *c);

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // update VBO for each character
		/*
			Top Left: 0
			Bottom Left: 1
			Bottom Right: 2
			Top Right: 3

			0 1 2 0 2 3
		*/

		VertexQuad quad[1] =
		{
			(VertexQuad)
			{
				{
				{{x, y + h, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
				{{x, y, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
				{{x + w, y, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
				{{x + w, y + h, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}
				}
			}
		};

		/*
		VertexQuad quad[1] =
		{
			(VertexQuad)
			{
				{
				{{{1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 1.0f}}},
				{{{1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 0.0f}}},
				{{{-1.0f, -1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 0.0f}}},
				{{{-1.0f, 1.0f, 0.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 1.0f}}},
				}
			}
		};
		*/

        // render glyph texture over quad
		texture2d_bind(&ch.texture);
        // update content of VBO memory
		vertex_buffer_bind(&vertex_buffer, BUFFER_VBO);
		vertex_buffer_update(&vertex_buffer, quad, sizeof(quad), 0);
        // render quad
		vertex_buffer_draw_indexed(&vertex_buffer, GL_TRIANGLES, GL_UNSIGNED_SHORT, 6, 0);
        x += (float)ch.advance;
	}

	vertex_buffer_unbind_all();
	shader_unbind();
	texture2d_unbind();
}

void font_renderer_destroy()
{
	for(size_t i = 0; i < hmlen(characters); i++)
	{
		texture2d_destroy(&characters[i].value.texture);
	}

	hmfree(characters);

	vertex_buffer_destroy(&vertex_buffer);
	shader_destroy(&shader);
}
