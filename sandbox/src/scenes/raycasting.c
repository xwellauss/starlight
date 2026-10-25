#include "raycasting.h"

#include <starlight/core/engine.h>
#include <starlight/core/window/input.h>
#include <starlight/core/window/window.h>
#include <starlight/core/renderer/renderer.h>
#include <starlight/core/ecs.h>
#include <starlight/utils/math_utils.h>
#include <starlight/utils/logger.h>

#include <stdbool.h>

// https://lodev.org/cgtutor/raycasting.html

#define map_width 24
#define map_height 24

static int map[map_width][map_height] =
{
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

static Entity line_entity;
static InputState input_state;

static mat4s projection;
static mat4s view;

static Vertex2D line_vertices[2];

static uint32_t line_indices[2] =
{
	0, 1
};

static double pos_x = 22;
static double pos_y = 12;
static double dir_x = -1;
static double dir_y = 0;
static double plane_x = 0;
static double plane_y = 0.66;

static double move_speed = 5.0;
static double rot_speed = 3.0;

static void init_line()
{
	ecs_entity_init(&line_entity, "line");
	ecs_entity_add_component(&line_entity, COMPONENT_SPRITE);

	vertex_buffer_2d_init(&ecs_entity_get_sprite(&line_entity)->vertex_buffer, line_vertices, sizeof(line_vertices), line_indices, sizeof(line_indices), true);
	shader_init_from_file(&ecs_entity_get_sprite(&line_entity)->shader, "shaders/raycaster/line-vertex-shader.glsl", "shaders/raycaster/line-fragment-shader.glsl");
}

static void draw_line(float x, float start, float end, vec4s color)
{
	line_vertices[0].color = color;
	line_vertices[0].position.x = x;
	line_vertices[0].position.y = start;

	line_vertices[1].color = color;
	line_vertices[1].position.x = x;
	line_vertices[1].position.y = end;

	shader_bind(&ecs_entity_get_sprite(&line_entity)->shader);

	shader_uniform_mat4(&ecs_entity_get_sprite(&line_entity)->shader, "projection", projection);
	shader_uniform_mat4(&ecs_entity_get_sprite(&line_entity)->shader, "view", view);

	vertex_buffer_update(&ecs_entity_get_sprite(&line_entity)->vertex_buffer, line_vertices, sizeof(line_vertices), 0);
	vertex_buffer_draw_indexed(&ecs_entity_get_sprite(&line_entity)->vertex_buffer, PRIMITIVE_LINES, 2, 0);
}


static void init()
{
	init_line();

	projection = glms_ortho(0.0f, (float)window_get_width(), (float)window_get_height(), 0.0f, -1.0f, 1.0f);
	view = (mat4s)GLMS_MAT4_IDENTITY_INIT;
}

static void activate()
{
}

static void update()
{
	double dt = engine_get_deltatime();
	double ms = move_speed * dt;
	double rs = rot_speed * dt;

	projection = glms_ortho(0.0f, (float)window_get_width(), (float)window_get_height(), 0.0f, -1.0f, 1.0f);

	if(input_state.forward)
	{
		if(map[(int)(pos_x + dir_x * ms)][(int)(pos_y)] == false) pos_x += dir_x * ms;
		if(map[(int)(pos_x)][(int)(pos_y + dir_y * ms)] == false) pos_y += dir_y * ms;
	}

	if(input_state.backward)
	{
		if(map[(int)(pos_x - dir_x * ms)][(int)(pos_y)] == false) pos_x -= dir_x * ms;
		if(map[(int)(pos_x)][(int)(pos_y - dir_y * ms)] == false) pos_y -= dir_y * ms;
	}

	if(input_state.right)
	{
		double old_dir_x = dir_x;
		dir_x = dir_x * cos(-rs) - dir_y * sin(-rs);
		dir_y = old_dir_x * sin(-rs) + dir_y * cos(-rs);

		double old_plane_x = plane_x;
		plane_x = plane_x * cos(-rs) - plane_y * sin(-rs);
		plane_y = old_plane_x * sin(-rs) + plane_y * cos(-rs);
	}
	if(input_state.left)
	{
		double old_dir_x = dir_x;
		dir_x = dir_x * cos(rs) - dir_y * sin(rs);
		dir_y = old_dir_x * sin(rs) + dir_y * cos(rs);

		double old_plane_x = plane_x;
		plane_x = plane_x * cos(rs) - plane_y * sin(rs);
		plane_y = old_plane_x * sin(rs) + plane_y * cos(rs);
	}
}

static void build_ui()
{
}

static void render()
{
	int w = window_get_width();
	for(int x = 0; x < w; x++)
	{
		double camera_x = 2 * x/((double)w) - 1;
		double ray_dir_x = dir_x + plane_x * camera_x;
		double ray_dir_y = dir_y + plane_y * camera_x;

		int map_x = (int)pos_x;
		int map_y = (int)pos_y;

		double side_dist_x, side_dist_y;

		double delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1/ray_dir_x);
		double delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1/ray_dir_y);

		double perp_wall_dist;
		int step_x, step_y;

		int hit = 0;
		int side;

		if(ray_dir_x < 0)
		{
			step_x = -1;
			side_dist_x = (pos_x - map_x) * delta_dist_x;
		}
		else
		{
			step_x = 1;
			side_dist_x = (map_x - pos_x + 1) * delta_dist_x;
		}

		if(ray_dir_y < 0)
		{
			step_y = -1;
			side_dist_y = (pos_y - map_y) * delta_dist_y;
		}
		else
		{
			step_y = 1;
			side_dist_y = (map_y - pos_y + 1) * delta_dist_y;
		}

		while(hit == 0)
		{
			if(side_dist_x < side_dist_y)
			{
				side_dist_x += delta_dist_x;
				map_x += step_x;
				side = 0;
			}
			else
			{
				side_dist_y += delta_dist_y;
				map_y += step_y;
				side = 1;
			}

			if(map[map_x][map_y] > 0) hit = 1;
		}

		if(side == 0) perp_wall_dist = (side_dist_x -delta_dist_x);
		else perp_wall_dist = (side_dist_y - delta_dist_y);

		int h = window_get_height();

		int line_height = (int)(h/perp_wall_dist);
		
		int draw_start = (-line_height+h)/2;
		if(draw_start < 0) draw_start = 0;
		int draw_end = (line_height+h)/2;
		if(draw_end >= h) draw_end = h - 1;

		vec4s wall_color;
		switch(map[map_x][map_y])
		{
			case 1: wall_color = hex_to_rgba("#ff0000", 1.0f); break;
			case 2: wall_color = hex_to_rgba("#00ff00", 1.0f); break;
			case 3: wall_color = hex_to_rgba("#0000ff", 1.0f); break;
			case 4: wall_color = hex_to_rgba("#ffffff", 1.0f); break;
			default: wall_color = hex_to_rgba("#ffff00", 1.0f); break;
		}

		if(side == 1)
		{
			wall_color.r /= 2.0f;
			wall_color.g /= 2.0f;
			wall_color.b /= 2.0f;
		}

		draw_line(x, draw_start, draw_end, wall_color);
	}
}

static void process_input()
{
	input_state.forward = window_input_key_is_down(INPUT_KEY_UP) || window_input_key_is_down(INPUT_KEY_W);
	input_state.backward = window_input_key_is_down(INPUT_KEY_DOWN) || window_input_key_is_down(INPUT_KEY_S);
	input_state.left = window_input_key_is_down(INPUT_KEY_LEFT) || window_input_key_is_down(INPUT_KEY_A);
	input_state.right = window_input_key_is_down(INPUT_KEY_RIGHT) || window_input_key_is_down(INPUT_KEY_D);
	input_state.up = window_input_key_is_down(INPUT_KEY_SPACE);
	input_state.down = window_input_key_is_down(INPUT_KEY_LEFT_CONTROL);
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
	shader_destroy(&ecs_entity_get_sprite(&line_entity)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&line_entity)->vertex_buffer);
	ecs_entity_destroy(&line_entity);
}

Scene scene_raycasting = {"SceneRaycasting", .init=init, .destroy=destroy, .activate=activate, .deactivate=deactivate, .update=update, .render=render, .build_ui=build_ui, .process_input=process_input};
