#include <starlight/platform/platform.h>
#include <starlight/utils/logger.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char* platform_read_file(const char* filepath, FileReadMode mode, size_t* out_size)
{
	FILE* file = platform_fopen(filepath, mode == FILE_READ_TEXT ? "r" : "rb");

	if(!file)
	{
		log_debug("File not found: %s\n", filepath);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = malloc(mode == FILE_READ_TEXT ? size + 1 : size);

	fread(buffer, 1, size, file);
	if(mode == FILE_READ_TEXT) buffer[size] = '\0';

	fclose(file);

	if(out_size) *out_size = size;

	return buffer;
}

bool platform_write_file(const char* filepath, char* data)
{
	FILE* file = platform_fopen(filepath, "w+");
	if(file)
	{
		fwrite(data, 1, strlen(data), file);
		fclose(file);

		return true;
	}

	return false;
}


