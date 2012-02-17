#include <pthread.h>
#include <stdio.h>
#include "Core.h"
#include "Logger.h"

const static int ok;

void *testRunner(void *_)
{
	logResult(RESULT_SUCCESS, "\n");
	pthreadExit(&ok);
}

void printStats()
{
	uint64_t total = passes + failures;
	testPrintf("Total tests: %zu.  Failures: %u.  Pass rate (%%): ", total, failures);
	if (total == 0)
		testPrintf("--\n");
	else
		testPrintf("%0.2f\n", ((double)passes) / ((double)total));
}

int main(int argc, char **argv)
{
	log *logFile;
	//parseArguments();
	//runTests();
	logFile = startLogging("test.log");
	printStats();
	stopLogging(logFile);
	return 0;
}
