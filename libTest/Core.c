#include "libTest.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"
#include <stdlib.h>

unitTest *currentTest;
static const int error = 1;
uint32_t passes = 0, failures = 0;

#define ASSERTION_ERROR(params, result, expected) \
	logResult(RESULT_FAILURE, "Assertion failure: expected " params ", got " params, expected, result);

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
