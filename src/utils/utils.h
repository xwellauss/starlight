#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include <cglm/struct.h>

#if defined(_PLATFORM_ANDROID)
	#include <android/log.h>
	#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "STARLIGHT", __VA_ARGS__))	
#else
	#define LOG printf
#endif

// Color Codes taken from https://gist.github.com/iamnewton/8754917
#if defined(_PLATFORM_DESKTOP)
  #define LOG_COLOR_RESET       "\e[0m"
  #define LOG_COLOR_RED         "\e[0;31m"
  #define LOG_COLOR_GREEN       "\e[0;32m"
  #define LOG_COLOR_YELLOW      "\e[0;33m"
  #define LOG_COLOR_BLUE        "\e[0;34m"
  #define LOG_COLOR_WHITE       "\e[0;37m"
  #define LOG_COLOR_BOLD_RED    "\e[1;31m"
  #define LOG_COLOR_BOLD_GREEN  "\e[1;32m"
  #define LOG_COLOR_BOLD_YELLOW "\e[1;33m"
  #define LOG_COLOR_BOLD_BLUE   "\e[1;34m"
  #define LOG_COLOR_BOLD_WHITE  "\e[1;37m"
#else
  #define LOG_COLOR_RESET ""
  #define LOG_COLOR_RED ""
  #define LOG_COLOR_GREEN ""
  #define LOG_COLOR_YELLOW ""
  #define LOG_COLOR_BLUE ""
  #define LOG_COLOR_WHITE ""
  #define LOG_COLOR_BOLD_RED ""
  #define LOG_COLOR_BOLD_GREEN ""
  #define LOG_COLOR_BOLD_YELLOW ""
  #define LOG_COLOR_BOLD_BLUE ""
  #define LOG_COLOR_BOLD_WHITE ""
#endif

// Printing Macros
#define log_debug(...) LOG("%s[Debug]: %s", LOG_COLOR_BOLD_WHITE, LOG_COLOR_WHITE); LOG(__VA_ARGS__); LOG(LOG_COLOR_RESET)
#define log_info(...) LOG("%s[Info]: %s", LOG_COLOR_BOLD_BLUE, LOG_COLOR_WHITE); LOG(__VA_ARGS__); LOG(LOG_COLOR_RESET)
#define log_warn(...) LOG("%s[Warning]: %s", LOG_COLOR_BOLD_YELLOW, LOG_COLOR_WHITE); LOG(__VA_ARGS__); LOG(LOG_COLOR_RESET)
#define log_error(...) LOG("%s[Error]: %s", LOG_COLOR_BOLD_RED, LOG_COLOR_WHITE); LOG(__VA_ARGS__); LOG(LOG_COLOR_RESET)

// Functions
char* read_file(const char* filepath, const char* mode);
void write_to_file(const char* filepath, char* data);
vec4s hex_to_rbg(char colorcode[7], float alpha);
