#include <starlight/platform/platform.h>

#include <netinet/in.h>
const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;


FILE* platform_fopen(const char* path, const char* mode)
{
	return fopen(path, mode);
}

