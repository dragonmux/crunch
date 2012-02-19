#include <pthread.h>
#include <stdio.h>
#include "Core.h"
#include "Logger.h"
#include "ArgsParser.h"

static const int ok;
const arg args[] =
{
	{"--log", 1, 1},
	{0}
};

parsedArg **parsedArgs = NULL;

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

void runTests()
{
	log *logFile;
	parsedArg *logging = findArg(parsedArgs, "--log", NULL);
	if (logging != NULL)
		logFile = startLogging(logging->params[0]);
	printStats();
	if (logging != NULL)
		stopLogging(logFile);
}

int main(int argc, char **argv)
{
	parsedArgs = parseArguments(argc, argv);
	runTests();
	return 0;
}
