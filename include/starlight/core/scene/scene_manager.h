#pragma once

#include <starlight/core/scene/scene.h>

#include <stddef.h>

void scene_manager_register_scene(Scene* scene);
void scene_manager_switch_scene(const char* scene_name);
const Scene* scene_manager_get_current();
const Scene* scene_manager_get_scene(const char* name);
const char** scene_manager_get_scenes_list();
void scene_manager_destroy();

void scene_manager_scene_init();
void scene_manager_scene_update();
void scene_manager_scene_render();
void scene_manager_scene_build_ui();
void scene_manager_scene_process_input();
void scene_manager_scene_deactivate();
void scene_manager_scene_destroy();
