#include "libTest.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"
#include <stdlib.h>

unitTest *currentTest;
static const int error = 1;

#define ASSERTION_ERROR(params, result, expected) \
	logResult(RESULT_FAILURE, "Assertion failure: expected " params ", got " params, expected, result);

#define pthreadExit(val)	pthread_exit((void *)val)

void assertTrue(uint8_t value)
{
	if (value == FALSE)
	{
		ASSERTION_ERROR("%s", boolToString(value), "true");
		pthreadExit(&error);
	}
}

void assertFalse(uint8_t value)
{
	if (value != FALSE)
	{
		ASSERTION_ERROR("%s", boolToString(value), "false");
		pthreadExit(&error);
	}
}

void libDebugExit(int num)
{
	// This will change yet.
	exit(num);
}
