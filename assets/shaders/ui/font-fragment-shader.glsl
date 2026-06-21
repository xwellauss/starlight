#version 300 es

precision mediump float;

in vec4 f_color;
in vec2 f_tex_coord;

out vec4 FragColor;

uniform sampler2D sampler;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler, f_tex_coord).r);
    FragColor = f_color * sampled;
}
