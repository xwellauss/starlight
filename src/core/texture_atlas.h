#pragma once

#include "../utils/utils.h"
#include "../core/renderer/vertex_buffer.h"

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

void texture_atlas_init();
void texture_atlas_destroy();

TextureAtlas* texture_atlas_get(const char* name);


void atlas_get_uv(AtlasUV* atlas_uv, TextureAtlas* texture_atlas, AtlasRegion* atlas_region);
void atlas_get_frame_verts(Vertex vertex_render_data[], int start, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame);
void atlas_get_frame(Vertex2DQuad* quad, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame);
