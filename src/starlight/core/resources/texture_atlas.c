#include <starlight/core/resources/texture_atlas.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/json_helper.h>

#include <string.h>
#include <stb_ds.h>

static void compute_frame_uv(AtlasUV* uv, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame)
{
	if(texture_atlas->layout == ATLAS_LAYOUT_HORIZONTAL)
	{
		uv->x1 = (float)(frame * atlas_region->width)/texture_atlas->width;
		uv->x2 = (float)((frame + 1) * atlas_region->width)/texture_atlas->width;
		uv->y1 = (float)(atlas_region->y * atlas_region->height)/texture_atlas->height;
		uv->y2 = (float)((atlas_region->y + 1) * atlas_region->height)/texture_atlas->height;
	}
	else if(texture_atlas->layout == ATLAS_LAYOUT_VERTICAL)
	{
		uv->x1 = (float)(atlas_region->x * atlas_region->width)/texture_atlas->width;
		uv->x2 = (float)((atlas_region->x + 1) * atlas_region->width)/texture_atlas->width;
		uv->y1 = (float)(frame * atlas_region->height)/texture_atlas->height;
		uv->y2 = (float)((frame + 1) * atlas_region->height)/texture_atlas->height;
	}
}


void texture_atlas_init(TextureAtlas* texture_atlas, const char* atlas_json_path)
{
	cJSON* json = json_read_file(atlas_json_path);

	*texture_atlas = (TextureAtlas)
	{
		.name=strdup(json_get_string(json, "name")),
		.path=strdup(json_get_string(json, "path")),
		.default_region_name=strdup(json_get_string(json, "default_region")),
		.layout=json_get_int(json, "layout"),
		.width=json_get_int(json, "width"),
		.height=json_get_int(json, "height"),
		.atlas_region_map=NULL
	};

	cJSON* atlas_regions_json = json_get_object(json, "atlas_regions");
	int atlas_region_count = json_get_array_length(atlas_regions_json);

	for(size_t j = 0; j < atlas_region_count; j++)
	{
		cJSON* atlas_region_json = json_get_array_item(atlas_regions_json, j);

		AtlasRegion* atlas_region = malloc(sizeof(AtlasRegion));
		*atlas_region = (AtlasRegion)
		{
			.name=strdup(json_get_string(atlas_region_json, "name")),
			.x=json_get_int(atlas_region_json, "x"),
			.y=json_get_int(atlas_region_json, "y"),
			.frame_speed=json_get_int(atlas_region_json, "frame_speed"),
			.frame_count=json_get_int(atlas_region_json, "frame_count"),
			.width=json_get_int(atlas_region_json, "width"),
			.height=json_get_int(atlas_region_json, "height"),
		};

		shput(texture_atlas->atlas_region_map, atlas_region->name, atlas_region);
	}

	json_delete_object(json);
}

int texture_atlas_get_current_frame(TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int tick)
{
	int start = texture_atlas->layout == ATLAS_LAYOUT_HORIZONTAL ? atlas_region->x : atlas_region->y;

	return start + ((tick/atlas_region->frame_speed) % atlas_region->frame_count);
}

void texture_atlas_get_frame_quad(Vertex2DQuad* quad, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame)
{
	AtlasUV uv;
	compute_frame_uv(&uv, texture_atlas, atlas_region, frame);

	quad->vertices[0].tex_coord = (vec2s){{uv.x2, uv.y2}};
	quad->vertices[1].tex_coord = (vec2s){{uv.x2, uv.y1}};
	quad->vertices[2].tex_coord = (vec2s){{uv.x1, uv.y1}};
	quad->vertices[3].tex_coord = (vec2s){{uv.x1, uv.y2}};
}


void texture_atlas_get_frame_verts(Vertex2D vertex_render_data[], int start, TextureAtlas* texture_atlas, AtlasRegion* atlas_region, int frame)
{
	AtlasUV uv;
	compute_frame_uv(&uv, texture_atlas, atlas_region, frame);

	
	(vertex_render_data)[start + 0].tex_coord = (vec2s){{uv.x2, uv.y2}};
	(vertex_render_data)[start + 1].tex_coord = (vec2s){{uv.x2, uv.y1}};
	(vertex_render_data)[start + 2].tex_coord = (vec2s){{uv.x1, uv.y1}};

	(vertex_render_data)[start + 3].tex_coord = (vec2s){{uv.x1, uv.y1}};
	(vertex_render_data)[start + 4].tex_coord = (vec2s){{uv.x1, uv.y2}};
	(vertex_render_data)[start + 5].tex_coord = (vec2s){{uv.x2, uv.y2}};
}

void texture_atlas_get_uv(AtlasUV* atlas_uv, TextureAtlas* texture_atlas, AtlasRegion* atlas_region)
{
	int frame = texture_atlas->layout == ATLAS_LAYOUT_HORIZONTAL ? atlas_region->x : atlas_region->y;
	compute_frame_uv(atlas_uv, texture_atlas, atlas_region, frame);
}

void texture_atlas_destroy(TextureAtlas* texture_atlas)
{
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
}
