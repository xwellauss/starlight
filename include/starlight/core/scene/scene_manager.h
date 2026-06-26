#pragma once

#include <starlight/core/scene/scene.h>

void scene_manager_register_scene(Scene* scene);
void scene_manager_switch_scene(char* scene_name);
const Scene* scene_manager_get_current();
void scene_manager_destroy_all_scenes();

void scene_manager_scene_init();
void scene_manager_scene_update();
void scene_manager_scene_render();
void scene_manager_scene_build_ui();
void scene_manager_scene_process_input();
void scene_manager_scene_deactivate();
void scene_manager_scene_destroy();
