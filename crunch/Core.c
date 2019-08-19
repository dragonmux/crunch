/*
 * This file is part of crunch
 * Copyright © 2013-2019 Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * crunch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * crunch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "crunch.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"
#include <string.h>

test *tests;
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
	thrd_exit(error);
}

void assertTrue(uint8_t value)
{
	if (value == FALSE)
	{
		ASSERTION_ERROR("%s", boolToString(value), "true");
		thrd_exit(error);
	}
}

void assertFalse(uint8_t value)
{
	if (value != FALSE)
	{
		ASSERTION_ERROR("%s", boolToString(value), "false");
		thrd_exit(error);
	}
}

void assertIntEqual(int32_t result, int32_t expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%d", result, expected);
		thrd_exit(error);
	}
}

void assertIntNotEqual(int32_t result, int32_t expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR("%d", result, expected);
		thrd_exit(error);
	}
}

void assertInt64Equal(int64_t result, int64_t expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%lld", result, expected);
		thrd_exit(error);
	}
}

void assertInt64NotEqual(int64_t result, int64_t expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR("%lld", result, expected);
		thrd_exit(error);
	}
}

void assertPtrEqual(void *result, void* expected)
{
	if (result != expected)
	{
		ASSERTION_ERROR("%p", result, expected);
		thrd_exit(error);
	}
}

void assertPtrNotEqual(void *result, void *expected)
{
	if (result == expected)
	{
		ASSERTION_ERROR("%p", result, expected);
		thrd_exit(error);
	}
}

#define DELTA(result, expected) (result >= (expected - DOUBLE_DELTA) && result <= (expected + DOUBLE_DELTA))

void assertDoubleEqual(double result, double expected)
{
	if (!DELTA(result, expected))
	{
		ASSERTION_ERROR("%f", result, expected);
		thrd_exit(error);
	}
}

void assertDoubleNotEqual(double result, double expected)
{
	if (DELTA(result, expected))
	{
		ASSERTION_ERROR("%f", result, expected);
		thrd_exit(error);
	}
}

void assertStringEqual(const char *result, const char *expected)
{
	if (strcmp(result, expected) != 0)
	{
		ASSERTION_ERROR("%s", result, expected);
		thrd_exit(error);
	}
}

void assertStringNotEqual(const char *result, const char *expected)
{
	if (strcmp(result, expected) == 0)
	{
		ASSERTION_ERROR("%s", result, expected);
		thrd_exit(error);
	}
}

void assertMemEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (memcmp(result, expected, expectedLength) != 0)
	{
		ASSERTION_FAILURE("buffers %p and %p do not match", result, expected);
		thrd_exit(error);
	}
}

void assertMemNotEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (memcmp(result, expected, expectedLength) == 0)
	{
		ASSERTION_FAILURE("buffers %p and %p match", result, expected);
		thrd_exit(error);
	}
}

void assertNull(void *result)
{
	if (result != NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		thrd_exit(error);
	}
}

void assertNotNull(void *result)
{
	if (result == NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		thrd_exit(error);
	}
}

void assertConstNull(const void *const result)
{
	if (result != NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		thrd_exit(error);
	}
}

void assertConstNotNull(const void *const result)
{
	if (result == NULL)
	{
		ASSERTION_ERROR("%p", result, NULL);
		thrd_exit(error);
	}
}

void assertGreaterThan(long result, long expected)
{
	if (result <= expected)
	{
		ASSERTION_FAILURE("%ld was not greater than %ld", result, expected);
		thrd_exit(error);
	}
}

void assertLessThan(long result, long expected)
{
	if (result >= expected)
	{
		ASSERTION_FAILURE("%ld was not less than %ld", result, expected);
		thrd_exit(error);
	}
}

void libDebugExit(int num)
{
	/* This will change yet. */
	exit(num);
}
