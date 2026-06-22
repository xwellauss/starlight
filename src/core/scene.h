#pragma once

#include "../utils/utils.h"

typedef struct
{
	char* scene_name;

	void (*init)();
	void (*destroy)();
	void (*activate)();
	void (*deactivate)();
	void (*update)();
	void (*render)();
	void (*build_ui)();
	void (*process_input)();
} Scene;

void scene_register(Scene* scene);
void scene_switch(char* scene_name);

void scene_update(Scene* scene);
void scene_render(Scene* scene);
void scene_build_ui(Scene* scene);
void scene_process_input(Scene* scene);
void scene_destroy(Scene* scene);
void scene_destroy_all();
