#include "libTest.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"

test *tests;
unitTest *currentTest;
static const int error = 1;
uint32_t passes = 0, failures = 0;

#define ASSERTION_FAILURE(what, result, expected) \
	logResult(RESULT_FAILURE, "Assertion failure: " what, expected, result);

#define ASSERTION_ERROR(params, result, expected) \
	ASSERTION_FAILURE("expected " params ", got " params, result, expected);

void fail(const char *reason)
{
	logResult(RESULT_FAILURE, "Failure: %s", reason);
	pthreadExit(&error);
}

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

void assertEqual(int result, int expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%d", result, expected);
		pthreadExit(&error);
	}
}

void assertNotEqual(int result, int expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR("%d", result, expected);
		pthreadExit(&error);
	}
}

void libDebugExit(int num)
{
	// This will change yet.
	exit(num);
}
