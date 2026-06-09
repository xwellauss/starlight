#include "texture_atlas.h"

#include "../utils/json_helper.h"

#include <string.h>
#include <hashmap.h>

struct hashmap* texture_atlas_hashmap;

int texture_atlas_compare(const void* a, const void* b, void* udata)
{
	const TextureAtlas* ua = a;
	const TextureAtlas* ub = b;
	return strcmp(ua->name, ub->name);
}

int atlas_region_compare(const void* a, const void* b, void* udata)
{
	const AtlasRegion* ua = a;
	const AtlasRegion* ub = b;
	return strcmp(ua->name, ub->name);
}

uint64_t texture_atlas_hash(const void* item, uint64_t seed0, uint64_t seed1)
{
	const TextureAtlas* texture_atlas = item;
	return hashmap_sip(texture_atlas->name, strlen(texture_atlas->name), seed0, seed1);
}

uint64_t atlas_region_hash(const void* item, uint64_t seed0, uint64_t seed1)
{
	const AtlasRegion* atlas_region = item;
	return hashmap_sip(atlas_region->name, strlen(atlas_region->name), seed0, seed1);
}

void texture_atlas_init()
{
	texture_atlas_hashmap = hashmap_new(sizeof(TextureAtlas), 0, 0, 0, texture_atlas_hash, texture_atlas_compare, NULL, NULL);

	char* texture_json_string = read_file("texture_atlases.json", "r");
	cJSON* texture_json = json_parse(texture_json_string);

	cJSON* texture_atlases_json = json_get_object(texture_json, "texture_atlases");

	cJSON* texture_atlas_json;
	cJSON_ArrayForEach(texture_atlas_json, texture_atlases_json)
	{
		char* texture_atlas_name = strdup(json_get_string(texture_atlas_json, "name"));
		char* texture_atlas_path = strdup(json_get_string(texture_atlas_json, "path"));
		char* texture_atlas_default_region = strdup(json_get_string(texture_atlas_json, "default_atlas_region_name"));
		int texture_atlas_width = json_get_int(texture_atlas_json, "width");
		int texture_atlas_height = json_get_int(texture_atlas_json, "height");
		int texture_atlas_type = json_get_int(texture_atlas_json, "type");

		struct hashmap* atlas_region_map = hashmap_new(sizeof(AtlasRegion), 0, 0, 0, atlas_region_hash, atlas_region_compare, NULL, NULL);

		cJSON* atlas_regions_json = json_get_object(texture_atlas_json, "atlas_regions");

		cJSON* atlas_region_json;
		cJSON_ArrayForEach(atlas_region_json, atlas_regions_json)
		{
			char* region_name = strdup(json_get_string(atlas_region_json, "name"));
			int region_x = json_get_int(atlas_region_json, "x");
			int region_y = json_get_int(atlas_region_json, "y");
			int region_frame_speed = json_get_int(atlas_region_json, "frame_speed");
			int region_frame_count = json_get_int(atlas_region_json, "frame_count");
			int region_width = json_get_int(atlas_region_json, "width");
			int region_height = json_get_int(atlas_region_json, "height");

			hashmap_set(atlas_region_map, &(AtlasRegion){
				.name=region_name,
				.x=region_x, .y=region_y,
				.frame_speed=region_frame_speed,
				.frame_count=region_frame_count,
				.width=region_width, .height=region_height,
			});
		}

		hashmap_set(texture_atlas_hashmap, &(TextureAtlas){
			.name=texture_atlas_name,
			.path=texture_atlas_path,
			.type=texture_atlas_type,
			.default_region_name=texture_atlas_default_region,
			.width=texture_atlas_width, .height=texture_atlas_height,
			.atlas_region_map=atlas_region_map
		});
	}

	json_delete_object(texture_json);
	free(texture_json_string);
}

void atlas_get_frame(VertexQuad* quad, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame)
{
	float x1, x2, y1, y2;

	if(texture_atlas->type == ATLAS_LAYOUT_HORIZONTAL)
	{
		x1 = (float)(frame * atlas_region->width)/texture_atlas->width;
		x2 = (float)((frame + 1) * atlas_region->width)/texture_atlas->width;
		y1 = (float)(atlas_region->y * atlas_region->height)/texture_atlas->height;
		y2 = (float)((atlas_region->y + 1) * atlas_region->height)/texture_atlas->height;
	}
	if(texture_atlas->type == ATLAS_LAYOUT_VERTICAL)
	{
		x1 = (float)(atlas_region->x * atlas_region->width)/texture_atlas->width;
		x2 = (float)((atlas_region->x + 1) * atlas_region->width)/texture_atlas->width;
		y1 = (float)(frame * atlas_region->height)/texture_atlas->height;
		y2 = (float)((frame + 1) * atlas_region->height)/texture_atlas->height;
	}
	

	quad->vertices[0].tex_coord = (vec2s){{x2, y2}};
	quad->vertices[1].tex_coord = (vec2s){{x2, y1}};
	quad->vertices[2].tex_coord = (vec2s){{x1, y1}};
	quad->vertices[3].tex_coord = (vec2s){{x1, y2}};
}


void atlas_get_frame_verts(Vertex vertex_render_data[], int start, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame)
{
	float x1, x2, y1, y2;

	if(texture_atlas->type == ATLAS_LAYOUT_HORIZONTAL)
	{
		x1 = (float)(frame * atlas_region->width)/texture_atlas->width;
		x2 = (float)((frame + 1) * atlas_region->width)/texture_atlas->width;
		y1 = (float)(atlas_region->y * atlas_region->height)/texture_atlas->height;
		y2 = (float)((atlas_region->y + 1) * atlas_region->height)/texture_atlas->height;
	}
	if(texture_atlas->type == ATLAS_LAYOUT_VERTICAL)
	{
		x1 = (float)(atlas_region->x * atlas_region->width)/texture_atlas->width;
		x2 = (float)((atlas_region->x + 1) * atlas_region->width)/texture_atlas->width;
		y1 = (float)(frame * atlas_region->height)/texture_atlas->height;
		y2 = (float)((frame + 1) * atlas_region->height)/texture_atlas->height;
	}
	
	(vertex_render_data)[start + 0].tex_coord = (vec2s){{x2, y2}};
	(vertex_render_data)[start + 1].tex_coord = (vec2s){{x2, y1}};
	(vertex_render_data)[start + 2].tex_coord = (vec2s){{x1, y1}};

	(vertex_render_data)[start + 3].tex_coord = (vec2s){{x1, y1}};
	(vertex_render_data)[start + 4].tex_coord = (vec2s){{x1, y2}};
	(vertex_render_data)[start + 5].tex_coord = (vec2s){{x2, y2}};
}

void atlas_get_uv(AtlasUV* atlas_uv, TextureAtlas* texture_atlas, AtlasRegion* atlas_region)
{
	atlas_uv->x1 = (float)(atlas_region->x * atlas_region->width)/texture_atlas->width;
	atlas_uv->x2 = (float)((atlas_region->x + 1) * atlas_region->width)/texture_atlas->width;

	atlas_uv->y1 = (float)(atlas_region->y * atlas_region->height)/texture_atlas->height;
	atlas_uv->y2 = (float)((atlas_region->y + 1) * atlas_region->height)/texture_atlas->height;
}

void texture_atlas_destroy()
{
	size_t i = 0;
    void* data;
    while(hashmap_iter(texture_atlas_hashmap, &i, &data))
	{
		TextureAtlas* texture_atlas = data;
		free(texture_atlas->name);
		free(texture_atlas->path);
		free(texture_atlas->default_region_name);

		size_t j = 0;
		void* d;
		while(hashmap_iter(texture_atlas->atlas_region_map, &j, &d))
		{
			AtlasRegion* atlas_region = d;
			free(atlas_region->name);
		}

		hashmap_free(texture_atlas->atlas_region_map);
    }

	hashmap_free(texture_atlas_hashmap);
}
