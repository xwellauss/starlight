#include <starlight/core/scene/scene_manager.h>
#include <starlight/utils/logger.h>

#include <stb_ds.h>

static struct { char* key; Scene* value; }* scenes_map = NULL;
static char** scenes_name_list = NULL;
static Scene* current_scene = NULL;

void scene_manager_register_scene(Scene* scene)
{
	if(!scene || !scene->scene_name) return;

	shput(scenes_map, scene->scene_name, scene);
	arrput(scenes_name_list, scene->scene_name);
	scene->init();
}

void scene_manager_switch_scene(const char* scene_name)
{
	Scene* target = shget(scenes_map, scene_name);

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

const Scene* scene_manager_get_scene(const char* name)
{
	Scene* scene = shget(scenes_map, name);
	if(scene) return scene;
	else return NULL;
}

const char** scene_manager_get_scenes_list()
{
	return (const char**)scenes_name_list;
}

void scene_manager_destroy()
{
	for(int i = 0; i < shlen(scenes_map); i++)
	{
		Scene* target = scenes_map[i].value;
		target->deactivate();
		target->destroy();
	}

	shfree(scenes_map);
	arrfree(scenes_name_list);
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
