#include "scene_model.h"

#include "../core/game_engine.h"
#include "../core/ecs.h"
#include "../core/font_renderer.h"
#include "../utils/utils.h"
#include "../core/camera.h"
#include "../utils/ui_imgui.h"
#include "../core/model_loader/model_loader.h"

#include <cglm/struct.h>

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
static vec3s model_scale = (vec3s){0.1f, 0.1f, 0.1f};
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
	shader_uniform_vec4(&ecs_get_sprite(model_scene)->shader, "material.base_color", material->base_color);
	shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.roughness_factor", material->roughness_factor);
	shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.metallic_factor", material->metallic_factor);

	if(material->albedo_texture_id > 0)
	{
		texture_active_slot(GL_TEXTURE0);
		texture2d_bind_id(material->albedo_texture_id);
		shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.base_color", 0);
		shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.has_albedo_map", 1);
	}
	else
	{
		shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.has_albedo_map", 0);
	}

	if(material->metallic_roughness_texture_id > 0)
	{
		texture_active_slot(GL_TEXTURE1);
		texture2d_bind_id(material->metallic_roughness_texture_id);
		shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.metallic_roughness", 1);
		shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.has_metallic_roughness_map", 1);
	}
	else
	{
		shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.has_metallic_roughness_map", 0);
	}

	if(material->normal_texture_id > 0)
	{
		texture_active_slot(GL_TEXTURE2);
		texture2d_bind_id(material->normal_texture_id);
		shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.normal_map", 1);
	}
	else
	{
		shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.has_normal_map", 0);
	}

}


static void init()
{
	model_scene = ecs_create_entity("Model");
	light_source = ecs_create_entity("Light Source");

	ecs_add_component(model_scene, COMPONENT_TRANSFORM);
	ecs_add_component(model_scene, COMPONENT_SPRITE);

	ecs_add_component(light_source, COMPONENT_TRANSFORM);
	ecs_add_component(light_source, COMPONENT_SPRITE);


	
	model_load_from_file(&model, "3d-models/lambo.glb", FILE_GLTF);
	model_load_from_file(&light_model, "3d-models/cube.glb", FILE_GLTF);
//	model_load_from_file(&model, "3d-models/cube4.glb");
//	model_parse_data(&model);

	vertex_buffer_init(&ecs_get_sprite(model_scene)->vertex_buffer, model.vertex_data, model.vertex_count*sizeof(Vertex), model.index_data, model.index_count*sizeof(GLuint), true);
	shader_init(&ecs_get_sprite(model_scene)->shader, "shaders/model-vertex-shader.glsl", "shaders/model-fragment-shader.glsl");

	vertex_buffer_init(&ecs_get_sprite(light_source)->vertex_buffer, light_model.vertex_data, light_model.vertex_count*sizeof(Vertex), light_model.index_data, light_model.index_count*sizeof(GLuint), true);
	shader_init(&ecs_get_sprite(light_source)->shader, "shaders/light-source-vertex-shader.glsl", "shaders/light-source-fragment-shader.glsl");




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

	window_change_bgcolor(hex_to_rbg("#222222", 1.0f));
	
	// Light Cube
	mat4s light_source_transform = glms_mat4_identity();
	light_source_transform = glms_translate(light_source_transform, light_position);
	light_source_transform = glms_scale(light_source_transform, (vec3s){0.2f, 0.2f, 0.2f});

	shader_bind(&ecs_get_sprite(light_source)->shader);
	shader_uniform_vec3(&ecs_get_sprite(light_source)->shader, "light_color", light_color);
	shader_uniform_mat4(&ecs_get_sprite(light_source)->shader, "projection", camera.projection_matrix);
	shader_uniform_mat4(&ecs_get_sprite(light_source)->shader, "view", camera.view_matrix);
	shader_uniform_mat4(&ecs_get_sprite(light_source)->shader, "transform", light_source_transform);

	//bind_vertex_buffer(&ecs_get_sprite(light_source)->vertex_buffer, VAO);
	vertex_buffer_bind(&ecs_get_sprite(light_source)->vertex_buffer, BUFFER_VAO);
	glDrawElements(GL_TRIANGLES, light_model.index_count, GL_UNSIGNED_INT, 0);

	// Model

	mat4s model_transform = glms_mat4_identity();
	model_transform = glms_scale(model_transform, model_scale);
	model_transform = glms_rotate(model_transform, glm_rad(model_rotation.x), (vec3s){1.0f, 0.0f, 0.0f});
	model_transform = glms_rotate(model_transform, glm_rad(model_rotation.y), (vec3s){0.0f, 1.0f, 0.0f});
	model_transform = glms_rotate(model_transform, glm_rad(model_rotation.z), (vec3s){0.0f, 0.0f, 1.0f});
	model_transform = glms_translate(model_transform, model_position);


	shader_bind(&ecs_get_sprite(model_scene)->shader);

	shader_uniform_vec3(&ecs_get_sprite(model_scene)->shader, "cam_pos", camera.position);
	shader_uniform_vec3(&ecs_get_sprite(model_scene)->shader, "model_pos", model_position);

	shader_uniform_vec3(&ecs_get_sprite(model_scene)->shader, "light.position", light_position);
	shader_uniform_vec3(&ecs_get_sprite(model_scene)->shader, "light.color", light_color);
	shader_uniform_float(&ecs_get_sprite(model_scene)->shader, "light.intensity", light_intensity);

	shader_uniform_mat4(&ecs_get_sprite(model_scene)->shader, "projection", camera.projection_matrix);
	shader_uniform_mat4(&ecs_get_sprite(model_scene)->shader, "view", camera.view_matrix);
	
	vertex_buffer_bind(&ecs_get_sprite(model_scene)->vertex_buffer, BUFFER_VAO);


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
			shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.has_albedo_map", 0);
			shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.has_metallic_roughness_map", 0);
			shader_uniform_int(&ecs_get_sprite(model_scene)->shader, "material.has_normal_map", 0);
		}

		shader_uniform_mat4(&ecs_get_sprite(model_scene)->shader, "transform", model_transform);
		shader_uniform_mat3(&ecs_get_sprite(model_scene)->shader, "transform_normal", glms_mat3_transpose(glms_mat3_inv(glms_mat4_pick3(model_transform))));
		
		glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, (void*)(mesh->index_offset * sizeof(GLuint)));
	}


	// Imgui
	ImGui_Begin("Scene Controls", NULL, ImGuiWindowFlags_None);
	{
//		ImGui_SetWindowSize((ImVec2){150.0f, 500.0f}, ImGuiCond_Once);
//		ImGui_SetWindowPos((ImVec2){0.0f, 0.0f}, ImGuiCond_Once);
		if(ImGui_Button("Back to Menu"))
		{
			scene_switch("SceneMenu");
		}

		ImGui_DragFloat3("Model Position", model_position.raw);
		ImGui_DragFloat3("Model Scale", model_scale.raw);
		ImGui_DragFloat3("Model Rotation", model_rotation.raw);

		ImGui_DragFloat("Light Intensity", &light_intensity);
		ImGui_DragFloat3("Light Position", light_position.raw);
		ImGui_DragFloat3Ex("Light Color", light_color.raw, 0.05f, -1.0f, 1.0f, "%.3f", 0);
	}
	ImGui_End();
	
	font_renderer_render_text("Starlight", 0.0f, 0.0f, 1.0f, "#ffffff", 1.0f);
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
	vertex_buffer_unbind_all();
	shader_unbind();
	
	texture_active_slot(GL_TEXTURE1);
	texture2d_unbind();

	texture_active_slot(GL_TEXTURE0);
	texture2d_unbind();
}

static void destroy()
{
	model_free(&model);
	model_free(&light_model);
	
	shader_destroy(&ecs_get_sprite(model_scene)->shader);
	vertex_buffer_destroy(&ecs_get_sprite(model_scene)->vertex_buffer);

	shader_destroy(&ecs_get_sprite(light_source)->shader);
	vertex_buffer_destroy(&ecs_get_sprite(light_source)->vertex_buffer);

	ecs_destroy_entity(model_scene);
	ecs_destroy_entity(light_source);
}


Scene scene_model = {"SceneModel", init, destroy, activate, deactivate, update, render, process_input};
