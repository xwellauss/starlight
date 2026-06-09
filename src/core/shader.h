#pragma once

#include "platform.h"

#include <cglm/struct.h>

typedef struct
{
	unsigned int id;
} Shader;

void shader_init(Shader* shader, const char* vertex_shader_source_path, const char* fragment_shader_source_path);
void shader_destroy(Shader* shader);

void shader_bind(Shader* shader);
void shader_unbind();

void shader_uniform_int(Shader* shader, const char* uniform_name, int data);
void shader_uniform_float(Shader* shader, const char* uniform_name, float data);
void shader_uniform_vec2(Shader* shader, const char* uniform_name, vec2s data);
void shader_uniform_vec3(Shader* shader, const char* uniform_name, vec3s data);
void shader_uniform_vec4(Shader* shader, const char* uniform_name, vec4s data);
void shader_uniform_mat3(Shader* shader, const char* uniform_name, mat3s data);
void shader_uniform_mat4(Shader* shader, const char* uniform_name, mat4s data);

