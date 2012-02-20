#include <pthread.h>
#include <stdio.h>
#include "Core.h"
#include "Logger.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Memory.h"
#include <dlfcn.h>
#include <unistd.h>

static const int ok = 0;
const arg args[] =
{
	{"--log", 1, 1},
	{0}
};

parsedArg **parsedArgs = NULL;
parsedArg **namedTests = NULL;
uint32_t numTests = 0;
const char *cwd = NULL;

void *testRunner(void *self)
{
	unitTest *test = (unitTest *)self;
	testPrintf("%s...\n", test->theTest->testName);
	test->theTest->testFunc();
	logResult(RESULT_SUCCESS, "");
	pthreadExit(&ok);
}

void printStats()
{
	uint64_t total = passes + failures;
	testPrintf("Total tests: %zu.  Failures: %u.  Pass rate (%%): ", total, failures);
	if (total == 0)
		testPrintf("--\n");
	else
		testPrintf("%0.2f\n", ((double)passes) / ((double)total) * 100.0);
}

uint8_t getTests()
{
	uint32_t i, j, n;
	for (n = 0; parsedArgs[n] != NULL; n++);
	namedTests = testMalloc(sizeof(parsedArg *) * (n + 1));

	for (j = 0, i = 0; i < n; i++)
	{
		if (findArgInArgs(parsedArgs[i]->value) == NULL)
		{
			namedTests[j] = parsedArgs[i];
			j++;
		}
	}
	if (j == 0)
	{
		free(namedTests);
		return FALSE;
	}
	else
	{
		namedTests = testRealloc(namedTests, sizeof(parsedArg *) * (j + 1));
		numTests = j;
		return TRUE;
	}
}

uint8_t tryRegistration(void *testSuit)
{
	void (__cdecl *registerTests)();
	registerTests = dlsym(testSuit, "registerTests");
	if (registerTests == NULL)
	{
		dlclose(testSuit);
		return FALSE;
	}
	registerTests();
	return TRUE;
}

void runTests()
{
	pthread_attr_t threadAttrs;
	uint32_t i;
	test *currTest;
	log *logFile = NULL;

	parsedArg *logging = findArg(parsedArgs, "--log", NULL);
	if (logging != NULL)
		logFile = startLogging(logging->params[0]);
	pthread_attr_init(&threadAttrs);
	pthread_attr_setdetachstate(&threadAttrs, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&threadAttrs, PTHREAD_SCOPE_PROCESS);

	for (i = 0; i < numTests; i++)
	{
		char *testLib = formatString("%s/%s.so", cwd, namedTests[i]->value);
		void *testSuit = dlopen(testLib, RTLD_LAZY);
		free(testLib);
		if (testSuit == NULL || tryRegistration(testSuit) == FALSE)
		{
			testPrintf("Test library %s was not a valid library, skipping\n", namedTests[i]->value);
			continue;
		}
		currTest = tests;
		while (currTest->testFunc != NULL)
		{
			int *retVal;
			unitTest *test = testMalloc(sizeof(unitTest));
			test->theTest = currTest;
			test->testThread = testMalloc(sizeof(pthread_t));
			pthread_create(test->testThread, &threadAttrs, testRunner, test);
			pthread_join(*test->testThread, &retVal);
			if (retVal == NULL || *retVal == 2)
				exit(2);
			currTest++;
		}
	}

	pthread_attr_destroy(&threadAttrs);
	printStats();
	if (logging != NULL)
		stopLogging(logFile);
}

int main(int argc, char **argv)
{
	parsedArgs = parseArguments(argc, argv);
	if (parsedArgs == NULL || getTests() == FALSE)
	{
		testPrintf("Fatal error: There are no tests to run given on the command line!\n");
		return 2;
	}
	cwd = getcwd(NULL, 0);
	runTests();
	free((void *)cwd);
	return 0;
}
