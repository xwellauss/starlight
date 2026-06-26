#include <starlight/platform/platform.h>
#include <starlight/utils/logger.h>

#include <unistd.h>
#include <stdlib.h>

char* platform_read_file(const char* filepath, const char* mode)
{
#if !defined(_PLATFORM_ANDROID)
	if(access(filepath, F_OK) != 0)
	{
		log_error("File %s does not exist!\n", filepath);
		exit(EXIT_FAILURE);
	}
#endif

	char* buffer = NULL;
	long length;

	FILE* file = platform_fopen(filepath, mode);

	if(file)
	{
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = malloc(length + 1);

		if(buffer)
		{
			fread(buffer, 1, length, file);
			buffer[length] = '\0';
		}

		fclose(file);
	}

	return buffer;
}

bool platform_write_file(const char* filepath, char* data)
{
	FILE* file = platform_fopen(filepath, "w+");
	if(file)
	{
		fprintf(file, "%s", data);
		fclose(file);

		return true;
	}

	return false;
}


