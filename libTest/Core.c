#include "libTest.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"

unitTest *currentTest;
static const int error = 1;

#define ASSERTION_ERROR(params, result, expected) \
	logResult(RESULT_FAILURE, "Assertion failure: expected " params ", got " params, expected, result);

void assertTrue(uint8_t value)
{
	if (value == FALSE)
	{
		ASSERTION_ERROR("%s", boolToString(value), "true");
		pthread_exit(&error);
	}
}

void assertFalse(uint8_t value)
{
	if (value != FALSE)
	{
		ASSERTION_ERROR("%s", boolToString(value), "false");
		pthread_exit(&error);
	}
}

void libDebugExit(int num)
{
	// This will change yet.
	exit(num);
}
