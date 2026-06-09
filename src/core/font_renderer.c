#include "font_renderer.h"
#include "cglm/struct/mat4.h"
#include "game_engine.h"
#include "renderer.h"
#include "../utils/utils.h"

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <GLES3/gl3.h>
#endif

#include <stb_truetype.h>
#include <stb_ds.h>

#define FONT_VERTEX_SHADER_PATH "shaders/font-vertex-shader.glsl"
#define FONT_FRAGMENT_SHADER_PATH "shaders/font-fragment-shader.glsl"

typedef struct
{
	Texture texture;
	vec2s size;
	unsigned int advance;
} Character;

static struct { char key; Character value; }* characters = NULL;

static stbtt_fontinfo font;
static unsigned char* buffer;

static unsigned int shader_program;
static VertexAttributes vertex_attributes;

static Window* current_window;

static GLushort indices[6] =
{
	0, 1, 2, 0, 2, 3
};

mat4s projection = GLMS_MAT4_IDENTITY_INIT;

void font_renderer_init(const char* font_path, int character_size)
{
	current_window = &game_engine.current_window;

	init_shader_program(&shader_program, FONT_VERTEX_SHADER_PATH, FONT_FRAGMENT_SHADER_PATH);
	init_vertex_attributes(&vertex_attributes, NULL, sizeof(Quad), indices, sizeof(indices), true, false);

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
		Texture texture;
		texture.width = width;
		texture.height = height;
		texture.texture_config.is_init = true;
		texture.texture_config.wrap_s = GL_CLAMP_TO_EDGE;
		texture.texture_config.wrap_t = GL_CLAMP_TO_EDGE;
		texture.texture_config.min_filter = GL_LINEAR;
		texture.texture_config.mag_filter = GL_LINEAR;

		init_texture_from_data(&texture, 0, GL_R8, GL_RED, GL_UNSIGNED_BYTE, bitmap);

		stbtt_FreeBitmap(bitmap, NULL);
		unbind_texture();
	
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
	unbind_texture();
}

// BUG: Blurry text if scale is not 1.0
void font_renderer_render_text(char* text, float x, float y, float scale, char* hex_color, float opacity)
{
	bind_shader_program(&shader_program);
	mat4s projection = glms_mat4_identity();
	projection = glms_ortho(0.0f, current_window->width, 0.0f, current_window->height, -1.0f, 1.0f);

	uniform_mat4(&shader_program, "projection", projection);
	uniform_vec4(&shader_program, "text_color", hex_to_rbg(hex_color, opacity));

	bind_vertex_buffer(&vertex_attributes, VAO);

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

		Quad quad[1] =
		{
			(Quad)
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
		Quad quad[1] =
		{
			(Quad)
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
		bind_texture(&ch.texture);
        // update content of VBO memory
		bind_vertex_buffer(&vertex_attributes, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
        // render quad
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        x += (float)ch.advance;
	}

	unbind_vertex_buffer_all();
	unbind_shader_program();
	unbind_texture();
}

void font_renderer_destroy()
{
	for(size_t i = 0; i < hmlen(characters); i++)
	{
		destroy_texture(&characters[i].value.texture);
	}

	hmfree(characters);

	destroy_vertex_attributes(&vertex_attributes, true);
	destroy_shader_program(&shader_program);
}
