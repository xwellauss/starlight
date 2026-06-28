#include <starlight/core/renderer/texture.h>
#include <starlight/platform/platform.h>
#include <starlight/utils/logger.h>

#include <stb_ds.h>
#include <stb_image.h>

#include "gl_platform.h"

static void texture_set_parameteri(GLenum target, GLenum pname, GLint param)
{
	glTexParameteri(target, pname, param);
}

static GLenum texture_format_to_gl(TextureFormat format)
{
	switch(format)
	{
		case TEXTURE_FORMAT_RGBA: return GL_RGBA;
		case TEXTURE_FORMAT_RGB: return GL_RGB;
		case TEXTURE_FORMAT_RED: return GL_RED;
	}
}

static GLint texture_format_to_gl_internal(TextureFormat format)
{
	switch(format)
	{
		case TEXTURE_FORMAT_RGBA: return GL_RGBA8;
		case TEXTURE_FORMAT_RGB: return GL_RGB8;
		case TEXTURE_FORMAT_RED: return GL_R8;
	}
}

static GLint texture_wrap_to_gl(TextureWrap wrap)
{
	switch(wrap)
	{
		case TEXTURE_WRAP_REPEAT: return GL_REPEAT;
		case TEXTURE_WRAP_CLAMP: return GL_CLAMP_TO_EDGE;
		case TEXTURE_WRAP_MIRRORED: return GL_MIRRORED_REPEAT;
	}
}

static GLint texture_filter_to_gl(TextureFilter filter)
{
	switch(filter)
	{
		case TEXTURE_FILTER_NEAREST: return GL_NEAREST;
		case TEXTURE_FILTER_LINEAR: return GL_LINEAR;
	}
}

void texture_active_slot(TextureSlot slot)
{
	glActiveTexture(GL_TEXTURE0+slot);
}

void texture2d_init_from_file(Texture2D* texture, const char* texture_path)
{
	stbi_set_flip_vertically_on_load(true);

	size_t buffer_size;
	unsigned char* buffer = (unsigned char*)platform_read_file(texture_path, FILE_READ_BINARY, &buffer_size);

	int channels;
	unsigned char* texture_data = stbi_load_from_memory(buffer, buffer_size, &texture->width, &texture->height, &channels, 4);
	free(buffer);

	if(!texture_data)
	{
		log_error("Invalid Texture Data: %s\n", texture_path);
	}

	texture2d_init_from_bytes(texture, TEXTURE_FORMAT_RGBA, texture_data);
	
	stbi_image_free(texture_data);
}

void texture2d_init_from_bytes(Texture2D* texture, TextureFormat format, const unsigned char* texture_data)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
	texture_active_slot(TEXTURE_SLOT_0);

	glGenTextures(1, &texture->texture_id);
	texture2d_bind(texture);

	if(!texture->texture_config.is_init)
	{
		texture->texture_config.wrap_s = TEXTURE_WRAP_REPEAT;
		texture->texture_config.wrap_t = TEXTURE_WRAP_REPEAT;
		texture->texture_config.min_filter = TEXTURE_FILTER_NEAREST;
		texture->texture_config.mag_filter = TEXTURE_FILTER_NEAREST;
	}

	texture_set_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_wrap_to_gl(texture->texture_config.wrap_s));
	texture_set_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_wrap_to_gl(texture->texture_config.wrap_t));
	texture_set_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_filter_to_gl(texture->texture_config.min_filter));
	texture_set_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_filter_to_gl(texture->texture_config.mag_filter));

	glTexImage2D(GL_TEXTURE_2D, 0, texture_format_to_gl_internal(format), texture->width, texture->height, 0, texture_format_to_gl(format), GL_UNSIGNED_BYTE, texture_data);
	// TODO: Handle mipmaps
	//glGenerateMipmap(GL_TEXTURE_2D);

	texture2d_unbind();
}

void texture2d_bind_id(unsigned int id)
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void texture2d_bind(Texture2D* texture)
{
	texture2d_bind_id(texture->texture_id);
}

void texture2d_unbind()
{
	texture2d_bind_id(0);
}

void texture2d_destroy(Texture2D* texture)
{
	glDeleteTextures(1, &texture->texture_id);
}

void texture2d_map_add_from_file(Texture2DHashMap* textures, const char* texture_name, const char* texture_path)
{
	Texture2D texture = {};

	texture2d_init_from_file(&texture, texture_path);

	shput(*textures, texture_name, texture);
}

void texture2d_map_add_from_bytes(Texture2DHashMap* textures, const char* texture_name, uint32_t format, const unsigned char* texture_data)
{
	Texture2D texture = {};

	texture2d_init_from_bytes(&texture, format, texture_data);

	shput(*textures, texture_name, texture);

}

void texture2d_map_destroy(Texture2DHashMap* textures)
{
	for(int i = 0; i < shlen(*textures); i++)
	{
		texture2d_destroy(&((*textures)[i].value));
	}
	
	shfree(*textures);
	*textures = NULL;

}

void texture2d_array_destroy(Texture2D** textures)
{
	for(int i = 0; i < arrlen(*textures); i++)
	{
		texture2d_destroy(&(*textures)[i]);
	}

	arrfree(*textures);
	*textures = NULL;
}
