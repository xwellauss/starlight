#version 300 es

in vec2 a_pos;
in vec4 a_color;
in vec2 a_tex_coord;

out vec4 f_color;
out vec2 f_tex_coord;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(a_pos, 0.0, 1.0);

	f_color = a_color;
	f_tex_coord = a_tex_coord;
}
