#include <starlight/platform/platform.h>


FILE* platform_fopen(const char* path, const char* mode)
{
	return fopen(path, mode);
}

