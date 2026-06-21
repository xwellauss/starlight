#pragma once

//#include "scene_play.h"
#include "scene_model.h"
//#include "scene_play_server.h"
//#include "scene_play_client.h"
#include "scene_block.h"
#include "physics_sim/scene_gravity.h"
#include "scene_default.h"
#include "scene_menu.h"

void scenes_load_registered()
{
//	add_scene(&scene_play);
//	add_scene(&scene_play_server);
//	add_scene(&scene_play_client);
	scene_register(&scene_default);
	//scene_register(&scene_block);
	//scene_register(&scene_model);
	//scene_register(&scene_gravity);
	//scene_register(&scene_menu);
}
