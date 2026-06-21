#version 300 es

precision mediump float;

in vec4 f_color;
in vec2 f_tex_coord;

out vec4 FragColor;


void main()
{
    FragColor = f_color;
}
