#include <starlight/core/camera.h>
#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>

#include <cglm/struct.h>

static vec3s camera_direction;
static float last_x, last_y;

static bool first_mouse = true;
static bool prev_clicking = false;

static float window_width, window_height;

void init_camera(Camera* camera)
{
	window_width = (float)window_get_width();
	window_height = (float)window_get_height();

	last_x = window_width/2.0f;
	last_y = window_height/2.0f;

	camera->projection_matrix = (mat4s)GLMS_MAT4_IDENTITY_INIT;
	camera->view_matrix = (mat4s)GLMS_MAT4_IDENTITY_INIT;	
}

void update_camera(Camera* camera)
{
	camera->aspect_ratio = window_width/window_height;

	camera->projection_matrix = glms_perspective(camera->fov, camera->aspect_ratio, camera->near_plane, camera->far_plane);

	camera->view_matrix = glms_lookat(camera->position, glms_vec3_add(camera->position, camera->front), camera->up);
}

void move_camera(Camera* camera, KeyInputState input_state, float deltatime)
{
	vec2s mouse_position = window_input_mouse_get_position();

	if(camera->camera_type & CAMERA_WALK_AROUND)
	{
		if(input_state.up)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->front, camera->speed * deltatime));
		if(input_state.down)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->front, -camera->speed * deltatime));
		if(input_state.left)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(glms_cross(camera->front, camera->up), -camera->speed * deltatime));
		if(input_state.right)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(glms_cross(camera->front, camera->up), camera->speed * deltatime));
		if(input_state.space)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->up, camera->speed * deltatime));
		if(input_state.l_ctrl)
			camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->up, -camera->speed * deltatime));
	}

	if(camera->camera_type & CAMERA_LOOK_AROUND)
	{
		bool clicking = window_input_mouse_btn_is_down(INPUT_MOUSE_BUTTON_LEFT)
#if defined(_PLATFORM_ANDROID) || defined(_PLATFORM_WEB)
		&& (mouse_position.x >= window_width/2.0f)
#endif
		;

		if(!prev_clicking && clicking) first_mouse = true;
		prev_clicking = clicking;

		if(clicking)
		{
			window_set_cursor_mode(CURSOR_MODE_DISABLED);

			if(first_mouse)
			{
				last_x = mouse_position.x;
				last_y = mouse_position.y;
				first_mouse = false;
			}

			float xoffset = mouse_position.x - last_x;
			float yoffset = last_y - mouse_position.y;

			last_x = mouse_position.x;
			last_y = mouse_position.y;

			xoffset *= camera->mouse_sensitivity;
			yoffset *= camera->mouse_sensitivity;

			camera->yaw += xoffset;
			camera->pitch += yoffset;

			if(camera->pitch > 89.0f) camera->pitch = 89.0f;
			if(camera->pitch < -89.0f) camera->pitch = -89.0f;

			camera_direction.x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
			camera_direction.y = sin(glm_rad(camera->pitch));
			camera_direction.z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));

			camera->front = glms_vec3_normalize(camera_direction);
		}
		else
		{
			first_mouse = true;
			window_set_cursor_mode(CURSOR_MODE_NORMAL);
		}
	}
}
