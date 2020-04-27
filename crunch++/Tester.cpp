// SPDX-License-Identifier: LGPL-3.0-or-later
#include <future>
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

int32_t testsuite::testRunner(testsuite &unitClass, crunch::internal::cxxTest &unitTest)
{
	if (isTTY)
#ifndef _MSC_VER
		testPrintf(INFO);
#else
		SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
	testPrintf("%s...", unitTest.name());
	newline();
	try
		{ unitTest.function()(); }
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
		std::promise<int32_t> resultPromise{};
		auto result = resultPromise.get_future();
		std::thread testThread{
			[](testsuite &suite, crunch::internal::cxxTest &test, std::promise<int32_t> result)
			{
				try
					{ result.set_value(testRunner(suite, test)); }
				catch (...)
					{ result.set_exception(std::current_exception()); }
			}, std::ref(*this), std::ref(unitTest), std::move(resultPromise)
		};
		testThread.join();
		if (result.get() == 2)
			echoAborted();
	}
}

bool testsuite::registerTest(std::function<void ()> &&func, const char *const name) try
{
	tests.emplace_back(std::move(func), name);
	return true;
}
catch (std::exception &)
	{ return false; }

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
