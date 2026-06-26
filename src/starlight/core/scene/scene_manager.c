#include <starlight/core/scene/scene_manager.h>
#include <starlight/utils/logger.h>

#include <stb_ds.h>


static struct { char* key; Scene* value; }* scenes = NULL;
static Scene* current_scene = NULL;

void scene_manager_register_scene(Scene* scene)
{
	if(!scene || !scene->scene_name) return;

	shput(scenes, scene->scene_name, scene);
	scene->init();
}

void scene_manager_switch_scene(char* scene_name)
{
	Scene* target = shget(scenes, scene_name);

	if (!target)
	{
		log_error("Scene %s not found!\n", scene_name);
		return;
	}

	if(current_scene)
	{
		current_scene->deactivate();
	}

	current_scene = target;
	current_scene->activate();
}

const Scene* scene_manager_get_current()
{
	return current_scene;
}

void scene_manager_destroy_all_scenes()
{
	for(int i = 0; i < shlen(scenes); i++)
	{
		Scene* target = scenes[i].value;
		target->deactivate();
		target->destroy();
	}
}

void scene_manager_scene_init()
{
	if(current_scene)
	{
		current_scene->init();
	}
}

void scene_manager_scene_update()
{
	if(current_scene)
	{
		current_scene->update();
	}
}

void scene_manager_scene_render()
{
	if(current_scene)
	{
		current_scene->render();
	}
}

void scene_manager_scene_build_ui()
{
	if(current_scene)
	{
		current_scene->build_ui();
	}
}

void scene_manager_scene_process_input()
{
	if(current_scene)
	{
		current_scene->process_input();
	}
}

void scene_manager_scene_deactivate()
{
	if(current_scene)
	{
		current_scene->deactivate();
	}
}

void scene_manager_scene_destroy()
{
	if(current_scene)
	{
		current_scene->destroy();
	}
}
