// SPDX-License-Identifier: LGPL-3.0-or-later
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <fenv.h>
#include <float.h>
#include "crunch.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"

test *tests;
uint32_t passes = 0, failures = 0;
int32_t allocCount = -1;

#define ASSERTION_FAILURE(what, ...) \
	logResult(RESULT_FAILURE, "Assertion failure: " what, __VA_ARGS__);

#define ASSERTION_ERROR(params, result, expected) \
	ASSERTION_FAILURE("expected " params ", got " params, expected, result);

#define ASSERTION_ERROR_NEGATIVE(params, result) \
	ASSERTION_FAILURE("did not expect " params, result);

void fail(const char *reason)
{
	logResult(RESULT_FAILURE, "Failure: %s", reason);
	thrd_exit(THREAD_ERROR);
}

void assertTrue(uint8_t value)
{
	if (value == FALSE)
	{
		ASSERTION_ERROR("%s", boolToString(value), "true");
		thrd_exit(THREAD_ERROR);
	}
}

void assertFalse(uint8_t value)
{
	if (value != FALSE)
	{
		ASSERTION_ERROR("%s", boolToString(value), "false");
		thrd_exit(THREAD_ERROR);
	}
}

void assertIntEqual(int32_t result, int32_t expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%d", result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertIntNotEqual(int32_t result, int32_t expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR_NEGATIVE("%d", result);
		thrd_exit(THREAD_ERROR);
	}
}

void assertInt64Equal(int64_t result, int64_t expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%" PRId64, result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertInt64NotEqual(int64_t result, int64_t expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR_NEGATIVE("%" PRId64, result);
		thrd_exit(THREAD_ERROR);
	}
}

void assertPtrEqual(void *result, void* expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%p", result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertPtrNotEqual(void *result, void *expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR_NEGATIVE("%p", result);
		thrd_exit(THREAD_ERROR);
	}
}

#define DELTA(result, expected) ((result) >= ((expected) - DBL_EPSILON) && (result) <= ((expected) + DBL_EPSILON))

void assertDoubleEqual(double result, double expected)
{
	fenv_t env;
	feholdexcept(&env);
	if (!DELTA(result, expected))
	{
		ASSERTION_ERROR("%g", result, expected);
		fesetenv(&env);
		thrd_exit(THREAD_ERROR);
	}
	fesetenv(&env);
}

void assertDoubleNotEqual(double result, double expected)
{
	fenv_t env;
	feholdexcept(&env);
	if (DELTA(result, expected))
	{
		ASSERTION_ERROR_NEGATIVE("%g", result);
		fesetenv(&env);
		thrd_exit(THREAD_ERROR);
	}
	fesetenv(&env);
}

void assertStringEqual(const char *result, const char *expected)
{
	if (strcmp(result, expected) != 0)
	{
		ASSERTION_ERROR("%s", result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertStringNotEqual(const char *result, const char *expected)
{
	if (strcmp(result, expected) == 0)
	{
		ASSERTION_ERROR_NEGATIVE("%s", result);
		thrd_exit(THREAD_ERROR);
	}
}

void assertMemEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (memcmp(result, expected, expectedLength) != 0)
	{
		ASSERTION_FAILURE("buffers %p and %p do not match", result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertMemNotEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (memcmp(result, expected, expectedLength) == 0)
	{
		ASSERTION_FAILURE("buffers %p and %p match", result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertNull(const void *const result)
{
	if (result != NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		thrd_exit(THREAD_ERROR);
	}
}

void assertNotNull(const void *const result)
{
	if (result == NULL)
	{
		ASSERTION_ERROR_NEGATIVE("%p", result);
		thrd_exit(THREAD_ERROR);
	}
}

void assertGreaterThan(int32_t result, int32_t expected)
{
	if (result <= expected)
	{
		ASSERTION_FAILURE("%d was not greater than %d", result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertGreaterThan64(int64_t result, int64_t expected)
{
	if (result <= expected)
	{
		ASSERTION_FAILURE("%" PRId64 " was not greater than %" PRId64, result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertLessThan(int32_t result, int32_t expected)
{
	if (result >= expected)
	{
		ASSERTION_FAILURE("%d was not less than %d", result, expected);
		thrd_exit(THREAD_ERROR);
	}
}

void assertLessThan64(int64_t result, int64_t expected)
{
	if (result >= expected)
	{
		ASSERTION_FAILURE("%" PRId64 " was not less than %" PRId64, result, expected);
		thrd_exit(THREAD_ERROR);
	}
}
