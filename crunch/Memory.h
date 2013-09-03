#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stddef.h>
#include <malloc.h>

extern void *testMalloc(size_t size);
extern void *testRealloc(void *ptr, size_t size);

#endif /* __MEMORY_H__ */
