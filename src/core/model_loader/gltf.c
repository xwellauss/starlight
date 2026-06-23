#include "gltf.h"

#include <stb_ds.h>
#include <stb_image.h>

typedef struct
{
	cgltf_material* gltf_material;
	Texture2DHashMap* texture_hashmap;

	float metallic_factor;
	float roughness_factor;
} GLTF_Material;



static void gltf_parse_primitive(Model* model, Mesh* model_mesh, cgltf_data* gltf_data, cgltf_primitive* gltf_primtive)
{
	size_t vertex_count;
	size_t index_count;

	cgltf_accessor* position_accessor = NULL;
	cgltf_accessor* color_accessor = NULL;
	cgltf_accessor* tex_coord_accessor = NULL;
	cgltf_accessor* normal_accessor = NULL;

	for(size_t j = 0; j < gltf_primtive->attributes_count; j++)
	{
		vertex_count = gltf_primtive->attributes[j].data->count;

		switch(gltf_primtive->attributes[j].type)
		{
		case cgltf_attribute_type_position:
			position_accessor = gltf_primtive->attributes[j].data;
			break;
		case cgltf_attribute_type_color:
			color_accessor = gltf_primtive->attributes[j].data;
			break;
		case cgltf_attribute_type_texcoord:
			tex_coord_accessor = gltf_primtive->attributes[j].data;
			break;
		case cgltf_attribute_type_normal:
			normal_accessor = gltf_primtive->attributes[j].data;
			break;
		default:
			break;
		}
	}

	if (!position_accessor) return;

	model_mesh->vertex_offset = arrlen(model->vertex_data);
	model_mesh->index_offset = arrlen(model->index_data);

	vertex_count = position_accessor->count;
	model_mesh->vertex_count = vertex_count;
	
	if(gltf_primtive->indices)
	{
		index_count = gltf_primtive->indices->count;
		model_mesh->index_count = index_count;

		for(size_t j = 0; j < index_count; j++)
		{
			GLuint local_index = (GLuint)cgltf_accessor_read_index(gltf_primtive->indices, j);
			GLuint global_index = local_index + model_mesh->vertex_offset;
			arrput(model->index_data, global_index);
		}
	}

	if(gltf_primtive->material)
	{
		size_t material_index = cgltf_material_index(gltf_data, gltf_primtive->material);
		model_mesh->material_index = material_index;
	}
	else
	{
		model_mesh->material_index = -1;
	}


	for(size_t k = 0; k < position_accessor->count; k++)
	{
		Vertex vertex;
		memset(&vertex, 0, sizeof(Vertex));

		if(position_accessor)
		{
			float pos[3] = {0};
			cgltf_accessor_read_float(position_accessor, k, pos, 3);
			vec4s transformed_position = glms_mat4_mulv(model_mesh->transform, (vec4s){pos[0], pos[1], pos[2], 1.0f});
			vertex.position = (vec3s){transformed_position.x, transformed_position.y, transformed_position.z};
		}

		if(color_accessor)
		{
			float col[4] = {0.0f, 0.0f, 0.0f, 1.0f};
			cgltf_accessor_read_float(color_accessor, k, col, 4);
			vertex.color = (vec4s){col[0], col[1], col[2], col[3]};
			glms_vec4_print(vertex.color, stdout);
		}
		else
		{
			vertex.color = (vec4s){1.0f, 1.0f, 1.0f, 1.0f};
		}
		
		if(tex_coord_accessor)
		{
			float tex[2] = {0};
			cgltf_accessor_read_float(tex_coord_accessor, k, tex, 2);
			vertex.tex_coord = (vec2s){tex[0], tex[1]};
		}

		if(normal_accessor)
		{
			float normal[3] = {0};
			cgltf_accessor_read_float(normal_accessor, k, normal, 3);
			vertex.normal = (vec3s){normal[0], normal[1], normal[2]};
		}

		arrput(model->vertex_data, vertex);
	}
}

static void gltf_parse_mesh(Model* model, cgltf_data* gltf_data, cgltf_node* gltf_node)
{
	cgltf_mesh* gltf_mesh = gltf_node->mesh;

	Mesh model_mesh = {};
	memset(&model_mesh, 0, sizeof(Mesh));

	if(gltf_mesh->name)
	{
		snprintf(model_mesh.name, sizeof(model_mesh.name), "%s", gltf_mesh->name);
		log_debug("Loading Mesh: %s\n", model_mesh.name);
	}

	float matrix[16];
	cgltf_node_transform_local(gltf_node, matrix);

	model_mesh.transform = glms_mat4_mul(GLMS_MAT4_IDENTITY, *(mat4s*)matrix);

	for(size_t i = 0; i < gltf_mesh->primitives_count; i++)
	{
		gltf_parse_primitive(model, &model_mesh, gltf_data, &gltf_mesh->primitives[i]);
	}

	arrput(model->meshes, model_mesh);

	model->vertex_count = arrlen(model->vertex_data);
	model->index_count = arrlen(model->index_data);
	model->mesh_count = arrlen(model->meshes);
}


static void gltf_parse_node(Model* model, cgltf_data* gltf_data, cgltf_node* gltf_node)
{
	if(gltf_node->mesh)
	{
		gltf_parse_mesh(model, gltf_data, gltf_node);
	}

	for(size_t i = 0; i < gltf_node->children_count; i++)
	{
		gltf_parse_node(model, gltf_data, gltf_node->children[i]);
	}
}

static void gltf_parse_image(cgltf_image* image, Texture2D* texture)
{
	stbi_set_flip_vertically_on_load(false);

	if(image->uri)
	{
		texture2d_init_from_file(texture, image->uri);
		// FIXME: the image path
	}
	else if(image->buffer_view)
	{
		unsigned char* image_data = (unsigned char*)(image->buffer_view->buffer->data + image->buffer_view->offset);
		size_t image_size = image->buffer_view->size;

		int channels;
		unsigned char* data = stbi_load_from_memory(image_data, image_size, &texture->width, &texture->height, &channels, 4);

		texture2d_init_from_data(texture, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
}

static void gltf_parse_texture(Model* model, cgltf_texture* gltf_texture, size_t texture_index)
{
	Texture2D texture;
	
	if(gltf_texture->sampler)
	{
		texture.texture_config.is_init = true;
		texture.texture_config.wrap_s = gltf_texture->sampler->wrap_s;
		texture.texture_config.wrap_t = gltf_texture->sampler->wrap_t;
		texture.texture_config.min_filter = gltf_texture->sampler->min_filter;
		texture.texture_config.mag_filter = gltf_texture->sampler->mag_filter;
	}

	gltf_parse_image(gltf_texture->image, &texture);

	arrput(model->textures, texture);
}

static void gltf_parse_material(Model* model, cgltf_data* gltf_data, cgltf_material* gltf_material)
{
	Material material = {};

	if(gltf_material->has_pbr_metallic_roughness)
	{
		material.base_color.r = gltf_material->pbr_metallic_roughness.base_color_factor[0];
		material.base_color.g = gltf_material->pbr_metallic_roughness.base_color_factor[1];
		material.base_color.b = gltf_material->pbr_metallic_roughness.base_color_factor[2];
		material.base_color.a = gltf_material->pbr_metallic_roughness.base_color_factor[3];

		material.metallic_factor = gltf_material->pbr_metallic_roughness.metallic_factor;
		material.roughness_factor = gltf_material->pbr_metallic_roughness.roughness_factor;

		if(gltf_material->pbr_metallic_roughness.base_color_texture.texture)
		{
			size_t texture_index = cgltf_texture_index(gltf_data, gltf_material->pbr_metallic_roughness.base_color_texture.texture);

			Texture2D texture = model->textures[texture_index];

			material.albedo_texture_id = texture.texture_id;
		}

		if(gltf_material->pbr_metallic_roughness.metallic_roughness_texture.texture)
		{
			size_t texture_index = cgltf_texture_index(gltf_data, gltf_material->pbr_metallic_roughness.metallic_roughness_texture.texture);

			Texture2D texture = model->textures[texture_index];

			material.metallic_roughness_texture_id = texture.texture_id;
		}

		if(gltf_material->normal_texture.texture)
		{
			size_t texture_index = cgltf_texture_index(gltf_data, gltf_material->normal_texture.texture);

			Texture2D texture = model->textures[texture_index];

			material.normal_texture_id = texture.texture_id;
		}
	}
	else
	{
		material.base_color = (vec4s){1.0f, 1.0f, 1.0f, 1.0f};
		material.roughness_factor = 1.0f;
		material.metallic_factor = 1.0f;
	}


	arrput(model->materials, material);
}

// Load only the first scene for now
static void gltf_parse_data(Model* model, cgltf_data* gltf_data)
{
	cgltf_scene* gltf_scene = gltf_data->scene;

	for(size_t i = 0; i < gltf_data->textures_count; i++)
	{
		cgltf_texture* gltf_texture = &gltf_data->textures[i];
		
		gltf_parse_texture(model, gltf_texture, i);
	}

	for(size_t i = 0; i < gltf_data->materials_count; i++)
	{
		cgltf_material* gltf_material = &gltf_data->materials[i];
		gltf_parse_material(model, gltf_data, gltf_material);
	}

	for(size_t i = 0; i < gltf_scene->nodes_count; i++)
	{
		gltf_parse_node(model, gltf_data, gltf_scene->nodes[i]);
	}
}

void gltf_load_file(Model* model, const char* filepath)
{
	cgltf_options options = {0}; // Default for now
	cgltf_data* data = NULL;

	cgltf_result result = cgltf_parse_file(&options, filepath, &data);

	if(result == cgltf_result_success)
	{
		cgltf_load_buffers(&options, data, filepath);

		gltf_parse_data(model, data);

		cgltf_free(data);
	}
	else
	{
		log_debug("Couldn't Load GLTF Model: %d\n", result);

		cgltf_free(data);
	}
}

void gltf_model_free(Model *model)
{
}

/*

void gltf_load_image(cgltf_image* image, Texture* texture)
{
	if(image->uri)
	{
		init_texture_from_file(texture, image->uri);
	}
	else if(image->buffer_view)
	{
		unsigned char* image_data = (unsigned char*)(image->buffer_view->buffer->data + image->buffer_view->offset);
		size_t image_size = image->buffer_view->size;

		int channels;
		unsigned char* data = stbi_load_from_memory(image_data, image_size, &texture->width, &texture->height, &channels, 4);

		init_texture_from_data(texture, 0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
}


void gltf_load_texture(cgltf_texture* gltf_texture, TextureHashMap** textures, const char* texture_name)
{
	Texture texture;
	
	if(gltf_texture->sampler)
	{
		texture.texture_config.is_init = true;
		texture.texture_config.wrap_s = gltf_texture->sampler->wrap_s;
		texture.texture_config.wrap_t = gltf_texture->sampler->wrap_t;
		texture.texture_config.min_filter = gltf_texture->sampler->min_filter;
		texture.texture_config.mag_filter = gltf_texture->sampler->mag_filter;
	}

	gltf_load_image(gltf_texture->image, &texture);

	shput(*textures, texture_name, texture);
}

void gltf_load_mesh(Model* model, cgltf_node* gltf_node)
{
	Mesh mesh = {};
	mesh.mesh = gltf_node->mesh;
	cgltf_mesh* gltf_mesh = mesh.mesh;

	float matrix[16];
	cgltf_node_transform_local(gltf_node, matrix);

	mesh.mesh_transform = glms_mat4_mul(GLMS_MAT4_IDENTITY, *(mat4s*)matrix);

	if(mesh.mesh)
	{
		for(size_t i = 0; i < gltf_mesh->primitives_count; i++)
		{
			const cgltf_primitive* primitive = &gltf_mesh->primitives[i];
			size_t vertex_count;
			size_t index_count;

			const cgltf_accessor* position_accessor = NULL;
			const cgltf_accessor* color_accessor = NULL;
			const cgltf_accessor* tex_coord_accessor = NULL;
			const cgltf_accessor* normal_accessor = NULL;

	       	for(size_t j = 0; j < primitive->attributes_count; j++)
			{
				switch(primitive->attributes[j].type)
				{
				case cgltf_attribute_type_position:
		           	position_accessor = primitive->attributes[j].data;
					vertex_count = position_accessor->count;
					mesh.vertex_count = position_accessor->count;
    	    		break;
		        case cgltf_attribute_type_color:
    		       	color_accessor = primitive->attributes[j].data;
	    		    break;
    	    	case cgltf_attribute_type_texcoord:
		       	    tex_coord_accessor = primitive->attributes[j].data;
    		       	break;
				case cgltf_attribute_type_normal:
	        	    normal_accessor = primitive->attributes[j].data;
	           		break;
		        default:
    			       break;
				}
		    }

			if(primitive->material)
			{
				log_debug("Loading Material: %s\n", primitive->material->name);
				mesh.material.gltf_material = primitive->material;

				if(mesh.material.gltf_material->has_pbr_metallic_roughness)
				{
					mesh.material.metallic_factor = mesh.material.gltf_material->pbr_metallic_roughness.metallic_factor;
					mesh.material.roughness_factor = mesh.material.gltf_material->pbr_metallic_roughness.roughness_factor;

					//mesh.material.base_color.r = mesh.material.gltf_material->pbr_metallic_roughness.base_color_factor[0];
					/mesh.material.base_color.g = mesh.material.gltf_material->pbr_metallic_roughness.base_color_factor[1];
					//mesh.material.base_color.b = mesh.material.gltf_material->pbr_metallic_roughness.base_color_factor[2];
					//mesh.material.base_color.a = mesh.material.gltf_material->pbr_metallic_roughness.base_color_factor[3];
					

					if(mesh.material.gltf_material->pbr_metallic_roughness.base_color_texture.texture)
					{
						gltf_load_texture(mesh.material.gltf_material->pbr_metallic_roughness.base_color_texture.texture, &mesh.material.texture_hashmap, "base_color");
					}

					if(mesh.material.gltf_material->pbr_metallic_roughness.metallic_roughness_texture.texture)
					{
						gltf_load_texture(mesh.material.gltf_material->pbr_metallic_roughness.metallic_roughness_texture.texture, &mesh.material.texture_hashmap, "metallic_roughness");
					}
				}

				if(mesh.material.gltf_material->normal_texture.texture)
				{
					gltf_load_texture(mesh.material.gltf_material->normal_texture.texture, &mesh.material.texture_hashmap, "normal");
				}

				if(mesh.material.gltf_material->occlusion_texture.texture)
				{
					gltf_load_texture(mesh.material.gltf_material->occlusion_texture.texture, &mesh.material.texture_hashmap, "occlusion");
				}
			}


			if(primitive->indices)
    	    {
				index_count = primitive->indices->count;
				mesh.index_count = index_count;
				model->index_count += index_count;
			
//				mesh.index_data = (GLuint*)(primitive->indices->buffer_view->buffer->data + primitive->indices->buffer_view->offset);
				mesh.index_data = malloc(sizeof(unsigned int) * mesh.index_count);
				for(size_t i = 0; i < mesh.index_count; i++)
				{
					((unsigned int*)mesh.index_data)[i] = cgltf_accessor_read_index(primitive->indices, i);
				}
		    }
 
			for(size_t k = 0; k < vertex_count; k++)
			{
				Vertex vertex;

				if(position_accessor)
					{
						float pos[3] = {0};
	  					cgltf_accessor_read_float(position_accessor, k, pos, 3);
						vec4s transformed_position = glms_mat4_mulv(mesh.mesh_transform, (vec4s){pos[0], pos[1], pos[2], 1.0f});
						vertex.position = (vec3s){transformed_position.x, transformed_position.y, transformed_position.z};
					}

					if(color_accessor)
					{
			    	    float col[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        				cgltf_accessor_read_float(color_accessor, k, col, 4);
			    	    vertex.color = (vec4s){col[0], col[1], col[2], col[3]};
						glms_vec4_print(vertex.color, stdout);
					}
					else
					{
						vertex.color = (vec4s){1.0f, 1.0f, 1.0f, 1.0f};
					}

			    	if(tex_coord_accessor)
					{
    	    			float tex[2] = {0};
				        cgltf_accessor_read_float(tex_coord_accessor, k, tex, 2);
    	    			vertex.tex_coord = (vec2s){tex[0], tex[1]};
			    	}

					if(normal_accessor)
					{
						float normal[3] = {0};
	  					cgltf_accessor_read_float(normal_accessor, k, normal, 3);
						vertex.normal = (vec3s){normal[0], normal[1], normal[2]};
					}

					arrput(mesh.vertex_render_data, vertex);
				}
			}
		
			init_vertex_attributes(&mesh.vertex_attribs, mesh.vertex_render_data, mesh.vertex_count*sizeof(Vertex), mesh.index_data, mesh.index_count*sizeof(unsigned int), true);

			shput(model->meshes_hashmap, gltf_mesh->name, mesh);
		}
}

// Sussy Code!

void gltf_model_load_node(Model* model, cgltf_node* node)
{
	log_debug("Loading node %s\n", node->name);
	gltf_load_mesh(model, node);

	for(size_t i = 0; i < node->children_count; i++)
	{
		cgltf_node* node_child = node->children[i];
		gltf_load_mesh(model, node_child);
	}
}

void gltf_model_parse_data(Model* model)
{
	cgltf_scene* scene = model->data->scene;

	for(size_t i = 0; i < scene->nodes_count; i++)
	{
		gltf_model_load_node(model, scene->nodes[i]);
	}
}


void model_free(Model* model)
{
//	free(model->index_data);

	switch (model->type)
	{
	case MODEL_GLTF:

		for(size_t i = 0; i < hmlen(model->meshes_hashmap); i++)
		{
			Mesh* mesh = &model->meshes_hashmap[i].value;
			
			destroy_textures(mesh->material.texture_hashmap);
			if(mesh->index_data)
			{
				free(mesh->index_data);
			}
		}

		hmfree(model->meshes_hashmap);
		cgltf_free(model->data);
		break;
	}
}

*/


