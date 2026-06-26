#pragma once

#include <starlight/core/renderer/renderer.h>

typedef struct
{
	size_t id;
	bool is_active;
	char* tag;

	size_t component_mask;
} Entity;

typedef struct
{
	vec3s position;
	vec3s scale;
	vec3s rotation;
		
	float speed; // FIXME: update this for other code using it like ScenePlay.

	mat4s transform;
} Transform_Component;

typedef struct
{
	vec3s velocity;
	vec3s acceleration;

	float mass;
} Physics_Component;


// TODO: Split this
typedef struct
{
	VertexBuffer vertex_buffer;
	VertexBufferHashMap vertex_buffer_hashmap;
	Shader shader;
	
	vec4s color; // For testing and debugging

	Texture2DHashMap textures;
} Sprite_Component;


typedef enum
{
	COMPONENT_TRANSFORM = 1 << 0,
	COMPONENT_PHYSICS = 1 << 1,
	COMPONENT_SPRITE = 1 << 2,
} Component_Type;

void ecs_init();
void ecs_destroy();

Entity* ecs_create_entity(char* tag);
void ecs_destroy_entity(Entity* e);

void ecs_add_component(Entity* e, Component_Type type);
void ecs_remove_component(Entity* e, Component_Type type);
bool ecs_has_component(Entity* e, Component_Type type);

Transform_Component* ecs_get_transform(Entity* e);
Physics_Component* ecs_get_physics(Entity* e);
Sprite_Component* ecs_get_sprite(Entity* e);
