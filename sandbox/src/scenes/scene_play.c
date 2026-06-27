#include "scene_play.h"

#include <starlight/core/engine.h>
#include <starlight/core/ecs.h>
#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>
#include <starlight/core/camera.h>
#include <starlight/core/resources/texture_atlas.h>
#include <starlight/utils/logger.h>

#include <math.h>
#include <stdint.h>
#include <time.h>

#include <stb_perlin.h>
#include <stb_ds.h>
#include <cglm/struct.h>

static Camera camera;
static InputState input_state;

static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static vec2s joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static vec4s joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};

// Player
static char* player_current_sprite_name = "";
static TextureAtlas player_texture_atlas;
static Entity player;
static float player_speed = 4.0f;

static Vertex2DQuad player_quad[1] =
{
	(Vertex2DQuad)
	{
		{
		{{{1.0f, 1.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 1.0f}}},
		{{{1.0f, -1.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{1.0f, 0.0f}}},
		{{{-1.0f, -1.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 0.0f}}},
		{{{-1.0f, 1.0f}}, {{0.0f, 0.0f, 0.0f, 0.0f}}, {{0.0f, 1.0f}}},
		}
	}
};

static uint32_t player_indices[6] =
{
	0, 1, 2,
	2, 3, 0
};

// Map
#define MAP_SIZE_X 50
#define MAP_SIZE_Y 50
#define MAP_TILE_COUNT MAP_SIZE_X * MAP_SIZE_Y

static Entity map;
static TextureAtlas map_texture_atlas;
static Vertex2DQuad map_tiles[MAP_TILE_COUNT];
static uint32_t map_indices[MAP_TILE_COUNT * 6];

// Player
static void init_player()
{
	ecs_entity_init(&player, "player");
	ecs_entity_add_component(&player, COMPONENT_TRANSFORM);
	ecs_entity_add_component(&player, COMPONENT_SPRITE);

	ecs_entity_get_transform(&player)->position = (vec3s){0.0f, 0.0f, 0.1f};
	ecs_entity_get_transform(&player)->scale = (vec3s){1.0f, 1.0f, 1.0f};

	texture_atlas_init(&player_texture_atlas, "player.json");

	player_current_sprite_name = player_texture_atlas.default_region_name;

	texture2d_map_add_from_file(&ecs_entity_get_sprite(&player)->textures, "player", player_texture_atlas.path);
	vertex_buffer_2d_init(&ecs_entity_get_sprite(&player)->vertex_buffer, player_quad, sizeof(player_quad), player_indices, sizeof(player_indices), true);
	shader_init_from_file(&ecs_entity_get_sprite(&player)->shader, "shaders/player-vertex-shader.glsl", "shaders/player-fragment-shader.glsl");
}

static void player_update_texcoords()
{
	AtlasRegion* current_sprite = shget(player_texture_atlas.atlas_region_map, player_current_sprite_name);

	static int anim_tick = 0;
	anim_tick++;

	int anim_frame = texture_atlas_get_current_frame(&player_texture_atlas, current_sprite, anim_tick);
	texture_atlas_get_frame_quad(player_quad, &player_texture_atlas, current_sprite, anim_frame);
}

static void draw_player()
{
	shader_bind(&ecs_entity_get_sprite(&player)->shader);

	texture2d_bind(&shget(ecs_entity_get_sprite(&player)->textures, "player"));

	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
	transform = glms_scale(transform, ecs_entity_get_transform(&player)->scale);
	transform = glms_translate(transform, ecs_entity_get_transform(&player)->position);

	shader_uniform_mat4(&ecs_entity_get_sprite(&player)->shader, "projection", camera.projection_matrix);
	shader_uniform_mat4(&ecs_entity_get_sprite(&player)->shader, "view", camera.view_matrix);
	shader_uniform_mat4(&ecs_entity_get_sprite(&player)->shader, "transform", transform);

	player_update_texcoords();

	vertex_buffer_update(&ecs_entity_get_sprite(&player)->vertex_buffer, player_quad, sizeof(player_quad), 0);
	vertex_buffer_draw_indexed(&ecs_entity_get_sprite(&player)->vertex_buffer, PRIMITIVE_TRIANGLES, 6, 0);
}

// Map
static void fill_map()
{
	srand(time(NULL));
	const int seed = rand();

	// Vertices
	for(int y = 0; y < MAP_SIZE_Y; y++)
	{
		for(int x = 0; x < MAP_SIZE_X; x++)
		{
			float noise_x = (float)x/(float)MAP_SIZE_X;
			float noise_y = (float)y/(float)MAP_SIZE_Y;
			float noise_factor = 10.0f;
			float scale = 0.1;
			static char* tile_sprite = "grass";

			float perlin_noise = stb_perlin_noise3_seed(noise_x * noise_factor, noise_y * noise_factor, 0.0f, 0, 0, 0, seed);

			if(perlin_noise < -0.2) tile_sprite = "water";
			if(perlin_noise > -0.1) tile_sprite = "grass";
			if(perlin_noise > 0.3) tile_sprite = "stone";

			AtlasRegion* current_tile_sprite = shget(map_texture_atlas.atlas_region_map, tile_sprite);

			float x1 = (float)(x * current_tile_sprite->width * scale);
			float y1 = (float)(y * current_tile_sprite->height * scale);
			float x2 = (float)((x+1) * current_tile_sprite->width * scale);
			float y2 = (float)((y+1) * current_tile_sprite->height * scale);

			AtlasUV uv_coords;
			texture_atlas_get_uv(&uv_coords, &map_texture_atlas, current_tile_sprite);

			map_tiles[y * MAP_SIZE_X + x] = (Vertex2DQuad)
			{
				{
					{{{x2, y2}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{uv_coords.x2, uv_coords.y2}}},
					{{{x2, y1}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{uv_coords.x2, uv_coords.y1}}},
					{{{x1, y1}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{uv_coords.x1, uv_coords.y1}}},
					{{{x1, y2}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{uv_coords.x1, uv_coords.y2}}},
				}
			};
		}
	}

	// Indices
	for(int i = 0; i < sizeof(map_indices)/sizeof(map_indices[0]); i += 6)
	{
		static int offset = 0;

		map_indices[i + 0] = 0 + offset;
		map_indices[i + 1] = 1 + offset;
		map_indices[i + 2] = 2 + offset;

		map_indices[i + 3] = 2 + offset;
		map_indices[i + 4] = 3 + offset;
		map_indices[i + 5] = 0 + offset;

		offset += 4;
	}
}

static void init_map()
{
	ecs_entity_init(&map, "map");
	ecs_entity_add_component(&map, COMPONENT_TRANSFORM);
	ecs_entity_add_component(&map, COMPONENT_SPRITE);

	ecs_entity_get_transform(&map)->position = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_entity_get_transform(&map)->scale = (vec3s){1.0f, 1.0f, 1.0f};
	
	texture_atlas_init(&map_texture_atlas, "blocks.json");

	texture2d_map_add_from_file(&ecs_entity_get_sprite(&map)->textures, "map", map_texture_atlas.path);

	fill_map();
	vertex_buffer_2d_init(&ecs_entity_get_sprite(&map)->vertex_buffer, map_tiles, sizeof(map_tiles), map_indices, sizeof(map_indices), true);
	shader_init_from_file(&ecs_entity_get_sprite(&map)->shader, "shaders/map-vertex-shader.glsl", "shaders/map-fragment-shader.glsl");
}

static void draw_map()
{
	shader_bind(&ecs_entity_get_sprite(&map)->shader);

	texture2d_bind(&shget(ecs_entity_get_sprite(&map)->textures, "map"));

	mat4s transform = GLMS_MAT4_IDENTITY_INIT;
	transform = glms_scale(transform, (vec3s){{1.0f, 1.0f, 0.0f}});
	transform = glms_translate(transform, (vec3s){{0.0f, 0.0f, 0.0f}});

	shader_uniform_mat4(&ecs_entity_get_sprite(&map)->shader, "projection", camera.projection_matrix);
	shader_uniform_mat4(&ecs_entity_get_sprite(&map)->shader, "view", camera.view_matrix);
	shader_uniform_mat4(&ecs_entity_get_sprite(&map)->shader, "transform", transform);

	vertex_buffer_draw_indexed(&ecs_entity_get_sprite(&map)->vertex_buffer, PRIMITIVE_TRIANGLES, MAP_TILE_COUNT*6, 0);
}

// Scene
static void init()
{
	init_player();
	init_map();

	camera.position = (vec3s){{0.0f, 0.0f, 10.0f}};
	camera.target = (vec3s){{0.0f, 0.0f, 0.0f}};
	camera.up = (vec3s){{0.0f, 1.0f, 0.0f}};
	camera.speed = 5.0f;
	camera.front = (vec3s){{0.0f, 0.0f, -1.0f}};
	camera.fov = glm_rad(45.0f);
	camera.near_plane = 0.1f;
	camera.far_plane = 1000.0f;
	camera.mouse_sensitivity = 0.1f;
	camera.pitch = 0.0f;
	camera.yaw = -90.0f;
	camera.camera_type = CAMERA_NO_MOVE;
	init_camera(&camera);	
}

static void update()
{
	update_camera(&camera);

	bool moving = input_state.left || input_state.right || input_state.forward || input_state.backward;
	float deltatime = engine_get_deltatime();
	float speed = player_speed * deltatime;
	vec3s* pos = &ecs_entity_get_transform(&player)->position;

	if(!moving)
	{
		player_current_sprite_name = player_texture_atlas.default_region_name;
	}

	if(input_state.forward)
	{
		pos->y += speed;
		player_current_sprite_name = "move-up";
	}
	if(input_state.backward)
	{
		pos->y -= speed;
		player_current_sprite_name = "move-down";
	}
	if(input_state.left)
	{
		pos->x -= speed;
		player_current_sprite_name = "move-left";
	}
	if(input_state.right)
	{
		pos->x += speed;
		player_current_sprite_name = "move-right";
	}

	move_camera(&camera, input_state);

	camera.position.x = pos->x;
	camera.position.y = pos->y;
}

static void build_ui()
{
}

static void render()
{
#if defined(_PLATFORM_ANDROID) || defined(_PLATFORM_WEB)
	//ui_component_joystick("Input", "Joystick", joystick_box_size, joystick_radius, joystick_color, &joystick_angle, &is_joystick_active);
#endif

	draw_map();
	draw_player();
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
	texture2d_map_destroy(&ecs_entity_get_sprite(&map)->textures);
	shader_destroy(&ecs_entity_get_sprite(&map)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&map)->vertex_buffer);
	
	texture2d_map_destroy(&ecs_entity_get_sprite(&player)->textures);
	shader_destroy(&ecs_entity_get_sprite(&player)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&player)->vertex_buffer);

	ecs_entity_destroy(&player);
	ecs_entity_destroy(&map);
}

Scene scene_play = {"ScenePlay", init, destroy, activate, deactivate, update, render, build_ui, process_input};
