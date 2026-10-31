#include "default.h"

#include <starlight/core/window/input.h>
#include <starlight/utils/math_utils.h>
#include <starlight/core/renderer/renderer.h>
#include <starlight/core/ecs.h>
#include <starlight/utils/logger.h>

#include <stdbool.h>

static Entity quad;

static Vertex2DQuad quad_vertices[1] =
{
	(Vertex2DQuad)
	{
		{
		{{{0.5f, 0.5f}}, {{1.0f, 0.0f, 0.0f, 1.0f}}, {{1.0f, 1.0f}}},
		{{{0.5f, -0.5f}}, {{0.0f, 1.0f, 0.0f, 1.0f}}, {{1.0f, 0.0f}}},
		{{{-0.5f, -0.5f}}, {{0.0f, 0.0f, 1.0f, 1.0f}}, {{0.0f, 0.0f}}},
		{{{-0.5f, 0.5f}}, {{0.0f, 0.0f, 0.0f, 1.0f}}, {{0.0f, 1.0f}}},
		}
	}
};

static uint32_t quad_indices[6] =
{
	0, 1, 2,
	2, 3, 0
};

static void init()
{
	ecs_entity_init(&quad, "quad");
	ecs_entity_add_component(&quad, COMPONENT_SPRITE);
	vertex_buffer_2d_init(&ecs_entity_get_sprite(&quad)->vertex_buffer, quad_vertices, sizeof(quad_vertices), quad_indices, sizeof(quad_indices), true);
	shader_init_from_file(&ecs_entity_get_sprite(&quad)->shader, "shaders/quad-vertex-shader.glsl", "shaders/quad-fragment-shader.glsl");

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
	shader_destroy(&ecs_entity_get_sprite(&quad)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&quad)->vertex_buffer);

	ecs_entity_destroy(&quad);
}

Scene scene_default = {"SceneDefault", .init=init, .destroy=destroy, .activate=activate, .deactivate=deactivate, .update=update, .render=render, .build_ui=build_ui, .process_input=process_input};
