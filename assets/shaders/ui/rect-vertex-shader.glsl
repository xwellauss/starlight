#version 300 es

in vec2 a_pos;
in vec4 a_color;
in vec2 a_norm_pos;
in vec2 a_rect_pos;
in vec2 a_rect_size;
in vec4 a_corner_radius;
in vec4 a_border_width;

out vec4 f_color;
out vec2 f_norm_pos;
out vec2 f_rect_pos;
out vec2 f_rect_size;
out vec4 f_corner_radius;
out vec4 f_border_width;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(a_pos, 0.0, 1.0);

	f_color = a_color;
	f_norm_pos = a_norm_pos;
    f_rect_pos = a_rect_pos;
    f_rect_size = a_rect_size;
    f_corner_radius = a_corner_radius;
    f_border_width = a_border_width;
}
