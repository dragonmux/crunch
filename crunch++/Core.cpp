#include "crunch++.h"
#include "Logger.h"
#include <exception>

using namespace std;
bool loggingTests = 0;

testsuit::testsuit() { }

testsuit::~testsuit() { }

void testsuit::fail(const char *reason)
{
	::fail(reason);
}

void testsuit::assertTrue(bool value)
{
	::assertTrue(value);
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

int testsuit::testRunner(cxxUnitTest &test)
{
	if (isTTY != 0)
#ifndef _MSC_VER
		testPrintf(INFO);
#else
		SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
	testPrintf("%s...", test.theTest.testName);
	if (isTTY != 0)
#ifndef _MSC_VER
		testPrintf(NEWLINE);
#else
		newline();
#endif
	else
		testPrintf(" ");
	try
	{
		cxxTest &unitTest = test.theTest;
		(unitTest.unitClass->*unitTest.testFunc)();
	}
	catch (threadExit_t &val)
	{
		// Did the test switch logging on?
		if (loggingTests == 0 && logging == 1)
			// Yes, switch it back off again
			stopLogging(logger);
		return val;
	}
	catch (...)
	{
		exception_ptr e = current_exception();
		// Did the test switch logging on?
		if (loggingTests == 0 && logging == 1)
			// Yes, switch it back off again
			stopLogging(logger);
		test.theTest.unitClass->fail("Exception caught by crunch++");
	}
	// Did the test switch logging on?
	if (loggingTests == 0 && logging == 1)
		// Yes, switch it back off again
		stopLogging(logger);
	logResult(RESULT_SUCCESS, "");
	return 0;
}

void testsuit::test()
{
	for (auto &unitTest : tests)
	{
		cxxUnitTest test;
		int retVal = 2;
		test.theTest = unitTest;
		test.testThread = thread([&]{ retVal = testRunner(test); });
		test.testThread.join();
		if (retVal == 2)
			exit(2);
	}
}
