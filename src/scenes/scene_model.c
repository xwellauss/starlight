#include "scene_model.h"

#include "../core/game_engine.h"
#include "../core/ecs.h"
#include "../core/font_renderer.h"
#include "../utils/utils.h"
#include "../core/renderer.h"
#include "../core/camera.h"
#include "../utils/ui_imgui.h"
#include "../core/model_loader/model_loader.h"

#include "cimgui.h"

#include <stb_ds.h>

static ImGuiIO* imgui_io = NULL;


static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static ImVec2 joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static ImVec4 joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};


static Model model;
static Model light_model;


static Entity* model_scene;

static vec3s model_position = (vec3s){0.0f, 0.0f, 0.0f};
static vec3s model_scale = (vec3s){10.0f, 10.0f, 10.0f};
static vec3s model_rotation = (vec3s){0.0f, 0.0f, 0.0f};

static vec3s light_position = (vec3s){0.0f, 0.0f, 15.0f};
static vec3s light_color = (vec3s){1.0f, 1.0f, 1.0f};
static float light_intensity = 5.0f;

static Entity* light_source;

//static Vertex* vertex_render_data = NULL;
//static GLushort* index_data = NULL;


static Camera camera;

static InputState input_state;


static void render_mesh_material(Material* material)
{
	uniform_vec4(&model_scene->component_list.sprite_component.shader_program, "material.base_color", material->base_color);
	uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.roughness_factor", material->roughness_factor);
	uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.metallic_factor", material->metallic_factor);

	if(material->albedo_texture_id > 0)
	{
		texture_active_slot(GL_TEXTURE0);
		bind_texture_id(material->albedo_texture_id);
		uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.base_color", 0);
		uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.has_albedo_map", 1);
	}
	else
	{
		uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.has_albedo_map", 0);
	}

	if(material->metallic_roughness_texture_id > 0)
	{
		texture_active_slot(GL_TEXTURE1);
		bind_texture_id(material->metallic_roughness_texture_id);
		uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.metallic_roughness", 1);
		uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.has_metallic_roughness_map", 1);
	}
	else
	{
		uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.has_metallic_roughness_map", 0);
	}

	if(material->normal_texture_id > 0)
	{
		texture_active_slot(GL_TEXTURE2);
		bind_texture_id(material->normal_texture_id);
		uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.normal_map", 1);
	}
	else
	{
		uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.has_normal_map", 0);
	}

}


static void init()
{
	model_scene = add_entity("Model");
	light_source = add_entity("Light Source");
	
	model_load_from_file(&model, "3d-models/material_cube.glb", FILE_GLTF);
	model_load_from_file(&light_model, "3d-models/cube.glb", FILE_GLTF);
//	model_load_from_file(&model, "3d-models/cube4.glb");
//	model_parse_data(&model);

	init_vertex_attributes(&model_scene->component_list.sprite_component.vertex_attribs, model.vertex_data, model.vertex_count*sizeof(Vertex), model.index_data, model.index_count*sizeof(GLuint), true, true);
	init_shader_program(&model_scene->component_list.sprite_component.shader_program, "shaders/model-vertex-shader.glsl", "shaders/model-fragment-shader.glsl");

	init_vertex_attributes(&light_source->component_list.sprite_component.vertex_attribs, light_model.vertex_data, light_model.vertex_count*sizeof(Vertex), light_model.index_data, light_model.index_count*sizeof(GLuint), true, true);
	init_shader_program(&light_source->component_list.sprite_component.shader_program, "shaders/light-source-vertex-shader.glsl", "shaders/light-source-fragment-shader.glsl");




	camera.position = (vec3s){{-10.0f, 0.0f, 20.0f}};
	camera.target = (vec3s){{0.0f, 0.0f, 0.0f}};
	camera.up = (vec3s){{0.0f, 1.0f, 0.0f}};
	camera.front = (vec3s){{0.0f, 0.0f, -1.0f}};
	camera.speed = 5.0f;
	camera.fov = glm_rad(45.0f);
	camera.near_plane = 0.1f;
	camera.far_plane = 1000.0f;
	camera.mouse_sensitivity = 0.1f;
	camera.pitch = 0.0f;
	camera.yaw = -90.0f;
	camera.camera_type =  WALK_AROUND | LOOK_AROUND;
	init_camera(&camera);
	
	imgui_io = ImGui_GetIO();
}

static void render()
{
#if defined(_PLATFORM_ANDROID) || defined(_PLATFORM_WEB)
	ui_component_joystick("Input", "Joystick", joystick_box_size, joystick_radius, joystick_color, &joystick_angle, &is_joystick_active);
#endif

	change_window_color(hex_to_rbg("#222222", 1.0f));
	
	// Light Cube
	mat4s light_source_transform = GLMS_MAT4_IDENTITY_INIT;
	light_source_transform = glms_translate(light_source_transform, light_position);
	light_source_transform = glms_scale(light_source_transform, (vec3s){0.2f, 0.2f, 0.2f});

	bind_shader_program(&light_source->component_list.sprite_component.shader_program);
	uniform_vec3(&light_source->component_list.sprite_component.shader_program, "light_color", light_color);
	uniform_mat4(&light_source->component_list.sprite_component.shader_program, "projection", camera.projection_matrix);
	uniform_mat4(&light_source->component_list.sprite_component.shader_program, "view", camera.view_matrix);
	uniform_mat4(&light_source->component_list.sprite_component.shader_program, "transform", light_source_transform);

	//bind_vertex_buffer(&light_source->component_list.sprite_component.vertex_attribs, VAO);
	bind_vertex_buffer(&light_source->component_list.sprite_component.vertex_attribs, VAO);
	glDrawElements(GL_TRIANGLES, light_model.index_count, GL_UNSIGNED_INT, 0);

	// Model

	mat4s model_transform = GLMS_MAT4_IDENTITY_INIT;
	model_transform = glms_scale(model_transform, model_scale);
	model_transform = glms_rotate(model_transform, glm_rad(model_rotation.x), (vec3s){1.0f, 0.0f, 0.0f});
	model_transform = glms_rotate(model_transform, glm_rad(model_rotation.y), (vec3s){0.0f, 1.0f, 0.0f});
	model_transform = glms_rotate(model_transform, glm_rad(model_rotation.z), (vec3s){0.0f, 0.0f, 1.0f});
	model_transform = glms_translate(model_transform, model_position);


	bind_shader_program(&model_scene->component_list.sprite_component.shader_program);

	uniform_vec3(&model_scene->component_list.sprite_component.shader_program, "cam_pos", camera.position);
	uniform_vec3(&model_scene->component_list.sprite_component.shader_program, "model_pos", model_position);

	uniform_vec3(&model_scene->component_list.sprite_component.shader_program, "light.position", light_position);
	uniform_vec3(&model_scene->component_list.sprite_component.shader_program, "light.color", light_color);
	uniform_float(&model_scene->component_list.sprite_component.shader_program, "light.intensity", light_intensity);

	uniform_mat4(&model_scene->component_list.sprite_component.shader_program, "projection", camera.projection_matrix);
	uniform_mat4(&model_scene->component_list.sprite_component.shader_program, "view", camera.view_matrix);
	
	bind_vertex_buffer(&model_scene->component_list.sprite_component.vertex_attribs, VAO);


	for(size_t i = 0; i < model.mesh_count; i++)
	{
		Mesh* mesh = &model.meshes[i];
		Material* material;

		if(mesh->material_index != -1)
		{
			material = &model.materials[mesh->material_index];
			render_mesh_material(material);
		}
		else
		{
			uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.has_albedo_map", 0);
			uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.has_metallic_roughness_map", 0);
			uniform_int(&model_scene->component_list.sprite_component.shader_program, "material.has_normal_map", 0);
		}

		uniform_mat4(&model_scene->component_list.sprite_component.shader_program, "transform", model_transform);
		uniform_mat3(&model_scene->component_list.sprite_component.shader_program, "transform_normal", glms_mat3_transpose(glms_mat3_inv(glms_mat4_pick3(model_transform))));
		
		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void*)(mesh->index_offset * sizeof(GLuint)));
	}


	// Imgui
	ImGui_Begin("Scene Controls", NULL, ImGuiWindowFlags_None);
	{
//		ImGui_SetWindowSize((ImVec2){150.0f, 500.0f}, ImGuiCond_Once);
//		ImGui_SetWindowPos((ImVec2){0.0f, 0.0f}, ImGuiCond_Once);
		if(ImGui_Button("Back to Menu"))
		{
			change_scene("SceneMenu");
		}

		ImGui_DragFloat3("Model Position", model_position.raw);
		ImGui_DragFloat3("Model Scale", model_scale.raw);
		ImGui_DragFloat3("Model Rotation", model_rotation.raw);

		ImGui_DragFloat("Light Intensity", &light_intensity);
		ImGui_DragFloat3("Light Position", light_position.raw);
		ImGui_DragFloat3Ex("Light Color", light_color.raw, 0.05f, -1.0f, 1.0f, "%.3f", 0);
	}
	ImGui_End();
}

static void update()
{
	update_camera(&camera);
}

static void process_input()
{
	InputSystem input_system = game_engine.current_window.input_system;

	input_state.up = input_system.key_pressed_data[GLFW_KEY_UP] || input_system.key_pressed_data[GLFW_KEY_W];
	input_state.down = input_system.key_pressed_data[GLFW_KEY_DOWN] || input_system.key_pressed_data[GLFW_KEY_S];
	input_state.left = input_system.key_pressed_data[GLFW_KEY_LEFT] || input_system.key_pressed_data[GLFW_KEY_A];
	input_state.right = input_system.key_pressed_data[GLFW_KEY_RIGHT] || input_system.key_pressed_data[GLFW_KEY_D];
	input_state.space = input_system.key_pressed_data[GLFW_KEY_SPACE];
	input_state.l_ctrl = input_system.key_pressed_data[GLFW_KEY_LEFT_CONTROL];

	// Joystick
	if(is_joystick_active)
	{
		if(joystick_angle >= 315.0f || joystick_angle <= 45.0f) input_state.right = true;
		else if(joystick_angle >= 45.0f && joystick_angle <= 135.0f) input_state.up = true;
		else if(joystick_angle >= 135.0f && joystick_angle <= 225.0f) input_state.left = true;
		else if(joystick_angle >= 225.0f && joystick_angle <= 315.0f) input_state.down = true;
	}

	move_camera(&camera, input_state, game_engine.deltatime);

}

static void activate()
{
}

static void deactivate()
{
	unbind_vertex_buffer_all();
	unbind_shader_program();
	
	texture_active_slot(GL_TEXTURE1);
	unbind_texture();

	texture_active_slot(GL_TEXTURE0);
	unbind_texture();
}

static void destroy()
{
	model_free(&model);
	model_free(&light_model);
	
	destroy_shader_program(&model_scene->component_list.sprite_component.shader_program);
	destroy_vertex_attributes(&model_scene->component_list.sprite_component.vertex_attribs, true);

	destroy_shader_program(&light_source->component_list.sprite_component.shader_program);
	destroy_vertex_attributes(&light_source->component_list.sprite_component.vertex_attribs, true);

}


Scene scene_model = {"SceneModel", init, destroy, activate, deactivate, update, render, process_input};
