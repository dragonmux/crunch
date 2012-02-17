#ifndef __CORE_H__
#define __CORE_H__

#ifdef _MSC_VER
#define NORETURN __declspec(noreturn)
#else
#define NORETURN __attribute((noreturn))
#endif

extern void libDebugExit(int num) NORETURN;

typedef struct _unitTest
{
	pthread_t testThread;
} unitTest;

#endif /* __CORE_H__ */
