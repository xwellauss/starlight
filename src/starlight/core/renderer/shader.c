#include <starlight/core/renderer/shader.h>
#include <starlight/utils/logger.h>
#include <starlight/platform/platform.h>

#include "gl_platform.h"

static void shader_check_for_errors(unsigned int id, int status)
{
	int success;
	char info_log[512];

	if(status == GL_COMPILE_STATUS)
	{
		glGetShaderiv(id, status, &success);

		if(!success)
		{
			glGetShaderInfoLog(id, 512, NULL, info_log);
			log_error("Error in Compiling Shader\n%s\n", info_log);
			exit(EXIT_FAILURE);
		}
	}
	else if(status == GL_LINK_STATUS)
	{
		glGetProgramiv(id, status, &success);

		if(!success)
		{
			glGetProgramInfoLog(id, 512, NULL, info_log);
			log_error("Error in Linking Shaders\n%s\n", info_log);
			exit(EXIT_FAILURE);
		}
	}
}

static void shader_compile_source(unsigned int* shader, const int shader_type, const char** shader_source)
{
	*shader = glCreateShader(shader_type);
	glShaderSource(*shader, 1, shader_source, NULL);
	glCompileShader(*shader);

	shader_check_for_errors(*shader, GL_COMPILE_STATUS);
}


void shader_init_from_source(Shader* shader, const char* vertex_shader_source, const char* fragment_shader_source)
{
	unsigned int vertex_shader;
	shader_compile_source(&vertex_shader, GL_VERTEX_SHADER, (const char**)&vertex_shader_source);

	unsigned int fragment_shader;
	shader_compile_source(&fragment_shader, GL_FRAGMENT_SHADER, (const char**)&fragment_shader_source);

	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex_shader);
	glAttachShader(shader->id, fragment_shader);
	glLinkProgram(shader->id);

	shader_check_for_errors(shader->id, GL_LINK_STATUS);
	
	shader_unbind();

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void shader_init_from_file(Shader* shader, const char* vertex_shader_path, const char* fragment_shader_path)
{
	char* vertex_shader_source = platform_read_file(vertex_shader_path, FILE_READ_TEXT, NULL);
	char* fragment_shader_source = platform_read_file(fragment_shader_path, FILE_READ_TEXT, NULL);

	unsigned int vertex_shader;
	shader_compile_source(&vertex_shader, GL_VERTEX_SHADER, (const char**)&vertex_shader_source);

	unsigned int fragment_shader;
	shader_compile_source(&fragment_shader, GL_FRAGMENT_SHADER, (const char**)&fragment_shader_source);

	shader->id = glCreateProgram();
	glAttachShader(shader->id, vertex_shader);
	glAttachShader(shader->id, fragment_shader);
	glLinkProgram(shader->id);

	shader_check_for_errors(shader->id, GL_LINK_STATUS);
	
	shader_unbind();

	free(vertex_shader_source);
	free(fragment_shader_source);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

static int shader_get_uniform_location(Shader* shader, const char* uniform_name)
{
	return glGetUniformLocation(shader->id, uniform_name);
}

void shader_uniform_int(Shader* shader, const char* uniform_name, int data)
{
	glUniform1i(shader_get_uniform_location(shader, uniform_name), data);
}

void shader_uniform_float(Shader* shader, const char* uniform_name, float data)
{
	glUniform1f(shader_get_uniform_location(shader, uniform_name), data);
}

void shader_uniform_vec2(Shader* shader, const char* uniform_name, vec2s data)
{
	glUniform2fv(shader_get_uniform_location(shader, uniform_name), 1, data.raw);
}

void shader_uniform_vec3(Shader* shader, const char* uniform_name, vec3s data)
{
	glUniform3fv(shader_get_uniform_location(shader, uniform_name), 1, data.raw);
}
void shader_uniform_vec4(Shader* shader, const char* uniform_name, vec4s data)
{
	glUniform4fv(shader_get_uniform_location(shader, uniform_name), 1, data.raw);
}

void shader_uniform_mat3(Shader* shader, const char* uniform_name, mat3s data)
{
	glUniformMatrix3fv(shader_get_uniform_location(shader, uniform_name), 1, GL_FALSE, (float*)data.raw);
}

void shader_uniform_mat4(Shader* shader, const char* uniform_name, mat4s data)
{
	glUniformMatrix4fv(shader_get_uniform_location(shader, uniform_name), 1, GL_FALSE, (float*)data.raw);
}



void shader_bind(Shader* shader)
{
	glUseProgram(shader->id);
}

void shader_unbind()
{
	glUseProgram(0);
}

void shader_destroy(Shader* shader)
{
	glDeleteProgram(shader->id);
}
