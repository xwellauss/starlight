#pragma once

// Only include this file in main.c

#include <starlight/core/scene/scene_manager.h>

#include "scenes/scene_play.h"
#include "scenes/scene_menu.h"
#include "scenes/scene_model.h"
#include "scenes/scene_block.h"
#include "scenes/physics_sim/scene_gravity.h"
#include "scenes/scene_default.h"
#include "scenes/scene_server_test.h"
#include "scenes/scene_client_test.h"

void sandbox_scenes()
{
	scene_manager_register_scene(&scene_play);
	scene_manager_register_scene(&scene_default);
	scene_manager_register_scene(&scene_block);
	scene_manager_register_scene(&scene_model);
	scene_manager_register_scene(&scene_gravity);
	scene_manager_register_scene(&scene_server_test);
	scene_manager_register_scene(&scene_client_test);
	scene_manager_register_scene(&scene_menu);
}
