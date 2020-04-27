// SPDX-License-Identifier: LGPL-3.0-or-later
#include "crunch++.h"
#include "core.hxx"
#include "logger.hxx"

using namespace std;

bool loggingTests = false;
std::vector<cxxTestClass> cxxTests;

void newline()
{
	if (isTTY)
#ifdef _MSC_VER
	{
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		testPrintf("\n");
	}
#else
		testPrintf(NEWLINE);
#endif
	else
		testPrintf(" ");
}

int testsuite::testRunner(testsuite &unitClass, crunch::internal::cxxUnitTest &test)
{
	if (isTTY)
#ifndef _MSC_VER
		testPrintf(INFO);
#else
		SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
	testPrintf("%s...", test.unitTest().name());
	newline();
	try
	{
		const auto &unitTest = test.unitTest();
		unitTest.function()();
	}
	catch (threadExit_t &val)
	{
		// Did the test switch logging on?
		if (!loggingTests && logger)
			// Yes, switch it back off again
			stopLogging(logger);
		return val;
	}
	catch (...)
	{
		unitClass.exceptions.emplace_back(current_exception());
		// Did the test switch logging on?
		if (!loggingTests && logger)
			// Yes, switch it back off again
			stopLogging(logger);
		logResult(RESULT_FAILURE, "Failure: Exception caught by crunch++");
#ifndef _MSC_VER
		testPrintf(CURS_UP);
#endif
		return 2;
	}
	// Did the test switch logging on?
	if (!loggingTests && logger)
		// Yes, switch it back off again
		stopLogging(logger);
	logResult(RESULT_SUCCESS, "");
	return 0;
}

void testsuite::test()
{
	for (auto &unitTest : tests)
	{
		crunch::internal::cxxUnitTest test{unitTest};
		int retVal = 2;
		test.testThread = thread([&, this]{ retVal = testRunner(*this, test); });
		test.testThread.join();
		if (retVal == 2)
			echoAborted();
	}
}

namespace crunch
{
	namespace internal
	{
		cxxTest::cxxTest(std::function<void ()> &&func, const char *const name) noexcept :
			testFunc{std::move(func)}, testName{name} { }

		void registerTestClass(std::unique_ptr<testsuite> &&suite, const char *name)
			{ cxxTests.emplace_back(std::move(suite), name); }
	}
}

bool testsuite::registerTest(std::function<void ()> &&func, const char *const name) try
{
	tests.emplace_back(std::move(func), name);
	return true;
}
catch (std::exception &)
	{ return false; }
