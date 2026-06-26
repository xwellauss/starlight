#pragma once

#include <stdio.h>
#include <stdbool.h>

FILE* platform_fopen(const char* path, const char* mode);
char* platform_read_file(const char* filepath, const char* mode);
bool platform_write_file(const char* filepath, char* data);
