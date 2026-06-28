#include <starlight/core/window/window.h>
#include <starlight/core/window/input.h>
#include <starlight/core/renderer/renderer.h>
#include <starlight/utils/logger.h>

#include <GLFW/glfw3.h>
#if defined(_PLATFORM_ANDROID)
	#define GLFW_EXPOSE_NATIVE_ANDROID
	#include <GLFW/glfw3native.h>
#endif

#include "internal.h"

#include <string.h>


Window window = {0};

static int cursor_mode_to_glfw(WindowCursorMode mode)
{
	switch(mode)
	{
		case CURSOR_MODE_NORMAL:   return GLFW_CURSOR_NORMAL;
        case CURSOR_MODE_HIDDEN:   return GLFW_CURSOR_HIDDEN;
        case CURSOR_MODE_DISABLED: return GLFW_CURSOR_DISABLED;
        default:                   return GLFW_CURSOR_NORMAL;
	}
}

static void GLFW_error_callback(int errorcode, const char* error_description)
{
	log_error("GLFW Error: [Error Code] %d, [Error Description] %s\n", errorcode, error_description);
}

static void key_callback(GLFWwindow* handle, int key, int scancode, int action, int mods)
{
	InputKey input_key = glfw_key_to_input_key(key);
	if(input_key == INPUT_KEY_UNKNOWN) return;

	if(key < 0 || key >= GLFW_KEY_LAST) return;

	if(action == GLFW_PRESS)
	{
		window.input_system.keys[input_key] = true;
	}
	else if(action == GLFW_RELEASE)
	{
		window.input_system.keys[input_key] = false;
	}
}

static void mouse_callback(GLFWwindow* handle, double xpos, double ypos)
{
	window.input_system.mouse_moved = true;
	window.input_system.mouse_position = (vec2s){xpos, ypos};
}

static void mouse_button_callback(GLFWwindow* handle, int button, int action, int mods)
{
	InputMouseButton input_mouse_btn = glfw_mouse_btn_to_input_mouse_btn(button);

	if(button < 0 || button >= GLFW_MOUSE_BUTTON_LAST) return;

	if(action == GLFW_PRESS)
	{
		window.input_system.mouse_btns[input_mouse_btn] = true;
	}
	else if(action == GLFW_RELEASE)
	{
		window.input_system.mouse_btns[input_mouse_btn] = false;
	}
}

static void scroll_callback(GLFWwindow* handle, double xoffset, double yoffset)
{
	window.input_system.mouse_scroll_delta.x += (float)xoffset;
	window.input_system.mouse_scroll_delta.y += (float)yoffset;
}

static void framebuffer_size_callback(GLFWwindow* handle, int width, int height)
{
	window.config.width = width;
	window.config.height = height;

	renderer_set_viewport(0, 0, window.config.width, window.config.height);
}

void window_init(WindowConfig window_config)
{
	window.config = window_config;

#if defined(_PLATFORM_WEB)
	window.config.width = 1920;
	window.config.height = 1080;

	emscripten_request_fullscreen_strategy("canvas", true, &(EmscriptenFullscreenStrategy)
	{
		.scaleMode=EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT,
		.canvasResolutionScaleMode=EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE,
		.filteringMode=EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST
	});
#endif

	glfwSetErrorCallback(GLFW_error_callback);

	glfwInit();
	
#if defined(_PLATFORM_ANDROID)
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
#endif
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	
	window.handle = glfwCreateWindow(window.config.width, window.config.height, window.config.title, NULL, NULL);
	if(!window.handle)
	{
		glfwTerminate();
		log_error("Failed to Initialize GLFW Window!\n");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window.handle);
	glfwSwapInterval(1);

#if defined(_PLATFORM_ANDROID)
	window.config.width = ANativeWindow_getWidth(glfwGetAndroidApp()->window);
	window.config.height = ANativeWindow_getHeight(glfwGetAndroidApp()->window);
	glfwSetWindowSize(window.handle, window.config.width, window.config.height);
#endif	
	
	renderer_init();
	renderer_set_viewport(0, 0, window.config.width, window.config.height);

	glfwSetKeyCallback(window.handle, key_callback);
	glfwSetCursorPosCallback(window.handle, mouse_callback);
	glfwSetMouseButtonCallback(window.handle, mouse_button_callback);
	glfwSetScrollCallback(window.handle, scroll_callback);
	glfwSetFramebufferSizeCallback(window.handle, framebuffer_size_callback);
}

double window_get_time()
{
	return glfwGetTime();
}

void window_poll_events()
{
	memcpy(window.input_system.keys_prev, window.input_system.keys, sizeof(window.input_system.keys));
	memcpy(window.input_system.mouse_btns_prev, window.input_system.mouse_btns, sizeof(window.input_system.mouse_btns));
	window.input_system.mouse_scroll_delta = (vec2s){0};
	window.input_system.mouse_moved = false;

	glfwPollEvents();
}

void window_swap_buffers()
{
	glfwSwapBuffers(window.handle);
}

int window_should_close()
{
	return glfwWindowShouldClose(window.handle);
}

int window_is_valid()
{
#if defined(_PLATFORM_ANDROID)
	return glfwIsAndroidWindowValid();
#else
	return true;
#endif
}

void window_set_cursor_mode(WindowCursorMode mode)
{
	glfwSetInputMode(window.handle, GLFW_CURSOR, cursor_mode_to_glfw(mode));
}

int window_get_width()
{
	return window.config.width;
}

int window_get_height()
{
	return window.config.height;
}

void window_destroy()
{
	glfwDestroyWindow(window.handle);
	glfwTerminate();
}
