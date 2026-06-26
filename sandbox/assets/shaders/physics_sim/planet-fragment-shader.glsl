#version 300 es

precision mediump float;

in vec4 f_color;
in vec2 f_tex_coord;

out vec4 FragColor;

uniform vec4 light_color;

void main()
{
//	gl_FragColor = texture2D(texture_sampler, f_tex_coord);
//	gl_FragColor = vec4(light_color * object_color, 1.0);
	FragColor = light_color;
}
