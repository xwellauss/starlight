#include "model_loader.h"
#include "gltf.h"
#include "../renderer.h"



void model_load_from_file(Model* model, const char* filepath, enum ModelFileType filetype)
{
	memset(model, 0, sizeof(Model));
	model->meshes = NULL;
	model->vertex_data = NULL;
	model->textures = NULL;

	switch (filetype)
	{
	case FILE_GLTF:
		gltf_load_file(model, filepath);
		break;
	}

	model->vertex_count = arrlen(model->vertex_data);
	model->index_count = arrlen(model->index_data);
	model->mesh_count = arrlen(model->meshes);
}

void model_free(Model* model)
{
	destroy_textures_array(&model->textures);
}

