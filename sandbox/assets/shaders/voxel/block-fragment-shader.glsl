#version 300 es

precision mediump float;

in vec4 f_color;
in vec2 f_tex_coord;

out vec4 FragColor;

uniform sampler2D texture_sampler;

void main()
{
	FragColor = texture(texture_sampler, f_tex_coord);
}
