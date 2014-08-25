#include "crunch++.h"
#include "Core.h"
#include "Logger.h"
#include "StringFuncs.h"

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
void assertionError(const char *params, T result, nullptr_t)
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

void testsuit::assertTrue(bool value)
{
	if (!value)
	{
		assertionError("%s", boolToString(value), "true");
		throw threadExit_t(1);
	}
}

void testsuit::assertFalse(bool value)
{
	if (value)
	{
		assertionError("%s", boolToString(value), "false");
		throw threadExit_t(1);
	}
}

void testsuit::assertEqual(int32_t result, int32_t expected)
{
	if (result != expected)
	{
		assertionError("%d", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(int32_t result, int32_t expected)
{
	if (result == expected)
	{
		assertionError("%d", result);
		throw threadExit_t(1);
	}
}

void testsuit::assertEqual(int64_t result, int64_t expected)
{
	if (result != expected)
	{
		assertionError("%lld", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(int64_t result, int64_t expected)
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

void testsuit::assertEqual(double result, double expected)
{
	if (!delta(result, expected))
	{
		assertionError("%f", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotEqual(double result, double expected)
{
	if (delta(result, expected))
	{
		assertionError("%f", result, expected);
		throw threadExit_t(1);
	}
}

void testsuit::assertEqual(const char *result, const char *expected)
{
	assertStringEqual(result, expected);
}

void testsuit::assertNotEqual(const char *result, const char *expected)
{
	assertStringNotEqual(result, expected);
}

void testsuit::assertEqual(const void *result, const void *expected, const size_t expectedLength)
{
	assertMemEqual(result, expected, expectedLength);
}

void testsuit::assertNotEqual(const void *result, const void *expected, const size_t expectedLength)
{
	assertMemNotEqual(result, expected, expectedLength);
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

void testsuit::assertNull(const void *result)
{
	if (result != nullptr)
	{
		assertionError("%p", result, nullptr);
		throw threadExit_t(1);
	}
}

void testsuit::assertNotNull(const void *result)
{
	if (result == nullptr)
	{
		assertionError("%p", result);
		throw threadExit_t(1);
	}
}

void testsuit::assertGreaterThan(long result, long expected)
{
	::assertGreaterThan(result, expected);
}

void testsuit::assertLessThan(long result, long expected)
{
	::assertLessThan(result, expected);
}
