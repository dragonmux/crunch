#include "threadShim.h"

#ifndef _WINDOWS
#include <errno.h>
#include <stdint.h>

#ifdef PTHREAD_CREATE_JOINABLE
#undef PTHREAD_CREATE_JOINABLE
#endif
#ifdef PTHREAD_SCOPE_PROCESS
#undef PTHREAD_SCOPE_PROCESS
#endif

typedef void *(*pthread_start_t)(void *);

int thrd_err_map(const int result)
{
	switch (result)
	{
		case 0:
			return thrd_success;
		case ENOMEM:
			return thrd_nomem;
		case ETIMEDOUT:
			return thrd_timedout;
		case EBUSY:
			return thrd_busy;
		default:
			return thrd_error;
	}
}

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	if (!thr)
		return thrd_error;
	pthread_attr_t threadAttrs = {};
	pthread_attr_init(&threadAttrs);
	pthread_attr_setdetachstate(&threadAttrs, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&threadAttrs, PTHREAD_SCOPE_PROCESS);
	const int result = pthread_create(thr, &threadAttrs, (pthread_start_t)func, arg);
	pthread_attr_destroy(&threadAttrs);
	return thrd_err_map(result);
}

int thrd_join(thrd_t thr, int *res)
{
	void *resultPtr = NULL;
	const int result = pthread_join(thr, &resultPtr);
	if (res)
		*res = (int)(uintptr_t)resultPtr;
	return thrd_err_map(result);
}

void thrd_exit(int res) { pthread_exit((void *)(uintptr_t)res); }
#else
#include <stdlib.h>

typedef struct
{
	thrd_start_t func;
	void *arg;
} thrd_params_t;

int thrd_get_error()
{
	const DWORD lastError = GetLastError();
	switch (lastError)
	{
		case ERROR_SUCCESS:
			return thrd_success;
		case ERROR_OUTOFMEMORY:
			return thrd_nomem;
		case WAIT_TIMEOUT:
			return thrd_timedout;
		case ERROR_BUSY:
			return thrd_busy;
		default:
			return thrd_error;
	}
}

DWORD WINAPI thrd_start(void *paramsPtr)
{
	thrd_params_t *params = paramsPtr;
	thrd_start_t func = params->func;
	void *arg = params->arg;
	free(params);
	return (*func)(arg);
}

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	if (!thr)
		return thrd_error;
	thrd_params_t *params = malloc(sizeof(thrd_params_t));
	if (!params)
		return thrd_nomem;
	params->func = func;
	params->arg = arg;
	*thr = CreateThread(NULL, 0, thrd_start, params, CREATE_SUSPENDED, NULL);
	if (*thr)
		ResumeThread(*thr);
	return thrd_get_error();
}

int thrd_join(thrd_t thr, int *res)
{
	DWORD result = WaitForSingleObject(thr, INFINITE);
	if (result)
	{
		if (result == WAIT_TIMEOUT)
			return thrd_timedout;
		return thrd_get_error();
	}
	if (GetExitCodeThread(thr, &result) && res)
		*res = result;
	return thrd_get_error();
}

void thrd_exit(int res) { ExitThread(res); }
#endif
