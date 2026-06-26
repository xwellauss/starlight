#pragma once

#include <starlight/core/renderer/renderer.h>

#include <cglm/struct.h>

enum AtlasLayout
{
	ATLAS_LAYOUT_HORIZONTAL = 0,
	ATLAS_LAYOUT_VERTICAL = 1,
};

typedef struct
{
	char* name;
	int x;
	int y;
	int frame_speed;
	int frame_count;
	int width;
	int height;
} AtlasRegion;

typedef struct
{
	char* name;
	char* path;
	char* default_region_name;
	enum AtlasLayout type;
	int width;
	int height;
	struct { char* key; AtlasRegion* value; }* atlas_region_map;
} TextureAtlas;

typedef struct
{
	float x1;
	float x2;
	float y1;
	float y2;
} AtlasUV;

typedef struct { char* key; TextureAtlas* value; }* TextureAtlasHashMap;

void texture_atlas_init(TextureAtlasHashMap* texture_atlas_hashmap);
void texture_atlas_destroy(TextureAtlasHashMap* texture_atlas_hashmap);

TextureAtlas* texture_atlas_get(TextureAtlasHashMap* texture_atlas_hashmap, const char* name);


void atlas_get_uv(AtlasUV* atlas_uv, TextureAtlas* texture_atlas, AtlasRegion* atlas_region);
void atlas_get_frame_verts(Vertex vertex_render_data[], int start, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame);
void atlas_get_frame(Vertex2DQuad* quad, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame);
