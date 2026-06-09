#pragma once

//#include "scene_play.h"
#include "scene_model.h"
//#include "scene_play_server.h"
//#include "scene_play_client.h"
#include "scene_block.h"
#include "physics_sim/scene_gravity.h"
#include "scene_default.h"
#include "scene_menu.h"

void load_scenes()
{
//	add_scene(&scene_play);
//	add_scene(&scene_play_server);
//	add_scene(&scene_play_client);
	scene_register(&scene_default);
	//add_scene(&scene_block);
	//add_scene(&scene_model);
	//add_scene(&scene_gravity);
	//scene_register(&scene_menu);
}
