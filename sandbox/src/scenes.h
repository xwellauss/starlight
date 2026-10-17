#pragma once

// Only include this file in main.c

#include <starlight/core/scene/scene_manager.h>

#include "scenes/play.h"
#include "scenes/menu.h"
#include "scenes/model.h"
#include "scenes/block.h"
#include "scenes/physics_sim/gravity.h"
#include "scenes/default.h"
#include "scenes/server_test.h"
#include "scenes/client_test.h"
#include "scenes/raycasting.h"

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
	scene_manager_register_scene(&scene_raycasting);
}
