#include "crunch++.h"
#include "Core.h"
#include "Logger.h"

using namespace std;

#ifdef _MSC_VER
template class CRUNCH_VIS std::allocator<cxxTest>;
template class CRUNCH_VIS std::vector<cxxTest>;
template class CRUNCH_VIS std::allocator<std::exception_ptr>;
template class CRUNCH_VIS std::vector<std::exception_ptr>;
#endif

bool loggingTests = false;
std::vector<cxxTestClass> cxxTests;

#ifdef _MSC_VER
void newline()
{
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	testPrintf("\n");
}
#endif

int testsuit::testRunner(testsuit &unitClass, cxxUnitTest &test)
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
		unitTest.testFunc();
	}
	catch (threadExit_t &val)
	{
		// Did the test switch logging on?
		if (!loggingTests && logging)
			// Yes, switch it back off again
			stopLogging(logger);
		return val;
	}
	catch (...)
	{
		unitClass.exceptions.emplace_back(current_exception());
		// Did the test switch logging on?
		if (!loggingTests && logging)
			// Yes, switch it back off again
			stopLogging(logger);
		logResult(RESULT_FAILURE, "Failure: Exception caught by crunch++");
#ifndef _MSC_VER
		testPrintf(CURS_UP);
#endif
		return 2;
	}
	// Did the test switch logging on?
	if (!loggingTests && logging)
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
		test.testThread = thread([&, this]{ retVal = testRunner(*this, test); });
		test.testThread.join();
		if (retVal == 2)
			echoAborted();
	}
}
