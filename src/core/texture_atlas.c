#include "texture_atlas.h"

#include "../utils/json_helper.h"

#include <string.h>
#include <stb_ds.h>

static struct { char* key; TextureAtlas* value; }* texture_atlas_hashmap = NULL;

void texture_atlas_init()
{
	sh_new_arena(texture_atlas_hashmap);

	cJSON* file_json = json_read_file("texture_atlases.json");
	cJSON* texture_atlases_json = json_get_object(file_json, "texture_atlases");
	
	int texture_atlas_count = json_get_array_length(texture_atlases_json);

	for(size_t i = 0; i < texture_atlas_count; i++)
	{
		cJSON* texture_atlas_json = json_get_array_item(texture_atlases_json, i);

		char* texture_atlas_name = strdup(json_get_string(texture_atlas_json, "name"));
		char* texture_atlas_path = strdup(json_get_string(texture_atlas_json, "path"));
		char* texture_atlas_default_region = strdup(json_get_string(texture_atlas_json, "default_atlas_region_name"));
		int texture_atlas_width = json_get_int(texture_atlas_json, "width");
		int texture_atlas_height = json_get_int(texture_atlas_json, "height");
		int texture_atlas_type = json_get_int(texture_atlas_json, "type");

		TextureAtlas* texture_atlas = malloc(sizeof(TextureAtlas));

		*texture_atlas = (TextureAtlas)
		{
			.name=texture_atlas_name,
			.path=texture_atlas_path,
			.type=texture_atlas_type,
			.default_region_name=texture_atlas_default_region,
			.width=texture_atlas_width, .height=texture_atlas_height,
			.atlas_region_map=NULL
		};

		cJSON* atlas_regions_json = json_get_object(texture_atlas_json, "atlas_regions");
		int atlas_region_count = json_get_array_length(atlas_regions_json);

		for(size_t j = 0; j < atlas_region_count; j++)
		{
			cJSON* atlas_region_json = json_get_array_item(atlas_regions_json, j);

			char* region_name = strdup(json_get_string(atlas_region_json, "name"));
			int region_x = json_get_int(atlas_region_json, "x");
			int region_y = json_get_int(atlas_region_json, "y");
			int region_frame_speed = json_get_int(atlas_region_json, "frame_speed");
			int region_frame_count = json_get_int(atlas_region_json, "frame_count");
			int region_width = json_get_int(atlas_region_json, "width");
			int region_height = json_get_int(atlas_region_json, "height");

			AtlasRegion* atlas_region = malloc(sizeof(AtlasRegion));
			*atlas_region = (AtlasRegion)
			{
				.name=region_name,
				.x=region_x, .y=region_y,
				.frame_speed=region_frame_speed,
				.frame_count=region_frame_count,
				.width=region_width, .height=region_height,
			};

			shput(texture_atlas->atlas_region_map, region_name, atlas_region);
		}
		
		shput(texture_atlas_hashmap, texture_atlas_name, texture_atlas);
	}

	json_delete_object(file_json);
}

TextureAtlas* texture_atlas_get(const char* name)
{
	return shget(texture_atlas_hashmap, name);
}

void atlas_get_frame(Vertex2DQuad* quad, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame)
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
	for(size_t i = 0; i < shlen(texture_atlas_hashmap); i++)
	{
		TextureAtlas* texture_atlas = texture_atlas_hashmap[i].value;
		for(size_t j = 0; j < shlen(texture_atlas->atlas_region_map); j++)
		{
			AtlasRegion* atlas_region = texture_atlas->atlas_region_map[j].value;

			free(atlas_region->name); // free strdup
			free(atlas_region);
		}

		shfree(texture_atlas->atlas_region_map);
		free(texture_atlas->name);
		free(texture_atlas->path);
		free(texture_atlas->default_region_name);
		free(texture_atlas);
	}

	shfree(texture_atlas_hashmap);
}
