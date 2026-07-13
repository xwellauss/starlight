#include "scene_gravity.h"

#include <starlight/core/engine.h>
#include <starlight/core/ecs.h>
#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>
#include <starlight/core/camera.h>
#include <starlight/core/resources/model_loader.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

#include <cglm/struct.h>
#include <math.h>
#include <stdint.h>

//#define G 6.674 * pow(10, -11)
#define G 0.07


static InputState input_state;

static Camera camera;

static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static vec2s joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static vec4s joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};

static Model sphere_model;

static Entity planet1_entity;
static Entity planet2_entity;


static void update_entity_transform(Entity* e)
{
	ecs_entity_get_transform(e)->transform = glms_mat4_identity();
	ecs_entity_get_transform(e)->transform = glms_scale(ecs_entity_get_transform(e)->transform, ecs_entity_get_transform(e)->scale);
	ecs_entity_get_transform(e)->transform = glms_rotate(ecs_entity_get_transform(e)->transform, glm_rad(ecs_entity_get_transform(e)->rotation.x), (vec3s){1.0f, 0.0f, 0.0f});
	ecs_entity_get_transform(e)->transform = glms_rotate(ecs_entity_get_transform(e)->transform, glm_rad(ecs_entity_get_transform(e)->rotation.y), (vec3s){0.0f, 1.0f, 0.0f});
	ecs_entity_get_transform(e)->transform = glms_rotate(ecs_entity_get_transform(e)->transform, glm_rad(ecs_entity_get_transform(e)->rotation.z), (vec3s){0.0f, 0.0f, 1.0f});
	ecs_entity_get_transform(e)->transform = glms_translate(ecs_entity_get_transform(e)->transform, ecs_entity_get_transform(e)->position);
}

static void render_planet_model(Entity* e)
{
	shader_bind(&ecs_entity_get_sprite(e)->shader);

	shader_uniform_vec4(&ecs_entity_get_sprite(e)->shader, "light_color", ecs_entity_get_sprite(e)->color);
	shader_uniform_mat4(&ecs_entity_get_sprite(e)->shader, "projection", camera.projection_matrix);
	shader_uniform_mat4(&ecs_entity_get_sprite(e)->shader, "view", camera.view_matrix);
	shader_uniform_mat4(&ecs_entity_get_sprite(e)->shader, "transform", ecs_entity_get_transform(e)->transform);

	vertex_buffer_draw_indexed(&ecs_entity_get_sprite(e)->vertex_buffer, PRIMITIVE_TRIANGLES, sphere_model.index_count, 0);
}

void update_physics()
{
	float deltatime = engine_get_deltatime();
	Entity* e1 = &planet1_entity;
	Entity* e2 = &planet2_entity;

	float mutual_force_mod = (G * ecs_entity_get_physics(e1)->mass * ecs_entity_get_physics(e2)->mass)/(glms_vec3_distance2(ecs_entity_get_transform(e1)->position, ecs_entity_get_transform(e2)->position));
	vec3s relative_position = glms_vec3_normalize(glms_vec3_sub(ecs_entity_get_transform(e1)->position, ecs_entity_get_transform(e2)->position)); // r1 - r2

	ecs_entity_get_physics(e1)->acceleration = glms_vec3_scale(relative_position, -mutual_force_mod/ecs_entity_get_physics(e1)->mass);
	ecs_entity_get_physics(e2)->acceleration = glms_vec3_scale(relative_position, mutual_force_mod/ecs_entity_get_physics(e2)->mass);


	ecs_entity_get_physics(e1)->velocity = glms_vec3_add(ecs_entity_get_physics(e1)->velocity, glms_vec3_scale(ecs_entity_get_physics(e1)->acceleration, deltatime));
	ecs_entity_get_physics(e2)->velocity = glms_vec3_add(ecs_entity_get_physics(e2)->velocity, glms_vec3_scale(ecs_entity_get_physics(e2)->acceleration, deltatime));

	ecs_entity_get_transform(e1)->position = glms_vec3_add(ecs_entity_get_transform(e1)->position, glms_vec3_scale(ecs_entity_get_physics(e1)->velocity, deltatime));
	ecs_entity_get_transform(e2)->position = glms_vec3_add(ecs_entity_get_transform(e2)->position, glms_vec3_scale(ecs_entity_get_physics(e2)->velocity, deltatime));
}

void update_transform()
{
	update_entity_transform(&planet1_entity);
	update_entity_transform(&planet2_entity);
}


static void init()
{
	model_load_from_file(&sphere_model, "3d-models/sphere.glb", FILE_GLTF);

	ecs_entity_init(&planet1_entity, "Planet1");
	ecs_entity_add_component(&planet1_entity, COMPONENT_TRANSFORM);
	ecs_entity_add_component(&planet1_entity, COMPONENT_PHYSICS);
	ecs_entity_add_component(&planet1_entity, COMPONENT_SPRITE);

	ecs_entity_get_transform(&planet1_entity)->position = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_entity_get_transform(&planet1_entity)->rotation = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_entity_get_transform(&planet1_entity)->scale = (vec3s){1.0f, 1.0f, 1.0f};
	ecs_entity_get_sprite(&planet1_entity)->color = hex_to_rgba("#32a852", 1.0f);
	ecs_entity_get_physics(&planet1_entity)->mass = 10.0f;

	vertex_buffer_3d_init(&ecs_entity_get_sprite(&planet1_entity)->vertex_buffer, sphere_model.vertex_data, sphere_model.vertex_count*sizeof(Vertex3D), sphere_model.index_data, sphere_model.index_count*sizeof(uint32_t), true);
	shader_init_from_file(&ecs_entity_get_sprite(&planet1_entity)->shader, "shaders/physics_sim/planet-vertex-shader.glsl", "shaders/physics_sim/planet-fragment-shader.glsl");

	ecs_entity_init(&planet2_entity, "Planet2");
	ecs_entity_add_component(&planet2_entity, COMPONENT_TRANSFORM);
	ecs_entity_add_component(&planet2_entity, COMPONENT_PHYSICS);
	ecs_entity_add_component(&planet2_entity, COMPONENT_SPRITE);


	ecs_entity_get_transform(&planet2_entity)->position = (vec3s){10.0f, 0.0f, 0.0f};
	ecs_entity_get_transform(&planet2_entity)->rotation = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_entity_get_transform(&planet2_entity)->scale = (vec3s){1.0f, 1.0f, 1.0f};
	ecs_entity_get_sprite(&planet2_entity)->color = hex_to_rgba("#c79910", 1.0f);
	ecs_entity_get_physics(&planet2_entity)->mass = 10.0f;

	vertex_buffer_3d_init(&ecs_entity_get_sprite(&planet2_entity)->vertex_buffer, sphere_model.vertex_data, sphere_model.vertex_count*sizeof(Vertex3D), sphere_model.index_data, sphere_model.index_count*sizeof(uint32_t), true);
	shader_init_from_file(&ecs_entity_get_sprite(&planet2_entity)->shader, "shaders/physics_sim/planet-vertex-shader.glsl", "shaders/physics_sim/planet-fragment-shader.glsl");



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
	camera.camera_type =  CAMERA_WALK_AROUND | CAMERA_LOOK_AROUND;
	init_camera(&camera);
}

static void build_ui()
{
}

static void render()
{
#if defined(_PLATFORM_ANDROID) || defined(_PLATFORM_WEB)
	//ui_component_joystick("Input", "Joystick", joystick_box_size, joystick_radius, joystick_color, &joystick_angle, &is_joystick_active);
#endif

	renderer_set_bg_color(hex_to_rgba("#111111", 1.0f));

	render_planet_model(&planet1_entity);
	render_planet_model(&planet2_entity);
}

static void update()
{
	update_camera(&camera);
	move_camera(&camera, input_state);
	update_physics();
	update_transform();
}

static void process_input()
{
	input_state.forward = window_input_key_is_down(INPUT_KEY_UP) || window_input_key_is_down(INPUT_KEY_W);
	input_state.backward = window_input_key_is_down(INPUT_KEY_DOWN) || window_input_key_is_down(INPUT_KEY_S);
	input_state.left = window_input_key_is_down(INPUT_KEY_LEFT) || window_input_key_is_down(INPUT_KEY_A);
	input_state.right = window_input_key_is_down(INPUT_KEY_RIGHT) || window_input_key_is_down(INPUT_KEY_D);
	input_state.up = window_input_key_is_down(INPUT_KEY_SPACE);
	input_state.down = window_input_key_is_down(INPUT_KEY_LEFT_CONTROL);

	// Joystick
	if(is_joystick_active)
	{
		if(joystick_angle >= 315.0f || joystick_angle <= 45.0f) input_state.right = true;
		else if(joystick_angle >= 45.0f && joystick_angle <= 135.0f) input_state.forward = true;
		else if(joystick_angle >= 135.0f && joystick_angle <= 225.0f) input_state.left = true;
		else if(joystick_angle >= 225.0f && joystick_angle <= 315.0f) input_state.backward = true;
	}
}

static void activate()
{
}

static void deactivate()
{
	vertex_buffer_unbind_all();
	shader_unbind();

	texture_active_slot(TEXTURE_SLOT_1);
	texture2d_unbind();

	texture_active_slot(TEXTURE_SLOT_0);
	texture2d_unbind();
}

static void destroy()
{
	model_free(&sphere_model);

	shader_destroy(&ecs_entity_get_sprite(&planet1_entity)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&planet1_entity)->vertex_buffer);

	shader_destroy(&ecs_entity_get_sprite(&planet2_entity)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&planet2_entity)->vertex_buffer);

	ecs_entity_destroy(&planet1_entity);
	ecs_entity_destroy(&planet2_entity);
}

Scene scene_gravity = {"SceneGravity", init, destroy, activate, deactivate, update, render, build_ui, process_input};
