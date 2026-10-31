#version 300 es

layout(location=0) in vec2 a_pos;
layout(location=1) in vec4 a_color;
layout(location=2) in vec2 a_tex_coord;

out vec4 f_color;
out vec2 f_tex_coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 transform;

void main()
{
	gl_Position = projection * view * transform * vec4(a_pos.xy, 0.0, 1.0);

	f_color = a_color;
	f_tex_coord = a_tex_coord;
}
