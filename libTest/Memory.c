#include "Memory.h"
#include <malloc.h>

#define die(...) \
{ \
	printf(__VA_ARGS__); \
	exit(1); \
}

void *testMalloc(size_t size)
{
	void *ret = malloc(size);
	if (ret == NULL)
		die("**** libTest Fatal ****\nCould not allocate enough memory\n**** libTest Fatal ****");
	return ret;
}
