#include "ecs.h"
#include "../utils/utils.h"

#include <stb_ds.h>
#include <cglm/struct.h>

#define MAX_ENTITIES 500

static int total_entities = 0;

static Entity** entities = NULL;

static Transform_Component transforms[MAX_ENTITIES];
static Physics_Component physics[MAX_ENTITIES];
static Sprite_Component sprites[MAX_ENTITIES];

void ecs_init()
{
	memset(transforms, 0, sizeof(transforms));
	memset(physics, 0, sizeof(physics));
	memset(sprites, 0, sizeof(sprites));

	total_entities = 0;
}

Entity* ecs_create_entity(char* tag)
{
	Entity* e = malloc(sizeof(Entity));
	e->id = total_entities++;
	e->tag = tag;
	e->is_active = true;
	e->component_mask = 0;

	arrput(entities, e);

	return e;
}

void ecs_add_component(Entity* e, Component_Type type)
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

bool ecs_has_component(Entity* e, Component_Type type)
{
	return e->component_mask & type;
} 

void ecs_remove_component(Entity* e, Component_Type type)
{
	e->component_mask &= ~type;
}

Transform_Component* ecs_get_transform(Entity* e)
{
	return ecs_has_component(e, COMPONENT_TRANSFORM) ? &transforms[e->id] : NULL;
}

Physics_Component* ecs_get_physics(Entity* e)
{
	return ecs_has_component(e, COMPONENT_PHYSICS) ? &physics[e->id] : NULL;
}

Sprite_Component* ecs_get_sprite(Entity* e)
{
	return ecs_has_component(e, COMPONENT_SPRITE) ? &sprites[e->id] : NULL;
}

void ecs_destroy_entity(Entity *e)
{
	for(int i = 0; i < arrlen(entities); i++)
    {
        if(entities[i]->id == e->id)
        {
            arrdel(entities, i);
            break;
        }
    }

	free(e);
}

void ecs_destroy()
{
	arrfree(entities);
}
