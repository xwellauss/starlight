#pragma once

#include "../renderer.h"
#include "../../utils/utils.h"


enum ModelFileType
{
	FILE_GLTF = 1 << 0,
};

typedef struct
{
	char name[64];

	vec4s base_color;
	float roughness_factor;
	float metallic_factor;

	int albedo_texture_id;
	int metallic_roughness_texture_id;
	int normal_texture_id;
} Material;

typedef struct
{
	char name[256];

	size_t vertex_count;
	size_t index_count;
	size_t vertex_offset;
	size_t index_offset;

	int material_index;

	mat4s transform;
} Mesh;

typedef struct
{
	Mesh* meshes;
	size_t mesh_count;

	Vertex* vertex_data;
	size_t vertex_count;

	GLuint* index_data;
	size_t index_count;

	Material* materials;

	vec4s color; // for testing and debugging
	
	Texture2D* textures;
} Model;


void model_load_from_file(Model* model, const char* filepath, enum ModelFileType filetype);

void model_free(Model* model);
