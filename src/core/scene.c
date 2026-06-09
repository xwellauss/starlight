#include "scene.h"
#include "game_engine.h"

#include <stb_ds.h>

void scene_register(Scene* scene)
{
	shput(game_engine.scenes, scene->scene_name, scene);
	scene->init();
}

void scene_switch(char* scene_name)
{
	Scene* scene = shget(game_engine.scenes, scene_name);

	if (!scene)
	{
		log_error("Scene %s not found!\n", scene_name);
		return;
	}

	if(game_engine.current_scene)
	{
		game_engine.current_scene->deactivate();
	}

	game_engine.current_scene = scene;
	game_engine.current_scene->activate();
}

void scene_update(Scene* scene)
{
	scene->update();
}

void scene_render(Scene* scene)
{
	scene->render();
}

void scene_process_input(Scene* scene)
{
	scene->process_input();
}

void scene_destroy(Scene* scene)
{
	scene->deactivate();
	scene->destroy();
}

void scene_destroy_all()
{
	for(int i = 0; i < shlen(game_engine.scenes); i++)
	{
		scene_destroy(game_engine.scenes[i].value);
	}
}
