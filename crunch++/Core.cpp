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
	free(mesg);
}

template<typename T>
void assertionError(const char *params, T result, T expected)
{
	char *what = formatString("expected %s, got %s", params, params);
	assertionFailure(what, result, expected);
	free(what);
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
	::assertFalse(value);
}

void testsuit::assertEqual(int result, int expected)
{
	assertIntEqual(result, expected);
}

void testsuit::assertEqual(int64_t result, int64_t expected)
{
	assertInt64Equal(result, expected);
}

void testsuit::assertEqual(void *result, void *expected)
{
	assertPtrEqual(result, expected);
}

void testsuit::assertEqual(double result, double expected)
{
	assertDoubleEqual(result, expected);
}

void testsuit::assertEqual(const char *result, const char *expected)
{
	assertStringEqual(result, expected);
}

void testsuit::assertEqual(const void *result, const void *expected, const size_t expectedLength)
{
	assertMemEqual(result, expected, expectedLength);
}

void testsuit::assertNotEqual(int result, int expected)
{
	assertIntNotEqual(result, expected);
}

void testsuit::assertNotEqual(int64_t result, int64_t expected)
{
	assertInt64NotEqual(result, expected);
}

void testsuit::assertNotEqual(void *result, void *expected)
{
	assertPtrNotEqual(result, expected);
}

void testsuit::assertNotEqual(double result, double expected)
{
	assertDoubleNotEqual(result, expected);
}

void testsuit::assertNotEqual(const char *result, const char *expected)
{
	assertStringNotEqual(result, expected);
}

void testsuit::assertNotEqual(const void *result, const void *expected, const size_t expectedLength)
{
	assertMemNotEqual(result, expected, expectedLength);
}

void testsuit::assertNull(void *result)
{
	::assertNull(result);
}

void testsuit::assertNotNull(void *result)
{
	::assertNotNull(result);
}

void testsuit::assertNull(const void *result)
{
	assertConstNull(result);
}

void testsuit::assertNotNull(const void *result)
{
	assertConstNotNull(result);
}

void testsuit::assertGreaterThan(long result, long expected)
{
	::assertGreaterThan(result, expected);
}

void testsuit::assertLessThan(long result, long expected)
{
	::assertLessThan(result, expected);
}
