#include "scene_gravity.h"

#include "../../core/game_engine.h"
#include "../../core/ecs.h"
#include "../../utils/utils.h"
#include "../../core/camera.h"
#include "../../core/model_loader/model_loader.h"
#include "../../utils/ui_imgui.h"

#include <cglm/struct.h>
#include <math.h>

//#define G 6.674 * pow(10, -11)
#define G 0.07

static Window* current_window;

static InputState input_state;

static Camera camera;

static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static ImVec2 joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static ImVec4 joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};

static ImGuiIO* imgui_io;

static Model sphere_model;

static Entity* planet1_entity;
static Entity* planet2_entity;


static void update_entity_transform(Entity* e)
{
	ecs_get_transform(e)->transform = glms_mat4_identity();
	ecs_get_transform(e)->transform = glms_scale(ecs_get_transform(e)->transform, ecs_get_transform(e)->scale);
	ecs_get_transform(e)->transform = glms_rotate(ecs_get_transform(e)->transform, glm_rad(ecs_get_transform(e)->rotation.x), (vec3s){1.0f, 0.0f, 0.0f});
	ecs_get_transform(e)->transform = glms_rotate(ecs_get_transform(e)->transform, glm_rad(ecs_get_transform(e)->rotation.y), (vec3s){0.0f, 1.0f, 0.0f});
	ecs_get_transform(e)->transform = glms_rotate(ecs_get_transform(e)->transform, glm_rad(ecs_get_transform(e)->rotation.z), (vec3s){0.0f, 0.0f, 1.0f});
	ecs_get_transform(e)->transform = glms_translate(ecs_get_transform(e)->transform, ecs_get_transform(e)->position);
}

static void render_planet_model(Entity* e)
{
	shader_bind(&ecs_get_sprite(e)->shader);

	shader_uniform_vec4(&ecs_get_sprite(e)->shader, "light_color", ecs_get_sprite(e)->color);
	shader_uniform_mat4(&ecs_get_sprite(e)->shader, "projection", camera.projection_matrix);
	shader_uniform_mat4(&ecs_get_sprite(e)->shader, "view", camera.view_matrix);
	shader_uniform_mat4(&ecs_get_sprite(e)->shader, "transform", ecs_get_transform(e)->transform);
	
	vertex_buffer_bind(&ecs_get_sprite(e)->vertex_buffer, BUFFER_VAO);
	vertex_buffer_draw_indexed(&ecs_get_sprite(e)->vertex_buffer, GL_TRIANGLES, GL_UNSIGNED_INT, sphere_model.index_count, 0);
}

void update_physics()
{
	Entity* e1 = planet1_entity;
	Entity* e2 = planet2_entity;

	float mutual_force_mod = (G * ecs_get_physics(e1)->mass * ecs_get_physics(e2)->mass)/(glms_vec3_distance2(ecs_get_transform(e1)->position, ecs_get_transform(e2)->position));
	vec3s relative_position = glms_vec3_normalize(glms_vec3_sub(ecs_get_transform(e1)->position, ecs_get_transform(e2)->position)); // r1 - r2

	ecs_get_physics(e1)->acceleration = glms_vec3_scale(relative_position, -mutual_force_mod/ecs_get_physics(e1)->mass);
	ecs_get_physics(e2)->acceleration = glms_vec3_scale(relative_position, mutual_force_mod/ecs_get_physics(e2)->mass);


	ecs_get_physics(e1)->velocity = glms_vec3_add(ecs_get_physics(e1)->velocity, glms_vec3_scale(ecs_get_physics(e1)->acceleration, game_engine.deltatime));
	ecs_get_physics(e2)->velocity = glms_vec3_add(ecs_get_physics(e2)->velocity, glms_vec3_scale(ecs_get_physics(e2)->acceleration, game_engine.deltatime));

	ecs_get_transform(e1)->position = glms_vec3_add(ecs_get_transform(e1)->position, glms_vec3_scale(ecs_get_physics(e1)->velocity, game_engine.deltatime));
	ecs_get_transform(e2)->position = glms_vec3_add(ecs_get_transform(e2)->position, glms_vec3_scale(ecs_get_physics(e2)->velocity, game_engine.deltatime));
}

void update_transform()
{
	update_entity_transform(planet1_entity);
	update_entity_transform(planet2_entity);
}


static void init()
{
	current_window = &game_engine.current_window;
	
	model_load_from_file(&sphere_model, "3d-models/sphere.glb", FILE_GLTF);

	planet1_entity = ecs_create_entity("Planet1");
	ecs_add_component(planet1_entity, COMPONENT_TRANSFORM);
	ecs_add_component(planet1_entity, COMPONENT_PHYSICS);
	ecs_add_component(planet1_entity, COMPONENT_SPRITE);

	ecs_get_transform(planet1_entity)->position = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_get_transform(planet1_entity)->rotation = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_get_transform(planet1_entity)->scale = (vec3s){1.0f, 1.0f, 1.0f};
	ecs_get_sprite(planet1_entity)->color = hex_to_rbg("#32a852", 1.0f);
	ecs_get_physics(planet1_entity)->mass = 10.0f;

	vertex_buffer_init(&ecs_get_sprite(planet1_entity)->vertex_buffer, sphere_model.vertex_data, sphere_model.vertex_count*sizeof(Vertex), sphere_model.index_data, sphere_model.index_count*sizeof(GLuint), true);
	shader_init(&ecs_get_sprite(planet1_entity)->shader, "shaders/physics_sim/planet-vertex-shader.glsl", "shaders/physics_sim/planet-fragment-shader.glsl");

	planet2_entity = ecs_create_entity("Planet2");
	ecs_add_component(planet2_entity, COMPONENT_TRANSFORM);
	ecs_add_component(planet2_entity, COMPONENT_PHYSICS);
	ecs_add_component(planet2_entity, COMPONENT_SPRITE);


	ecs_get_transform(planet2_entity)->position = (vec3s){10.0f, 0.0f, 0.0f};
	ecs_get_transform(planet2_entity)->rotation = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_get_transform(planet2_entity)->scale = (vec3s){1.0f, 1.0f, 1.0f};
	ecs_get_sprite(planet2_entity)->color = hex_to_rbg("#c79910", 1.0f);
	ecs_get_physics(planet2_entity)->mass = 10.0f;

	vertex_buffer_init(&ecs_get_sprite(planet2_entity)->vertex_buffer, sphere_model.vertex_data, sphere_model.vertex_count*sizeof(Vertex), sphere_model.index_data, sphere_model.index_count*sizeof(GLuint), true);
	shader_init(&ecs_get_sprite(planet2_entity)->shader, "shaders/physics_sim/planet-vertex-shader.glsl", "shaders/physics_sim/planet-fragment-shader.glsl");



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
	
	window_change_bgcolor(hex_to_rbg("#111111", 1.0f));
	
	render_planet_model(planet1_entity);
	render_planet_model(planet2_entity);


	ImGui_Begin("Scene Controls", NULL, ImGuiWindowFlags_None);
	{
//		ImGui_SetWindowSize((ImVec2){150.0f, 500.0f}, ImGuiCond_Once);
//		ImGui_SetWindowPos((ImVec2){0.0f, 0.0f}, ImGuiCond_Once);
		if(ImGui_Button("Back to Menu"))
		{
			scene_switch("SceneMenu");
		}

		ImGui_DragFloat3("Planet 1 Position", ecs_get_transform(planet1_entity)->position.raw);
		ImGui_DragFloat3("Planet 1 Scale", ecs_get_transform(planet1_entity)->scale.raw);
		ImGui_DragFloat3("Planet 1 Rotation", ecs_get_transform(planet1_entity)->rotation.raw);
		ImGui_DragFloat3Ex("Planet 1 Color", ecs_get_sprite(planet1_entity)->color.raw, 0.05f, -1.0f, 1.0f, "%.1f", 0);
		ImGui_DragFloat("Planet 1 Mass", &ecs_get_physics(planet1_entity)->mass);
	
		
		ImGui_DragFloat3("Planet 2 Position", ecs_get_transform(planet2_entity)->position.raw);
		ImGui_DragFloat3("Planet 2 Scale", ecs_get_transform(planet2_entity)->scale.raw);
		ImGui_DragFloat3("Planet 2 Rotation", ecs_get_transform(planet2_entity)->rotation.raw);
		ImGui_DragFloat3Ex("Planet 2 Color", ecs_get_sprite(planet2_entity)->color.raw, 0.05f, -1.0f, 1.0f, "%.1f", 0);
		ImGui_DragFloat("Planet 2 Mass", &ecs_get_physics(planet2_entity)->mass);

	}
	ImGui_End();

}

static void update()
{
	update_camera(&camera);
	update_physics();
	update_transform();
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
	model_free(&sphere_model);

	shader_destroy(&ecs_get_sprite(planet1_entity)->shader);
	vertex_buffer_destroy(&ecs_get_sprite(planet1_entity)->vertex_buffer);

	ecs_destroy_entity(planet1_entity);
	ecs_destroy_entity(planet2_entity);
}

Scene scene_gravity = {"SceneGravity", init, destroy, activate, deactivate, update, render, process_input};
