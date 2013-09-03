#ifndef __CORE_H__
#define __CORE_H__

#ifdef _MSC_VER
#define NORETURN __declspec(noreturn)
#else
#define NORETURN __attribute((noreturn))
#endif

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include "crunch.h"

extern void libDebugExit(int num) NORETURN;

extern uint32_t passes, failures;

#define pthreadExit(val) \
	pthread_exit((void *)val); \
	exit(*val)

typedef struct _unitTest
{
	pthread_t *testThread;
	test *theTest;
} unitTest;

#endif /* __CORE_H__ */
