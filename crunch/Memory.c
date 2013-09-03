#include "Memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define die(...) \
{ \
	printf(__VA_ARGS__); \
	exit(1); \
}

void *testMalloc(size_t size)
{
	void *ret = malloc(size);
	if (ret == NULL)
		die("**** crunch Fatal ****\nCould not allocate enough memory!\n**** crunch Fatal ****");
	memset(ret, 0, size);
	return ret;
}

void *testRealloc(void *ptr, size_t size)
{
	void *ret = realloc(ptr, size);
	if (ret == NULL)
		die("**** crunch Fatal ****\nCould not reallocate memory - out of memory!\n**** crunch Fatal ****");
	return ret;
}
