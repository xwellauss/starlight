#include "raytracer1.h"

#include <starlight/core/window/input.h>
#include <starlight/utils/math_utils.h>
#include <starlight/core/renderer/renderer.h>
#include <starlight/core/ecs.h>
#include <starlight/utils/logger.h>

#include <stdbool.h>

#include <stb_ds.h>

#define WIDTH 1920
#define HEIGHT 720

static Entity quad;

static Vertex2DQuad quad_vertices[1] =
{
	(Vertex2DQuad)
	{
		{
		{{{1.0f, 1.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{1.0f, 1.0f}}},
		{{{1.0f, -1.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{1.0f, 0.0f}}},
		{{{-1.0f, -1.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{0.0f, 0.0f}}},
		{{{-1.0f, 1.0f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{0.0f, 1.0f}}},
		}
	}
};

static uint32_t quad_indices[6] =
{
	0, 1, 2,
	2, 3, 0
};

static unsigned char texture_data[WIDTH*HEIGHT*3];

static unsigned char float_to_byte(float f)
{
	if(f < 0.0f) f = 0.0f;
	if(f > 0.999f) f = 0.999f;

	return (unsigned char)(256.0f * f);
}

static void generate_texture_data()
{
	for(int j = 0; j < HEIGHT; j++)
	{
		log_debug("Remaining: %d\n", HEIGHT-j-1);

		for(int i = 0; i < WIDTH; i++)
		{
			int y = j;

			float r = (float)i/(WIDTH-1);
			float g = (float)j/(HEIGHT-1);
			float b = 0.25f;

			int idx = (WIDTH*y + i) * 3;

			texture_data[idx+0] = float_to_byte(r);
			texture_data[idx+1] = float_to_byte(g);
			texture_data[idx+2] = float_to_byte(b);
		}
	}
}

static void init()
{
	ecs_entity_init(&quad, "quad");
	ecs_entity_add_component(&quad, COMPONENT_SPRITE);

	generate_texture_data();

	texture2d_map_add_from_bytes(&ecs_entity_get_sprite(&quad)->textures, "quad", WIDTH, HEIGHT, TEXTURE_FORMAT_RGB, texture_data);

	vertex_buffer_2d_init(&ecs_entity_get_sprite(&quad)->vertex_buffer, quad_vertices, sizeof(quad_vertices), quad_indices, sizeof(quad_indices), true);
	shader_init_from_file(&ecs_entity_get_sprite(&quad)->shader, "shaders/raytracing/quad-vertex-shader.glsl", "shaders/raytracing/quad-fragment-shader.glsl");
}

static void activate()
{
}

static void update()
{
}

static void build_ui()
{
}

static void render()
{
	shader_bind(&ecs_entity_get_sprite(&quad)->shader);
	
	texture2d_bind(&shget(ecs_entity_get_sprite(&quad)->textures, "quad"));

	vertex_buffer_draw_indexed(&ecs_entity_get_sprite(&quad)->vertex_buffer, PRIMITIVE_TRIANGLES, 6, 0);

}

static void process_input()
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
	texture2d_map_destroy(&ecs_entity_get_sprite(&quad)->textures);
	shader_destroy(&ecs_entity_get_sprite(&quad)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&quad)->vertex_buffer);

	ecs_entity_destroy(&quad);
}

Scene scene_raytracer1 = {"SceneRaytracer1", .init=init, .destroy=destroy, .activate=activate, .deactivate=deactivate, .update=update, .render=render, .build_ui=build_ui, .process_input=process_input};
