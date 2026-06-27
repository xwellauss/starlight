#version 300 es

layout(location=0) in vec3 a_pos;
layout(location=1) in vec4 a_color;
layout(location=2) in vec2 a_tex_coord;
layout(location=3) in vec3 a_normal;

out vec3 f_pos;
out vec4 f_color;
out vec2 f_tex_coord;
out vec3 f_normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;
uniform mat3 transform_normal;

void main()
{
	f_color = a_color;
	f_tex_coord = a_tex_coord;
	f_pos = vec3(transform * vec4(a_pos, 1.0));
	f_normal = normalize(transform_normal * a_normal);
	

	gl_Position = projection * view * transform * vec4(a_pos, 1.0);
}
