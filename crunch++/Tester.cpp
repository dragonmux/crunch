#include "crunch++.h"
#include "Logger.h"
#include <exception>

using namespace std;

bool loggingTests = 0;
std::vector<cxxTestClass> cxxTests;

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
		(unitClass.*unitTest.testFunc)();
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
		unitClass.fail("Exception caught by crunch++");
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
		test.testThread = thread([&, this]{ retVal = testRunner(*this, test); });
		test.testThread.join();
		if (retVal == 2)
			exit(2);
	}
}
