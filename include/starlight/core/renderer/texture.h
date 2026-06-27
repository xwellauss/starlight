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

typedef struct
{
	bool is_init;

	int min_filter;
	int mag_filter;
	int wrap_s;
	int wrap_t;
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
void texture_set_parameteri(uint32_t target, uint32_t pname, int param);

void texture2d_init_from_file(Texture2D* texture, const char* texture_path);
void texture2d_init_from_data(Texture2D* texture, int level, int internalformat, uint32_t format, uint32_t type, void* texture_data);
void texture2d_bind(Texture2D* texture);
void texture2d_bind_id(uint32_t texture_id);
void texture2d_unbind();
void texture2d_destroy(Texture2D* texture);

void texture2d_map_add_from_file(Texture2DHashMap* textures, const char* texture_name, const char* texture_path);
void texture2d_map_add_from_data(Texture2DHashMap* textures, const char* texture_name, int level, int internalformat, uint32_t format, uint32_t type, void* texture_data);
void texture2d_map_destroy(Texture2DHashMap* textures);

void texture2d_array_destroy(Texture2D** textures);
