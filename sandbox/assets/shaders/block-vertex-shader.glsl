#version 300 es

in vec3 a_pos;
in vec4 a_color;
in vec2 a_tex_coord;
in vec3 a_normal;

out vec4 f_color;
out vec2 f_tex_coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

void main()
{
	//gl_Position = projection * view * transform * vec4(a_pos, 1.0);
	gl_Position = projection * view * vec4(a_pos, 1.0);

	f_color = a_color;
	f_tex_coord = a_tex_coord;
}
