#include "scene_model.h"

#include <starlight/core/engine.h>
#include <starlight/core/ecs.h>
#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>
#include <starlight/core/renderer/renderer.h>
#include <starlight/core/camera.h>
#include <starlight/core/resources/model_loader.h>
#include <starlight/utils/logger.h>
#include <starlight/utils/math_utils.h>

#include <cglm/struct.h>

#include <stb_ds.h>

static float joystick_angle = 0.0f;
static bool is_joystick_active = false;
static vec2s joystick_box_size = {400.0f, 400.0f};
static float joystick_radius = 70.0f;
static vec4s joystick_color = {225.0f, 225.0f, 225.0f, 100.0f};


static Model model;
static Model light_model;


static Entity model_entity;

static Entity light_source;
static float light_intensity = 50.0f;

static Camera camera;
static InputState input_state;


static void render_mesh_material(Material* material)
{
	SpriteComponent* m_sc = ecs_entity_get_sprite(&model_entity);
	Shader* m_s = &m_sc->shader;

	shader_uniform_vec4(m_s, "material.base_color", material->base_color);
	shader_uniform_float(m_s, "material.roughness_factor", material->roughness_factor);
	shader_uniform_float(m_s, "material.metallic_factor", material->metallic_factor);

	if(material->albedo_texture_id > 0)
	{
		texture_active_slot(TEXTURE_SLOT_0);
		texture2d_bind_id(material->albedo_texture_id);
		shader_uniform_int(m_s, "material.has_albedo_map", 1);
	}
	else
	{
		shader_uniform_int(m_s, "material.has_albedo_map", 0);
	}

	if(material->metallic_roughness_texture_id > 0)
	{
		texture_active_slot(TEXTURE_SLOT_1);
		texture2d_bind_id(material->metallic_roughness_texture_id);
		shader_uniform_int(m_s, "material.has_metallic_roughness_map", 1);
	}
	else
	{
		shader_uniform_int(m_s, "material.has_metallic_roughness_map", 0);
	}

	if(material->normal_texture_id > 0)
	{
		texture_active_slot(TEXTURE_SLOT_2);
		texture2d_bind_id(material->normal_texture_id);
		shader_uniform_int(m_s, "material.has_normal_map", 1);
	}
	else
	{
		shader_uniform_int(m_s, "material.has_normal_map", 0);
	}
}

static void update_entity_transform(Entity* e)
{
	TransformComponent* e_tc = ecs_entity_get_transform(e);
	mat4s* e_t = &e_tc->transform;

	*e_t = glms_mat4_identity();
	*e_t = glms_translate(*e_t, e_tc->position);
	*e_t = glms_rotate(*e_t, glm_rad(e_tc->rotation.x), (vec3s){1.0f, 0.0f, 0.0f});
	*e_t = glms_rotate(*e_t, glm_rad(e_tc->rotation.y), (vec3s){0.0f, 1.0f, 0.0f});
	*e_t = glms_rotate(*e_t, glm_rad(e_tc->rotation.z), (vec3s){0.0f, 0.0f, 1.0f});
	*e_t = glms_scale(*e_t, e_tc->scale);
}

static void render_light_source()
{
	TransformComponent* l_t = ecs_entity_get_transform(&light_source);
	SpriteComponent* l_sc = ecs_entity_get_sprite(&light_source);
	Shader* s_shader = &l_sc->shader;

	shader_bind(s_shader);
	shader_uniform_vec3(s_shader, "light_color", glms_vec3(l_sc->color));
	shader_uniform_mat4(s_shader, "projection", camera.projection_matrix);
	shader_uniform_mat4(s_shader, "view", camera.view_matrix);
	shader_uniform_mat4(s_shader, "transform", l_t->transform);

	//bind_vertex_buffer(&ecs_get_sprite(light_source)->vertex_buffer, VAO);
	vertex_buffer_draw_indexed(&l_sc->vertex_buffer, PRIMITIVE_TRIANGLES, light_model.index_count, 0);
}

static void render_model()
{
	TransformComponent* m_t = ecs_entity_get_transform(&model_entity);
	SpriteComponent* m_sc = ecs_entity_get_sprite(&model_entity);

	Shader* m_s = &m_sc->shader;

	shader_bind(m_s);
	shader_uniform_vec3(m_s, "cam_pos", camera.position);
	shader_uniform_vec3(m_s, "model_pos", m_t->position);

	shader_uniform_vec3(m_s, "light.position", ecs_entity_get_transform(&light_source)->position);
	shader_uniform_vec3(m_s, "light.color", glms_vec3(ecs_entity_get_sprite(&light_source)->color));
	shader_uniform_float(m_s, "light.intensity", light_intensity);

	shader_uniform_mat4(m_s, "projection", camera.projection_matrix);
	shader_uniform_mat4(m_s, "view", camera.view_matrix);


	shader_uniform_int(m_s, "material.base_color_map", 0);
	shader_uniform_int(m_s, "material.metallic_roughness_map", 1);
	shader_uniform_int(m_s, "material.normal_map", 2);

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
			shader_uniform_int(m_s, "material.has_albedo_map", 0);
			shader_uniform_int(m_s, "material.has_metallic_roughness_map", 0);
			shader_uniform_int(m_s, "material.has_normal_map", 0);
		}

		shader_uniform_mat4(m_s, "transform", m_t->transform);
		shader_uniform_mat3(m_s, "transform_normal", glms_mat3_transpose(glms_mat3_inv(glms_mat4_pick3(m_t->transform))));

		vertex_buffer_draw_indexed(&m_sc->vertex_buffer, PRIMITIVE_TRIANGLES, mesh->index_count, (void*)(mesh->index_offset * sizeof(uint32_t)));
	}
}

static void init()
{
	ecs_entity_init(&model_entity, "Model");
	ecs_entity_add_component(&model_entity, COMPONENT_TRANSFORM);
	ecs_entity_add_component(&model_entity, COMPONENT_SPRITE);
	ecs_entity_get_transform(&model_entity)->position = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_entity_get_transform(&model_entity)->scale = (vec3s){0.1f, 0.1f, 0.1f};
	ecs_entity_get_transform(&model_entity)->rotation = (vec3s){0.0f, 0.0f, 0.0f};

	ecs_entity_init(&light_source, "Light Source");
	ecs_entity_add_component(&light_source, COMPONENT_TRANSFORM);
	ecs_entity_add_component(&light_source, COMPONENT_SPRITE);
	ecs_entity_get_transform(&light_source)->position = (vec3s){0.0f, 0.0f, 15.0f};
	ecs_entity_get_transform(&light_source)->scale = (vec3s){0.2f, 0.2f, 0.2f};
	ecs_entity_get_transform(&light_source)->rotation = (vec3s){0.0f, 0.0f, 0.0f};
	ecs_entity_get_sprite(&light_source)->color = (vec4s){1.0f, 1.0f, 1.0f, 1.0f};

	model_load_from_file(&model, "3d-models/lambo.glb", FILE_GLTF);
	model_load_from_file(&light_model, "3d-models/cube.glb", FILE_GLTF);

	vertex_buffer_3d_init(&ecs_entity_get_sprite(&model_entity)->vertex_buffer, model.vertex_data, model.vertex_count*sizeof(Vertex3D), model.index_data, model.index_count*sizeof(uint32_t), true);
	shader_init_from_file(&ecs_entity_get_sprite(&model_entity)->shader, "shaders/model-vertex-shader.glsl", "shaders/model-fragment-shader.glsl");

	vertex_buffer_3d_init(&ecs_entity_get_sprite(&light_source)->vertex_buffer, light_model.vertex_data, light_model.vertex_count*sizeof(Vertex3D), light_model.index_data, light_model.index_count*sizeof(uint32_t), true);
	shader_init_from_file(&ecs_entity_get_sprite(&light_source)->shader, "shaders/light-source-vertex-shader.glsl", "shaders/light-source-fragment-shader.glsl");


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

	renderer_set_bg_color(hex_to_rgba("#222222", 1.0f));
}

static void build_ui()
{
}

static void render()
{
#if defined(_PLATFORM_ANDROID) || defined(_PLATFORM_WEB)
	//ui_component_joystick("Input", "Joystick", joystick_box_size, joystick_radius, joystick_color, &joystick_angle, &is_joystick_active);
#endif
	// Light Cube
	update_entity_transform(&light_source);
	render_light_source();

	// Model
	update_entity_transform(&model_entity);
	render_model();
}

static void update()
{
	update_camera(&camera);
	move_camera(&camera, input_state);
}

static void process_input()
{
	input_state.forward = window_input_key_is_down(INPUT_KEY_UP) || window_input_key_is_down(INPUT_KEY_W);
	input_state.backward = window_input_key_is_down(INPUT_KEY_DOWN) || window_input_key_is_down(INPUT_KEY_S);
	input_state.left = window_input_key_is_down(INPUT_KEY_LEFT) || window_input_key_is_down(INPUT_KEY_A);
	input_state.right = window_input_key_is_down(INPUT_KEY_RIGHT) || window_input_key_is_down(INPUT_KEY_D);
	input_state.up = window_input_key_is_down(INPUT_KEY_SPACE);
	input_state.down = window_input_key_is_down(INPUT_KEY_LEFT_CONTROL);

	vec3s* light_pos = &ecs_entity_get_transform(&light_source)->position;
	float light_speed = 5.0f;
	if(window_input_key_is_down(INPUT_KEY_H))
		*light_pos = glms_vec3_add(*light_pos, (vec3s){-light_speed, 0.0f, 0.0f});
	if(window_input_key_is_down(INPUT_KEY_L))
		*light_pos = glms_vec3_add(*light_pos, (vec3s){light_speed, 0.0f, 0.0f});
	if(window_input_key_is_down(INPUT_KEY_J))
		*light_pos = glms_vec3_add(*light_pos, (vec3s){0.0f, -light_speed, 0.0f});
	if(window_input_key_is_down(INPUT_KEY_K))
		*light_pos = glms_vec3_add(*light_pos, (vec3s){0.0f, light_speed, 0.0f});
	if(window_input_key_is_down(INPUT_KEY_U))
		*light_pos = glms_vec3_add(*light_pos, (vec3s){0.0f, 0.0f, light_speed});
	if(window_input_key_is_down(INPUT_KEY_I))
		*light_pos = glms_vec3_add(*light_pos, (vec3s){0.0f, 0.0f, -light_speed});
	if(window_input_key_is_down(INPUT_KEY_EQUAL))
		light_intensity += 1.0f;
	if(window_input_key_is_down(INPUT_KEY_MINUS))
		light_intensity -= 1.0f;

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
	model_free(&model);
	model_free(&light_model);

	shader_destroy(&ecs_entity_get_sprite(&model_entity)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&model_entity)->vertex_buffer);

	shader_destroy(&ecs_entity_get_sprite(&light_source)->shader);
	vertex_buffer_destroy(&ecs_entity_get_sprite(&light_source)->vertex_buffer);

	ecs_entity_destroy(&model_entity);
	ecs_entity_destroy(&light_source);
}


Scene scene_model = {"SceneModel", init, destroy, activate, deactivate, update, render, build_ui, process_input};
