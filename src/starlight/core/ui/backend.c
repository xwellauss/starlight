#include "internal.h"

#include <starlight/core/renderer/renderer.h>
#include <starlight/core/resources/font_atlas.h>
#include <starlight/core/window/window.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

#include <stb_ds.h>

#include "shaders.h"

#define MAX_RECTS 1024
#define MAX_GLYPHS 65536

typedef struct
{
	vec2s position;
	vec4s color;
	vec2s norm_pos;
	vec2s rect_pos;
	vec2s rect_size;
	vec4s corner_radius;
	vec4s border_width;
} RectVertex;

typedef struct
{
	RectVertex vertices[6];
} RectQuad;

typedef struct
{
	Vertex2D vertices[6];
} GlyphQuad;

static float window_width, window_height;

static mat4s projection = GLMS_MAT4_IDENTITY_INIT;

static VertexBuffer rect_vertex_buffer;
static Shader rect_shader;
static FontAtlas font_atlas;

static RectQuad rect_vertex_data[MAX_RECTS];
static size_t rect_count = 0;

static VertexBuffer glyph_vertex_buffer;
static Shader glyph_shader;

static GlyphQuad glyph_vertex_data[MAX_GLYPHS];
static size_t glyph_count = 0;


static Clay_Dimensions clay_measure_text(Clay_StringSlice glyph_vtx_array, Clay_TextElementConfig* config, void* user_data)
{
	if(font_atlas.texture.texture_id == 0)
	{
		log_error("MeasureText cannot do anything when texture is not loaded\n");
		return (Clay_Dimensions){.width=0, .height=0};
	}

	float x = 0.0f;
	float y = 0.0f;

	const char* str = glyph_vtx_array.chars;
	int len = glyph_vtx_array.length;

	float scale = config->fontSize / font_atlas.baked_font_size;
	float letter_spacing = (float)config->letterSpacing;
	float line_height = (config->lineHeight > 0) ? (float)config->lineHeight : font_atlas.baked_font_size;

	for(int i = 0; i < len; i++)
	{
		unsigned char c = str[i];

		if(c < 32 || c > 127)
		{
			log_error("Illegal char %d\n", (int)c);
			x += font_atlas.baked_font_size * 0.25f;
			continue;
		}

		stbtt_packedchar* pc = &font_atlas.glyph_ascii[c - 32];

		x += pc->xadvance * scale + letter_spacing;
	}

	float line_h = (font_atlas.ascent - font_atlas.descent) * scale;
	return (Clay_Dimensions){.width=x, .height=y+line_h};
}


static void flush_rects()
{
	if(rect_count == 0) return;

	shader_bind(&rect_shader);
	vertex_buffer_update(&rect_vertex_buffer, rect_vertex_data, rect_count * sizeof(RectQuad), 0);
	vertex_buffer_draw(&rect_vertex_buffer, PRIMITIVE_TRIANGLES, rect_count * 6, 0);
	shader_unbind();

	rect_count = 0;
}

static void flush_glyphs()
{
	if(glyph_count == 0) return;

	shader_bind(&glyph_shader);

	texture_active_slot(TEXTURE_SLOT_0);
	shader_uniform_int(&glyph_shader, "sampler", 0);
	texture2d_bind(&font_atlas.texture);

	vertex_buffer_update(&glyph_vertex_buffer, glyph_vertex_data, glyph_count*sizeof(GlyphQuad), 0);
	vertex_buffer_draw(&glyph_vertex_buffer, PRIMITIVE_TRIANGLES, glyph_count * 6, 0);
	shader_unbind();

	glyph_count = 0;
}

static void push_rect(float x0, float x1, float y0, float y1, vec4s color, vec2s rect_pos, vec2s rect_size, vec4s corner_radius, vec4s border_width)
{
	// 0 1 2 0 2 3
    if(rect_count >= MAX_RECTS) return;
    rect_vertex_data[rect_count++] = (RectQuad){{
        {{x0, y0}, color, {0.0f, 0.0f}, rect_pos, rect_size, corner_radius, border_width},
        {{x1, y0}, color, {1.0f, 0.0f}, rect_pos, rect_size, corner_radius, border_width},
        {{x1, y1}, color, {1.0f, 1.0f}, rect_pos, rect_size, corner_radius, border_width},

        {{x0, y0}, color, {0.0f, 0.0f}, rect_pos, rect_size, corner_radius, border_width},
        {{x1, y1}, color, {1.0f, 1.0f}, rect_pos, rect_size, corner_radius, border_width},
        {{x0, y1}, color, {0.0f, 1.0f}, rect_pos, rect_size, corner_radius, border_width},
    }};
}

static void push_glyph(float x0, float x1, float y0, float y1, float u0, float u1, float v0, float v1, vec4s color)
{
	// 0 1 2 0 2 3
    if(glyph_count >= MAX_GLYPHS) return;
    glyph_vertex_data[glyph_count++] = (GlyphQuad)
	{{
		{{x0, y0}, color, {u0, v0}},
		{{x1, y0}, color, {u1, v0}},
		{{x1, y1}, color, {u1, v1}},

		{{x0, y0}, color, {u0, v0}},
		{{x1, y1}, color, {u1, v1}},
		{{x0, y1}, color, {u0, v1}},
    }};
}


static void build_glyphs(const char* text, float x, float y, float requested_scale, vec4s color)
{
	float render_scale = requested_scale / font_atlas.baked_font_size;
	y += font_atlas.ascent * render_scale;

	for(const char* c = text; *c != '\0'; c++)
	{
		if(glyph_count >= MAX_GLYPHS) break;

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

		push_glyph(x0, x1, y0, y1, u0, u1, v0, v1, color);

		x += pc->xadvance * render_scale;
	}
}

FontAtlas* ui_backend_font_get_current()
{
	return &font_atlas;
}

void ui_backend_init(const char* font_path)
{
	font_atlas_init(&font_atlas, font_path, 1024, 1024, 45.0f);
	Clay_SetMeasureTextFunction(clay_measure_text, NULL);

	{
		VertexAttrib attribs[] =
		{
			{0, 2, ATTRIB_TYPE_FLOAT, offsetof(RectVertex, position)},
			{1, 4, ATTRIB_TYPE_FLOAT, offsetof(RectVertex, color)},
			{2, 2, ATTRIB_TYPE_FLOAT, offsetof(RectVertex, norm_pos)},
			{3, 2, ATTRIB_TYPE_FLOAT, offsetof(RectVertex, rect_pos)},
			{4, 2, ATTRIB_TYPE_FLOAT, offsetof(RectVertex, rect_size)},
			{5, 4, ATTRIB_TYPE_FLOAT, offsetof(RectVertex, corner_radius)},
			{6, 4, ATTRIB_TYPE_FLOAT, offsetof(RectVertex, border_width)},
		};

		VertexLayout layout = {attribs, 7, sizeof(RectVertex)};
		vertex_buffer_init_with_layout(&rect_vertex_buffer, NULL, MAX_RECTS * sizeof(RectQuad), NULL, 0, false, layout);

		shader_init_from_source(&rect_shader, UI_RECT_VERTEX_SHADER, UI_RECT_FRAGMENT_SHADER);
	}

	vertex_buffer_2d_init(&glyph_vertex_buffer, NULL, MAX_GLYPHS * sizeof(GlyphQuad), NULL, 0, false);
	shader_init_from_source(&glyph_shader, UI_FONT_VERTEX_SHADER, UI_FONT_FRAGMENT_SHADER);
}

void ui_backend_render(Clay_RenderCommandArray cmds)
{
	window_width = (float)window_get_width();
	window_height = (float)window_get_height();

	projection = glms_ortho(0.0f, window_width, window_height, 0.0f, -1.0f, 1.0f); // Top left origin

	shader_bind(&rect_shader);
	shader_uniform_mat4(&rect_shader, "projection", projection);
	shader_bind(&glyph_shader);
	shader_uniform_mat4(&glyph_shader, "projection", projection);
	shader_unbind();

	RendererState r_state = renderer_save_state();
	renderer_set_depth_test(false);
	renderer_set_depth_write(false);

	for(int i = 0; i < cmds.length; i++)
	{
		Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&cmds, i);
		Clay_BoundingBox bounding_box = (Clay_BoundingBox)
		{
			.x = roundf(cmd->boundingBox.x),
			.y = roundf(cmd->boundingBox.y),
			.width = roundf(cmd->boundingBox.width),
			.height = roundf(cmd->boundingBox.height),
		};

		bool scissor_changed = false;

		switch(cmd->commandType)
		{
		case CLAY_RENDER_COMMAND_TYPE_TEXT:
		{
			if(glyph_count >= MAX_GLYPHS) break;

			const Clay_TextRenderData* tr = &cmd->renderData.text;

			vec4s color;
			color.r = tr->textColor.r/255.0f;
			color.g = tr->textColor.g/255.0f;
			color.b = tr->textColor.b/255.0f;
			color.a = tr->textColor.a/255.0f;

			Clay_StringSlice ss = tr->stringContents;
			const char* txt = ss.chars;

			float x = cmd->boundingBox.x;
			float y = cmd->boundingBox.y;

			build_glyphs(txt, x, y, tr->fontSize, color);

			break;
		}
        case CLAY_RENDER_COMMAND_TYPE_BORDER:
		case CLAY_RENDER_COMMAND_TYPE_RECTANGLE:
		case CLAY_RENDER_COMMAND_TYPE_IMAGE:
		{
			if(rect_count >= MAX_RECTS) break;

			float x0 = bounding_box.x;
			float y0 = bounding_box.y;
			float x1 = bounding_box.x + bounding_box.width;
			float y1 = bounding_box.y + bounding_box.height;

			vec2s rect_pos = {x0, y0};
			vec2s rect_size =  {bounding_box.width, bounding_box.height};


			bool is_border = cmd->commandType == CLAY_RENDER_COMMAND_TYPE_BORDER;
			bool is_rect = cmd->commandType == CLAY_RENDER_COMMAND_TYPE_RECTANGLE;
			bool is_image = cmd->commandType == CLAY_RENDER_COMMAND_TYPE_IMAGE;

			vec4s color, corner_radius, border_width;

			if(is_border)
			{
				Clay_BorderRenderData* bd = &cmd->renderData.border;

				color.r = bd->color.r/255.0f;
				color.g = bd->color.g/255.0f;
				color.b = bd->color.b/255.0f;
				color.a = bd->color.a/255.0f;

				corner_radius = (vec4s){bd->cornerRadius.topLeft, bd->cornerRadius.topRight, bd->cornerRadius.bottomLeft, bd->cornerRadius.bottomRight};
				border_width = (vec4s){bd->width.top, bd->width.right, bd->width.bottom, bd->width.left};

			}
			else if(is_rect || is_image)
			{
				Clay_RectangleRenderData* rd = &cmd->renderData.rectangle;

				color.r = rd->backgroundColor.r/255.0f;
				color.g = rd->backgroundColor.g/255.0f;
				color.b = rd->backgroundColor.b/255.0f;
				color.a = rd->backgroundColor.a/255.0f;

				corner_radius = (vec4s){rd->cornerRadius.topLeft, rd->cornerRadius.topRight, rd->cornerRadius.bottomLeft, rd->cornerRadius.bottomRight};
				border_width = (vec4s){0.0f, 0.0f, 0.0f, 0.0f};
			}

			if(is_image)
			{
				flush_rects();
				flush_glyphs();

				Texture2D* texture = (Texture2D*)cmd->renderData.image.imageData;

				push_rect(x0, x1, y0, y1, color, rect_pos, rect_size, corner_radius, border_width);
				shader_bind(&rect_shader);
				shader_uniform_int(&rect_shader, "has_texture", true);
				texture_active_slot(TEXTURE_SLOT_0);
				shader_uniform_int(&rect_shader, "sampler", 0);
				texture2d_bind(texture);
				flush_rects();
				shader_bind(&rect_shader);
				shader_uniform_int(&rect_shader, "has_texture", false);
			}
			else if(is_rect || is_border)
			{
				push_rect(x0, x1, y0, y1, color, rect_pos, rect_size, corner_radius, border_width);
			}

			break;
		}
		case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
		{
			scissor_changed = true;
			break;
		}
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
		{
            scissor_changed = true;
			break;
		}
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM:
		{
			break;
		}
        default:
		{
			break;
		}
		}

		if (i == cmds.length - 1 || scissor_changed)
		{
			flush_rects();
			flush_glyphs();

			if(scissor_changed)
			{
				if(cmd->commandType == CLAY_RENDER_COMMAND_TYPE_SCISSOR_START)
				{
					int x = (int)bounding_box.x;
					int y = (int)(window_height - (bounding_box.y + bounding_box.height));

					renderer_scissor_begin(x, y, (size_t)bounding_box.width, (size_t)bounding_box.height);
				}
				else
				{
					renderer_scissor_end();
				}
			}
		}
	}

	renderer_restore_state(r_state);
}

void ui_backend_destroy()
{
	font_atlas_destroy(&font_atlas);

	vertex_buffer_destroy(&rect_vertex_buffer);
	shader_destroy(&rect_shader);

	vertex_buffer_destroy(&glyph_vertex_buffer);
	shader_destroy(&glyph_shader);
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
