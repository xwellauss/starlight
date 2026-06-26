#pragma once

#include <cglm/struct.h>

enum CameraType
{
	CAMERA_NO_MOVE = 1 << 0,
	CAMERA_LOOK_AROUND = 1 << 1,
	CAMERA_WALK_AROUND = 1 << 2,
};

typedef struct
{
	bool left;
	bool right;
	bool up;
	bool down;
	bool space;
	bool l_ctrl;
} KeyInputState;

typedef struct
{
	mat4s projection_matrix;
	mat4s view_matrix;

	vec3s position;
	vec3s target;
	vec3s up;

	vec3s front;

	float speed;
	float mouse_sensitivity;
	float fov;
	float near_plane;
	float far_plane;
	float aspect_ratio;

	// Rotation
	float pitch;
	float yaw;

	enum CameraType camera_type;
} Camera;

void init_camera(Camera* camera);
void update_camera(Camera* camera);
void move_camera(Camera* camera, KeyInputState input_state, float deltatime);
