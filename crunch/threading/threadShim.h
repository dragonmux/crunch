#ifndef THREAD_SHIM__H
#define THREAD_SHIM__H

#ifndef USE_C11_THREADING
#include <pthread.h>
#include <crunch.h>

enum
{
	thrd_success = 0,
	thrd_busy = 1,
	thrd_error = 2,
	thrd_nomem = 3,
	thrd_timedout = 4
};

typedef pthread_t thrd_t;
typedef int (*thrd_start_t)(void *);
CRUNCH_API int thrd_create(thrd_t *thr, thrd_start_t func, void *arg);
CRUNCH_API int thrd_join(thrd_t thr, int *res);
void thrd_exit(int res) __attribute__((__noreturn__));
#else
#include <threads.h>
#endif

#endif /*THREAD_SHIM__H*/