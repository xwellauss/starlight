#include <starlight/core/ecs.h>

#include <stb_ds.h>
#include <cglm/struct.h>
#include <stdint.h>

#define MAX_ENTITIES 500

static int free_ids[MAX_ENTITIES];
static int free_id_count = 0;
static int total_entities = 0; // Active Entities

static Transform_Component transforms[MAX_ENTITIES];
static Physics_Component physics[MAX_ENTITIES];
static Sprite_Component sprites[MAX_ENTITIES];

void ecs_init()
{
	memset(transforms, 0, sizeof(transforms));
	memset(physics, 0, sizeof(physics));
	memset(sprites, 0, sizeof(sprites));
}

void ecs_entity_init(Entity* e, char* tag)
{
	e->id = free_id_count > 0 ? free_ids[--free_id_count] : total_entities++;
	e->tag = tag;
	e->is_active = true;
	e->component_mask = 0;
}

void ecs_entity_add_component(Entity* e, Component_Type type)
{
	e->component_mask |= type;

	switch(type)
	{
	case COMPONENT_TRANSFORM:
		transforms[e->id] = (Transform_Component){0};
		break;
	case COMPONENT_PHYSICS:
		physics[e->id] = (Physics_Component){0};
		physics[e->id].mass = 1.0f;
		break;
	case COMPONENT_SPRITE:
		sprites[e->id] = (Sprite_Component){0};
		break;
	}
}

bool ecs_entity_has_component(Entity* e, Component_Type type)
{
	return e->component_mask & type;
} 

void ecs_entity_remove_component(Entity* e, Component_Type type)
{
	e->component_mask &= ~type;
}

Transform_Component* ecs_entity_get_transform(Entity* e)
{
	return ecs_entity_has_component(e, COMPONENT_TRANSFORM) ? &transforms[e->id] : NULL;
}

Physics_Component* ecs_entity_get_physics(Entity* e)
{
	return ecs_entity_has_component(e, COMPONENT_PHYSICS) ? &physics[e->id] : NULL;
}

Sprite_Component* ecs_entity_get_sprite(Entity* e)
{
	return ecs_entity_has_component(e, COMPONENT_SPRITE) ? &sprites[e->id] : NULL;
}

void ecs_entity_destroy(Entity* e)
{
	e->is_active = false;

	free_ids[free_id_count++] = e->id;
}

void ecs_destroy()
{
}
