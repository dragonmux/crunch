#ifndef THREAD_SHIM__H
#define THREAD_SHIM__H

#ifdef _MSC_VER
#define NORETURN(def) __declspec(noreturn) def
#else
#define NORETURN(def) def __attribute((noreturn))
#endif

#ifndef USE_C11_THREADING
#ifdef _WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef HANDLE thrd_t;
#else
#include <pthread.h>
typedef pthread_t thrd_t;
#endif
#include <crunch.h>

enum
{
	thrd_success = 0,
	thrd_busy = 1,
	thrd_error = 2,
	thrd_nomem = 3,
	thrd_timedout = 4
};

typedef int (*thrd_start_t)(void *);
CRUNCH_API int thrd_create(thrd_t *thr, thrd_start_t func, void *arg);
CRUNCH_API int thrd_join(thrd_t thr, int *res);
NORETURN(void thrd_exit(int res));

extern int thrd_err_map(const int result);
#else
#include <threads.h>
#endif

#endif /*THREAD_SHIM__H*/
