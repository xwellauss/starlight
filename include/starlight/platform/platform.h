#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef enum
{
	FILE_READ_TEXT,
	FILE_READ_BINARY
} FileReadMode;

FILE* platform_fopen(const char* path, const char* mode);
char* platform_read_file(const char* filepath, FileReadMode mode, size_t* out_size);
bool platform_write_file(const char* filepath, char* data);

