#pragma once

// Only include this file in main.c

#include <starlight/core/scene/scene_manager.h>

#include "scenes/scene_play.h"
#include "scenes/scene_model.h"
#include "scenes/scene_block.h"
#include "scenes/physics_sim/scene_gravity.h"
#include "scenes/scene_default.h"

void sandbox_scenes()
{
	scene_manager_register_scene(&scene_play);
	scene_manager_register_scene(&scene_default);
	scene_manager_register_scene(&scene_block);
	scene_manager_register_scene(&scene_model);
	scene_manager_register_scene(&scene_gravity);
}
