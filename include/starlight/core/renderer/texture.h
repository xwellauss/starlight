#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum
{
	TEXTURE_SLOT_0 = 0,
	TEXTURE_SLOT_1,
	TEXTURE_SLOT_2,
	TEXTURE_SLOT_3,
	TEXTURE_SLOT_4,
	TEXTURE_SLOT_5,
	TEXTURE_SLOT_6,
	TEXTURE_SLOT_7,
	TEXTURE_SLOT_8,
} TextureSlot;

typedef enum
{
	TEXTURE_FORMAT_RGBA,
	TEXTURE_FORMAT_RGB,
	TEXTURE_FORMAT_RED
} TextureFormat;

typedef enum
{
	TEXTURE_WRAP_REPEAT,
	TEXTURE_WRAP_CLAMP,
	TEXTURE_WRAP_MIRRORED
} TextureWrap;

typedef enum
{
	TEXTURE_FILTER_NEAREST,
	TEXTURE_FILTER_LINEAR
} TextureFilter;

typedef struct
{
	bool is_init;

	TextureFilter min_filter;
	TextureFilter mag_filter;
	TextureWrap wrap_s;
	TextureWrap wrap_t;
} Texture2DConfig;

typedef struct
{
	unsigned int texture_id;
	int width;
	int height;

	Texture2DConfig texture_config;
} Texture2D;

typedef struct { char* key; Texture2D value; }* Texture2DHashMap;

void texture_active_slot(TextureSlot slot);

void texture2d_init_from_file(Texture2D* texture, const char* texture_path);
void texture2d_init_from_bytes(Texture2D* texture, TextureFormat format, const unsigned char* texture_data);
void texture2d_bind(Texture2D* texture);
void texture2d_bind_id(unsigned int id);
void texture2d_unbind();
void texture2d_destroy(Texture2D* texture);

void texture2d_map_add_from_file(Texture2DHashMap* textures, const char* texture_name, const char* texture_path);
void texture2d_map_add_from_bytes(Texture2DHashMap* textures, const char* texture_name, uint32_t format, const unsigned char* texture_data);
void texture2d_map_destroy(Texture2DHashMap* textures);

void texture2d_array_destroy(Texture2D** textures);
