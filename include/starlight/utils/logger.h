#pragma once

#include <stdio.h>

#if defined(_PLATFORM_ANDROID)
	#include <android/log.h>
	#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "STARLIGHT", __VA_ARGS__))	
#else
	#define LOG printf
#endif

// Color Codes taken from https://gist.github.com/iamnewton/8754917
#if defined(_PLATFORM_DESKTOP)
  #define LOG_COLOR_RESET       "\033[0m"
  #define LOG_COLOR_RED         "\033[0;31m"
  #define LOG_COLOR_GREEN       "\033[0;32m"
  #define LOG_COLOR_YELLOW      "\033[0;33m"
  #define LOG_COLOR_BLUE        "\033[0;34m"
  #define LOG_COLOR_WHITE       "\033[0;37m"
  #define LOG_COLOR_GRAY        "\033[0;90m"
  #define LOG_COLOR_CYAN        "\033[0;36m"
  #define LOG_COLOR_BOLD_RED    "\033[1;31m"
  #define LOG_COLOR_BOLD_GREEN  "\033[1;32m"
  #define LOG_COLOR_BOLD_YELLOW "\033[1;33m"
  #define LOG_COLOR_BOLD_BLUE   "\033[1;34m"
  #define LOG_COLOR_BOLD_WHITE  "\033[1;37m"
  #define LOG_COLOR_BOLD_GRAY   "\033[1;90m"
  #define LOG_COLOR_BOLD_CYAN   "\033[1;36m"
#else
  #define LOG_COLOR_RESET       ""
  #define LOG_COLOR_RED         ""
  #define LOG_COLOR_GREEN       ""
  #define LOG_COLOR_YELLOW      ""
  #define LOG_COLOR_BLUE        ""
  #define LOG_COLOR_WHITE       ""
  #define LOG_COLOR_GRAY        ""
  #define LOG_COLOR_CYAN        ""
  #define LOG_COLOR_BOLD_RED    ""
  #define LOG_COLOR_BOLD_GREEN  ""
  #define LOG_COLOR_BOLD_YELLOW ""
  #define LOG_COLOR_BOLD_BLUE   ""
  #define LOG_COLOR_BOLD_WHITE  ""
  #define LOG_COLOR_BOLD_GRAY   ""
  #define LOG_COLOR_BOLD_CYAN   ""
#endif

// Printing Macros
#define log_debug(...) LOG("%s[Debug]: %s", LOG_COLOR_BOLD_BLUE, LOG_COLOR_WHITE); LOG(__VA_ARGS__); LOG(LOG_COLOR_RESET)
#define log_info(...) LOG("%s[Info]: %s", LOG_COLOR_BOLD_GREEN, LOG_COLOR_WHITE); LOG(__VA_ARGS__); LOG(LOG_COLOR_RESET)
#define log_warn(...) LOG("%s[Warning]: %s", LOG_COLOR_BOLD_YELLOW, LOG_COLOR_WHITE); LOG(__VA_ARGS__); LOG(LOG_COLOR_RESET)
#define log_error(...) LOG("%s[Error]: %s", LOG_COLOR_BOLD_RED, LOG_COLOR_WHITE); LOG(__VA_ARGS__); LOG(LOG_COLOR_RESET)
