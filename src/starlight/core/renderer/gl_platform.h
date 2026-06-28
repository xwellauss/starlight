#pragma once

#if defined(_PLATFORM_DESKTOP)
	#include <glad/gles2.h>
#elif defined(_PLATFORM_WEB)
	#include <emscripten.h>
	#include <emscripten/html5.h> 
	#include <GLES3/gl3.h>
#elif defined(_PLATFORM_ANDROID)
	#include <android/native_window.h>
	#include <GLES3/gl3.h>
#endif

#define GLSL_VERSION "#version 300 es"
