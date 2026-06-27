#pragma once

#include <starlight/core/renderer/renderer.h>

#include <cglm/struct.h>

typedef enum
{
	ATLAS_LAYOUT_HORIZONTAL = 0,
	ATLAS_LAYOUT_VERTICAL = 1,
} AtlasLayout;

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
	AtlasLayout layout;
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

void texture_atlas_init(TextureAtlas* texture_atlas, const char* atlas_json_path);
void texture_atlas_destroy(TextureAtlas* texture_atlas);

int texture_atlas_get_current_frame(TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int tick);
void texture_atlas_get_uv(AtlasUV* atlas_uv, TextureAtlas* texture_atlas, AtlasRegion* atlas_region);
void texture_atlas_get_frame_verts(Vertex2D vertex_render_data[], int start, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame);
void texture_atlas_get_frame_quad(Vertex2DQuad* quad, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame);
