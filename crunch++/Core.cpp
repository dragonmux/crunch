#include "crunch++.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"
#include <string.h>

uint32_t passes = 0, failures = 0;
const double doubleDelta = 0.0000001;

template<typename T>
void assertionFailure(const char *what, T result, T expected)
{
	char *mesg = formatString("Assertion failure: %s", what);
	logResult(RESULT_FAILURE, mesg, expected, result);
	delete [] mesg;
}

template<typename T>
void assertionError(const char *params, T result, T expected)
{
	char *what = formatString("expected %s, got %s", params, params);
	assertionFailure(what, result, expected);
	delete [] what;
}

template<typename T>
void assertionError(const char *params, T result, std::nullptr_t)
{
	return assertionError(params, result, T(nullptr));
}

template<typename T>
void assertionError(const char *params, T result)
{
	char *what = formatString("did not expect %s", params);
	assertionFailure(what, result, result);
	delete [] what;
}

testsuit::testsuit() { }
testsuit::~testsuit() { }

void testsuit::fail(const char *reason)
{
	logResult(RESULT_FAILURE, "Failure: %s", reason);
	throw threadExit_t(1);
}

void testsuit::assertTrue(const bool value)
{
	if (!value)
	{
		assertionError("%s", boolToString(value), "true");
		throw threadExit_t(1);
	}
}

void testsuit::assertFalse(const bool value)
{
	if (value)
	{
		assertionError("%s", boolToString(value), "false");
		throw threadExit_t(1);
	}
}

void testsuit::assertEqual(const int32_t result, const int32_t expected)
{
	if (result != expected)
	{
		assertionError("%d", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(const int32_t result, const int32_t expected)
{
	if (result == expected)
	{
		assertionError("%d", result);
		throw threadExit_t(1);
	}
}

void testsuit::assertEqual(const int64_t result, const int64_t expected)
{
	if (result != expected)
	{
		assertionError("%lld", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(const int64_t result, const int64_t expected)
{
	if (result == expected)
	{
		assertionError("%lld", result);
		throw threadExit_t(1);
	}
}

void testsuit::assertEqual(void *result, void *expected)
{
	if (result != expected)
	{
		assertionError("%p", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(void *result, void *expected)
{
	if (result == expected)
	{
		assertionError("%p", result);
		throw threadExit_t(1);
	}
}

inline bool delta(const double result, const double expected)
{
	return (result >= (expected - doubleDelta) && result <= (expected + doubleDelta));
}

void testsuit::assertEqual(const double result, const double expected)
{
	if (!delta(result, expected))
	{
		assertionError("%f", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(const double result, const double expected)
{
	if (delta(result, expected))
	{
		assertionError("%f", result);
		throw threadExit_t(1);
	}
}

void testsuit::assertEqual(const char *const result, const char *const expected)
{
	if (strcmp(result, expected) != 0)
	{
		assertionError("%s", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(const char *const result, const char *const expected)
{
	if (strcmp(result, expected) == 0)
	{
		assertionError("%s", result);
		throw threadExit_t(1);
	}
}

void testsuit::assertEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (memcmp(result, expected, expectedLength) != 0)
	{
		assertionFailure("buffers %p and %p do not match", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (memcmp(result, expected, expectedLength) == 0)
	{
		assertionFailure("buffers %p and %p match", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNull(void *result)
{
	if (result != nullptr)
	{
		assertionError("%p", result, nullptr);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotNull(void *result)
{
	if (result == nullptr)
	{
		assertionError("%p", result);
		throw threadExit_t(1);
	}
}

void testsuit::assertNull(const void *const result)
{
	if (result != nullptr)
	{
		assertionError("%p", result, nullptr);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotNull(const void *const result)
{
	if (result == nullptr)
	{
		assertionError("%p", result);
		throw threadExit_t(1);
	}
}

void testsuit::assertGreaterThan(const long result, const long expected)
{
	if (result <= expected)
	{
		assertionFailure("%ld was not greater than %ld", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertLessThan(const long result, const long expected)
{
	if (result >= expected)
	{
		assertionFailure("%ld was not less than %ld", result, expected);
		throw threadExit_t(1);
	}
}
