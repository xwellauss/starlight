#include <starlight/core/renderer/renderer.h>

#include <GLFW/glfw3.h>
#if defined(_PLATFORM_ANDROID)
	#define GLFW_EXPOSE_NATIVE_ANDROID
	#include <GLFW/glfw3native.h>
#endif

#include "gl_platform.h"

void renderer_init()
{
#if defined(_PLATFORM_DESKTOP)
	gladLoadGLES2(glfwGetProcAddress);
#endif

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	renderer_set_depth_test(true);
}

void renderer_set_viewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void renderer_clear_buffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_set_bg_color(vec4s color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

RendererState renderer_save_state()
{
	RendererState state;
    state.depth_test  = glIsEnabled(GL_DEPTH_TEST);
    glGetBooleanv(GL_DEPTH_WRITEMASK, (GLboolean*)&state.depth_write);

    return state;
}

void renderer_restore_state(RendererState state)
{
	renderer_set_depth_test(state.depth_test);
	renderer_set_depth_write(state.depth_write);
}

void renderer_set_depth_test(bool enabled)
{
	enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void renderer_set_depth_write(bool enabled)
{
	glDepthMask(enabled);
}

void renderer_scissor_begin(int x, int y, int width, int height)
{
	glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, height);
}

void renderer_scissor_end()
{
	glDisable(GL_SCISSOR_TEST);
}
