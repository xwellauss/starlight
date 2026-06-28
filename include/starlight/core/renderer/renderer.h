#pragma once

#include <starlight/core/renderer/vertex_buffer.h>
#include <starlight/core/renderer/shader.h>
#include <starlight/core/renderer/texture.h>

#include <cglm/struct.h>
typedef struct
{
	bool depth_test;
	bool depth_write;
} RendererState;

void renderer_init();
void renderer_set_viewport(int x, int y, int width, int height);
void renderer_clear_buffer();
RendererState renderer_save_state();
void renderer_set_bg_color(vec4s color);
void renderer_restore_state(RendererState state);
void renderer_set_depth_test(bool enabled);
void renderer_set_depth_write(bool enabled);
void renderer_scissor_begin(int x, int y, int width, int height);
void renderer_scissor_end();
