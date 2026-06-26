#include <starlight/core/renderer/texture.h>
#include <starlight/utils/logger.h>

#include <stb_ds.h>
#include <stb_image.h>

#include "../gl_platform.h"

void texture_active_slot(GLenum slot)
{
	glActiveTexture(slot);
}

void texture_set_parameteri(GLenum target, GLenum pname, GLint param)
{
	glTexParameteri(target, pname, param);
}

void texture2d_init_from_file(Texture2D* texture, const char* texture_path)
{
	stbi_set_flip_vertically_on_load(true);

	int channels;

	unsigned char* texture_data = stbi_load(texture_path, &texture->width, &texture->height, &channels, 4);
	if(!texture_data)
	{
		log_error("Invalid Texture Data: %s\n", texture_path);
	}

	texture2d_init_from_data(texture, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
	
	stbi_image_free(texture_data);

}

void texture2d_init_from_data(Texture2D* texture, GLint level, GLint internalformat, GLenum format, GLenum type, void* texture_data)
{
	texture_active_slot(GL_TEXTURE0);

	glGenTextures(1, &texture->texture_id);
	texture2d_bind(texture);

	if(!texture->texture_config.is_init)
	{
		texture->texture_config.wrap_s = GL_REPEAT;
		texture->texture_config.wrap_t = GL_REPEAT;
		texture->texture_config.min_filter = GL_NEAREST;
		texture->texture_config.mag_filter = GL_NEAREST;
	}

	texture_set_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->texture_config.wrap_s);
	texture_set_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->texture_config.wrap_t);
	texture_set_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->texture_config.min_filter);
	texture_set_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->texture_config.mag_filter);

	glTexImage2D(GL_TEXTURE_2D, level, internalformat, texture->width, texture->height, 0, format, type, texture_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	texture2d_unbind();
}

void texture2d_bind_id(GLuint texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
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

void texture2d_map_add_from_data(Texture2DHashMap* textures, const char* texture_name, GLint level, GLint internalformat, GLenum format, GLenum type, void* texture_data)
{
	Texture2D texture = {};

	texture2d_init_from_data(&texture, level, internalformat, format, type, texture_data);

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
