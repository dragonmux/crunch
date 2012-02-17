#ifndef __CORE_H__
#define __CORE_H__

#ifdef _MSC_VER
#define NORETURN __declspec(noreturn)
#else
#define NORETURN __attribute((noreturn))
#endif

#include <pthread.h>

extern void libDebugExit(int num) NORETURN;

extern int passes, failures;

typedef struct _unitTest
{
	pthread_t testThread;
} unitTest;

#endif /* __CORE_H__ */
