#include "scene_block.h"

#include <starlight/core/engine.h>
#include <starlight/core/ecs.h>
#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>
#include <starlight/core/camera.h>
#include <starlight/core/resources/texture_atlas.h>
#include <starlight/utils/logger.h>

#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#include <cglm/struct.h>
#include <stb_ds.h>
#include <stb_perlin.h>

static vec4s background_color = (vec4s){0.0f, 0.0f, 0.0f, 1.0f};

typedef struct
{
	vec3s position;
	bool is_transparent;
	Vertex3D* vertex_data;
} Block;

static Block* blocks;

static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static vec2s joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static vec4s joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};

static Camera camera;
static InputState input_state;

static Vertex3D* vertex_render_data = NULL;
//static vec3s* block_positions;

static TextureAtlas blocks_texture_atlas;

static Entity chunk;

//static void add_block(vec3s origin, float block_size, char* sprite_name, Vertex** vertex_data)
static void add_block(vec3s origin, float block_size, char* block_name, bool is_transparent)
{
	Block block = {};
	block.position = origin;
	block.is_transparent = is_transparent;

	vec3s cube_vertices_positions[8] =
	{
		(vec3s){-block_size, block_size, block_size},
		(vec3s){block_size, block_size, block_size},
		(vec3s){-block_size, -block_size, block_size},
		(vec3s){block_size, -block_size, block_size},
		(vec3s){-block_size, block_size, -block_size},
		(vec3s){block_size, block_size, -block_size},
		(vec3s){-block_size, -block_size, -block_size},
		(vec3s){block_size, -block_size, -block_size},
	};

	uint32_t cube_indices[36] =
	{
		1, 0, 2, 3, 1, 2,    // Back face
		5, 1, 3, 7, 5, 3,    // Right face
		7, 6, 4, 5, 7, 4,    // Front face
		0, 4, 6, 2, 0, 6,    // Left face
		5, 4, 0, 1, 5, 0,    // Top face
		3, 2, 6, 7, 3, 6     // Bottom face
	};

	AtlasRegion* atlas_region = shget(blocks_texture_atlas.atlas_region_map, block_name);

	AtlasUV uv_coords;
	texture_atlas_get_uv(&uv_coords, &blocks_texture_atlas, atlas_region);

	vec2s tex_coords[6];
	tex_coords[0] = (vec2s){uv_coords.x2, uv_coords.y2};
	tex_coords[1] = (vec2s){uv_coords.x1, uv_coords.y2};
	tex_coords[2] = (vec2s){uv_coords.x1, uv_coords.y1};

	tex_coords[4] = (vec2s){uv_coords.x2, uv_coords.y2};
	tex_coords[3] = (vec2s){uv_coords.x2, uv_coords.y1};
	tex_coords[5] = (vec2s){uv_coords.x1, uv_coords.y1};

	for(int i = 0; i < sizeof(cube_indices)/sizeof(cube_indices[0]); i++)
	{
		Vertex3D vertex;

		if(block.is_transparent)
		{
			vertex = (Vertex3D){};
			arrput(block.vertex_data, vertex);
			continue;
		}
		else
		{
			vertex.position = glms_vec3_add(cube_vertices_positions[cube_indices[i]], origin);
			vertex.color = (vec4s){0.0f, 0.0f, 0.0f, 0.0f};
			vertex.tex_coord = tex_coords[i % 6];
			vertex.normal = (vec3s){0.0f, 0.0f, 0.0f};

			//arrput(*vertex_data, vertex);
			arrput(block.vertex_data, vertex);
		}
	}

	arrput(blocks, block);
}

static void cull_faces(Block* block, float offset)
{
	// Linear Seach ;)
	for(int i = 0; i < arrlen(blocks); i++)
	{
		if(block->is_transparent || blocks[i].is_transparent)
		{
			continue;
		}

		if(glms_vec3_eqv((vec3s){block->position.x-offset, block->position.y, block->position.z}, blocks[i].position)) // left
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[18 + j] = (Vertex3D){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x+offset, block->position.y, block->position.z}, blocks[i].position)) // right
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[6 + j] = (Vertex3D){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x, block->position.y+offset, block->position.z}, blocks[i].position)) // Top
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[24 + j] = (Vertex3D){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x, block->position.y-offset, block->position.z}, blocks[i].position)) // Bottom
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[30 + j] = (Vertex3D){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x, block->position.y, block->position.z-offset}, blocks[i].position)) // Front
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[12 + j] = (Vertex3D){};
			}
		}
		if(glms_vec3_eqv((vec3s){block->position.x, block->position.y, block->position.z+offset}, blocks[i].position)) // Back
		{
			for(int j = 0; j < 6; j++)
			{
				block->vertex_data[0 + j] = (Vertex3D){};
			}
		}
	}
}

static void generate_map()
{
	int BLOCKS_X = 16;
	int BLOCKS_Y = 5;
	int BLOCKS_Z = 16;
	float HALF_BLOCK_SIZE = 1.0f;

	srand(time(NULL));
	const int seed = rand();

	if(blocks)
	{
		blocks = NULL;
	}

	for(int z = 0; z < BLOCKS_Z; z++)
	{
		for(int y = 0; y < BLOCKS_Y; y++)
		{
			for(int x = 0; x < BLOCKS_X; x++)
			{
				vec3s current_block_origin = glms_vec3_scale((vec3s){x, y, z}, 2*HALF_BLOCK_SIZE);

				float noise_x = (float)x/(float)BLOCKS_X;
				float noise_y = (float)y/(float)BLOCKS_Y;
				float noise_z = (float)z/(float)BLOCKS_Z;
				float noise_factor = 1.0f;

				static char* block_sprite = "grass";
				static bool block_is_transparent = false;

				float perlin_noise = stb_perlin_noise3_seed(noise_x * noise_factor, noise_y * noise_factor, noise_z * noise_factor, 0, 0, 0, seed);
//				log_debug("Perlin Noise: %f\n", perlin_noise);

				if(perlin_noise >= 0.2f)
				{
					block_sprite = "grass";
					block_is_transparent = true;
				}
				else if(perlin_noise >= 0.1f)
				{
					block_sprite = "grass";
					block_is_transparent = false;
				}	
				else if(perlin_noise >= 0.0f && perlin_noise <= 0.1f)
				{
					block_sprite = "water";
					block_is_transparent = false;
				}
				else if(perlin_noise < 0.0f)
				{
					block_sprite = "stone";
					block_is_transparent = false;
				}

				add_block(current_block_origin, HALF_BLOCK_SIZE, block_sprite, block_is_transparent);
			}
		}
	}

	for(int i = 0; i < arrlen(blocks); i++)
	{
		cull_faces(&blocks[i], 2 * HALF_BLOCK_SIZE);
	}

	if(vertex_render_data)
	{
		vertex_render_data = NULL;
	}

	for(int i = 0; i < arrlen(blocks); i++)
	{
		for(int j = 0; j < arrlen(blocks[i].vertex_data); j++)
		{
			arrput(vertex_render_data, blocks[i].vertex_data[j]);
		}
	}
}

static void init()
{
	ecs_entity_init(&chunk, "Chunk");
	ecs_entity_add_component(&chunk, COMPONENT_TRANSFORM);
	ecs_entity_add_component(&chunk, COMPONENT_SPRITE);

	texture_atlas_init(&blocks_texture_atlas, "blocks.json");

	generate_map();

	texture2d_map_add_from_file(&ecs_entity_get_sprite(&chunk)->textures, "block", blocks_texture_atlas.path);
	vertex_buffer_3d_init(&ecs_entity_get_sprite(&chunk)->vertex_buffer, vertex_render_data, sizeof(Vertex3D)*arrlen(vertex_render_data), NULL, 0, false);
	shader_init_from_file(&ecs_entity_get_sprite(&chunk)->shader, "shaders/block-vertex-shader.glsl", "shaders/block-fragment-shader.glsl");

	camera.position = (vec3s){{10.0f, 20.0f, 10.0f}};
	camera.target = (vec3s){{0.0f, 0.0f, 0.0f}};
	camera.up = (vec3s){{0.0f, 1.0f, 0.0f}};
	camera.front = (vec3s){{0.0f, 0.0f, -1.0f}};
	camera.speed = 5.0f;
	camera.fov = glm_rad(45.0f);
	camera.near_plane = 0.1f;
	camera.far_plane = 1000.0f;
	camera.mouse_sensitivity = 0.1f;
	camera.pitch = 0.0f;
	camera.yaw = -90.0f;
	camera.camera_type =  CAMERA_WALK_AROUND | CAMERA_LOOK_AROUND;
	init_camera(&camera);
}

static void update()
{
	update_camera(&camera);
	move_camera(&camera, input_state);
}

static void process_input()
{
	input_state.forward = window_input_key_is_down(INPUT_KEY_UP) || window_input_key_is_down(INPUT_KEY_W);
	input_state.backward = window_input_key_is_down(INPUT_KEY_DOWN) || window_input_key_is_down(INPUT_KEY_S);
	input_state.left = window_input_key_is_down(INPUT_KEY_LEFT) || window_input_key_is_down(INPUT_KEY_A);
	input_state.right = window_input_key_is_down(INPUT_KEY_RIGHT) || window_input_key_is_down(INPUT_KEY_D);
	input_state.up = window_input_key_is_down(INPUT_KEY_SPACE);
	input_state.down = window_input_key_is_down(INPUT_KEY_LEFT_CONTROL);

	// Joystick
	if(is_joystick_active)
	{
		if(joystick_angle >= 315.0f || joystick_angle <= 45.0f) input_state.right = true;
		else if(joystick_angle >= 45.0f && joystick_angle <= 135.0f) input_state.forward = true;
		else if(joystick_angle >= 135.0f && joystick_angle <= 225.0f) input_state.left = true;
		else if(joystick_angle >= 225.0f && joystick_angle <= 315.0f) input_state.backward = true;
	}
}

static void build_ui()
{
}

static void render()
{
#if defined(_PLATFORM_ANDROID) || defined(_PLATFORM_WEB)
	//ui_component_joystick("Input", "Joystick", joystick_box_size, joystick_radius, joystick_color, &joystick_angle, &is_joystick_active);
#endif
	
	const float frequency = 0.5f;
	background_color.r = 0.5f + 0.5f * sin(frequency * window_get_time());
	background_color.b = 0.5f + 0.5f * sin(frequency * window_get_time() + 2.0f * M_PI / 3.0f);
	background_color.g = 0.5f + 0.5f * sin(frequency * window_get_time() + 4.0f * M_PI / 3.0f);
	window_change_bgcolor(background_color);
	
	texture_active_slot(TEXTURE_SLOT_0);
	texture2d_bind(&shget(ecs_entity_get_sprite(&chunk)->textures, "block"));
	
	shader_bind(&ecs_entity_get_sprite(&chunk)->shader);
	shader_uniform_mat4(&ecs_entity_get_sprite(&chunk)->shader, "projection", camera.projection_matrix);
	shader_uniform_mat4(&ecs_entity_get_sprite(&chunk)->shader, "view", camera.view_matrix);
	shader_uniform_int(&ecs_entity_get_sprite(&chunk)->shader, "texture_sampler", 0);

	vertex_buffer_update(&ecs_entity_get_sprite(&chunk)->vertex_buffer, vertex_render_data, sizeof(Vertex3D)*arrlen(vertex_render_data), 0);
	vertex_buffer_draw(&ecs_entity_get_sprite(&chunk)->vertex_buffer, PRIMITIVE_TRIANGLES, arrlen(vertex_render_data), 0);	
}

static void activate()
{
}

static void deactivate()
{
	vertex_buffer_unbind_all();
	shader_unbind();
	
	texture_active_slot(TEXTURE_SLOT_1);
	texture2d_unbind();

	texture_active_slot(TEXTURE_SLOT_0);
	texture2d_unbind();
}

static void destroy()
{
	texture_atlas_destroy(&blocks_texture_atlas);
	arrfree(vertex_render_data);

	texture2d_map_destroy(&ecs_entity_get_sprite(&chunk)->textures);
	shader_destroy(&ecs_entity_get_sprite(&chunk)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&chunk)->vertex_buffer);

	ecs_entity_destroy(&chunk);
}


Scene scene_block = {"SceneBlock", init, destroy, activate, deactivate, update, render, build_ui, process_input};
