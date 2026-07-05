#include <starlight/core/resources/font_atlas.h>
#include <starlight/core/window/window.h>
#include <starlight/utils/logger.h>
#include <starlight/platform/platform.h>


#include <stb_rect_pack.h>
#include <stb_truetype.h>
#include <stb_ds.h>

#include <cglm/struct.h>

void font_atlas_init(FontAtlas* font_atlas, const char* filepath, int atlas_w, int atlas_h, float backed_font_size)
{
	unsigned char* font_file_buffer = (unsigned char*)platform_read_file(filepath, FILE_READ_BINARY, NULL);

	font_atlas->width = atlas_w;
	font_atlas->height = atlas_h;
	font_atlas->baked_font_size = backed_font_size;
	unsigned char* atlas_buffer = (unsigned char*)malloc(font_atlas->width * font_atlas->height);

	stbtt_fontinfo font_info;
	stbtt_InitFont(&font_info, font_file_buffer, 0);


	int ascent, descent, line_gap;
	stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

	float scale_factor = stbtt_ScaleForPixelHeight(&font_info, font_atlas->baked_font_size);
	font_atlas->ascent = ascent * scale_factor;
	font_atlas->descent = descent * scale_factor;
	font_atlas->line_gap = line_gap * scale_factor;

	
	
	stbtt_pack_context font_pack_ctx;
	stbtt_PackBegin(&font_pack_ctx, atlas_buffer, font_atlas->width, font_atlas->height, 0, 1, NULL);

	// ASCII 32-127
	//stbtt_PackSetOversampling(&font_pack_ctx, 2, 2);
	stbtt_PackFontRange(&font_pack_ctx, font_file_buffer, 0, font_atlas->baked_font_size, 32, 96, font_atlas->glyph_ascii);
	
	stbtt_PackEnd(&font_pack_ctx);

	// Upload to Textures
	font_atlas->texture.width = font_atlas->width;
	font_atlas->texture.height = font_atlas->height;
	font_atlas->texture.texture_config.is_init = true;
	font_atlas->texture.texture_config.wrap_s = TEXTURE_WRAP_CLAMP;
	font_atlas->texture.texture_config.wrap_t = TEXTURE_WRAP_CLAMP;
	font_atlas->texture.texture_config.min_filter = TEXTURE_FILTER_NEAREST;
	font_atlas->texture.texture_config.mag_filter = TEXTURE_FILTER_NEAREST;
	
	texture2d_init_from_bytes(&font_atlas->texture, TEXTURE_FORMAT_RED, atlas_buffer);

	free(atlas_buffer);
	free(font_file_buffer);
}

void font_atlas_destroy(FontAtlas* font_atlas)
{
	texture2d_destroy(&font_atlas->texture);
}
