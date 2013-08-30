#include "libTest.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"
#include <string.h>

test *tests;
unitTest *currentTest;
static const int error = 1;
uint32_t passes = 0, failures = 0;

#define DOUBLE_DELTA	0.0000001

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

void assertIntEqual(int result, int expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%d", result, expected);
		pthreadExit(&error);
	}
}

void assertIntNotEqual(int result, int expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR("%d", result, expected);
		pthreadExit(&error);
	}
}

void assertInt64Equal(int64_t result, int64_t expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%lld", result, expected);
		pthreadExit(&error);
	}
}

void assertInt64NotEqual(int64_t result, int64_t expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR("%lld", result, expected);
		pthreadExit(&error);
	}
}

void assertPtrEqual(void *result, void* expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%p", result, expected);
		pthreadExit(&error);
	}
}

void assertPtrNotEqual(void *result, void *expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR("%p", result, expected);
		pthreadExit(&error);
	}
}

#define DELTA(result, expected) (result >= (expected - DOUBLE_DELTA) && result <= (expected + DOUBLE_DELTA))

void assertDoubleEqual(double result, double expected)
{
	if (!DELTA(result, expected))
	{
		ASSERTION_ERROR("%f", result, expected);
		pthreadExit(&error);
	}
}

void assertDoubleNotEqual(double result, double expected)
{
	if (DELTA(result, expected))
	{
		ASSERTION_ERROR("%f", result, expected);
		pthreadExit(&error);
	}
}

void assertStringEqual(const char *result, const char *expected)
{
	if (strcmp(result, expected) != 0)
	{
		ASSERTION_ERROR("%s", result, expected);
		pthreadExit(&error);
	}
}

void assertStringNotEqual(const char *result, const char *expected)
{
	if (strcmp(result, expected) == 0)
	{
		ASSERTION_ERROR("%s", result, expected);
		pthreadExit(&error);
	}
}

void assertMemEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (memcmp(result, expected, expectedLength) != 0)
	{
		ASSERTION_FAILURE("buffers %p and %p do not match", result, expected);
		pthreadExit(&error);
	}
}

void assertMemNotEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (memcmp(result, expected, expectedLength) == 0)
	{
		ASSERTION_FAILURE("buffers %p and %p match", result, expected);
		pthreadExit(&error);
	}
}

void assertNull(void *result)
{
	if (result != NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		pthreadExit(&error);
	}
}

void assertNotNull(void *result)
{
	if (result == NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		pthreadExit(&error);
	}
}

void assertConstNull(const void *result)
{
	if (result != NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		pthreadExit(&error);
	}
}

void assertConstNotNull(const void *result)
{
	if (result == NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		pthreadExit(&error);
	}
}

void assertGreaterThan(long result, long expected)
{
	if (result <= expected)
	{
		ASSERTION_FAILURE("%ld was not greater than %ld", result, expected);
		pthreadExit(&error);
	}
}

void assertLessThan(long result, long expected)
{
	if (result >= expected)
	{
		ASSERTION_FAILURE("%ld was not less than %ld", result, expected);
		pthreadExit(&error);
	}
}

void libDebugExit(int num)
{
	/* This will change yet. */
	exit(num);
}
