// SPDX-License-Identifier: LGPL-3.0-or-later
#include "threading/threadShim.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "Core.h"
#include "Logger.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#ifndef _MSC_VER
#define __USE_GNU
#include <dlfcn.h>
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <io.h>
#define RTLD_LAZY 0
#define dlopen(fileName, flag) (void *)LoadLibrary(fileName)
#define dlsym(handle, symbol) GetProcAddress((HMODULE)handle, symbol)
#define dlclose(handle) FreeLibrary((HMODULE)handle)

char *dlerror()
{
	const DWORD error = GetLastError();
	char *message;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error, MAKELANGID(LANG_NEUTRAL,
		SUBLANG_DEFAULT), (char *)&message, 0, NULL);
	return message;
}

#include <crtdbg.h>
#endif
#include "version.h"

const arg_t crunchArgs[] =
{
	{"--log", 1, 1, 0},
	{"--version", 0, 0, 0},
	{"-v", 0, 0, 0},
	{"--help", 0, 0, 0},
	{"-h", 0, 0, 0},
	{NULL, 0, 0, 0}
};

#ifdef _MSC_VER
#define LIBEXT "tlib"
#else
#define LIBEXT "so"
#endif

constParsedArgs_t parsedArgs = NULL;
parsedArgs_t namedTests = NULL;
uint32_t numTests = 0;
const char *workingDir = NULL;
uint8_t loggingTests = 0;

typedef void (__cdecl *registerFn)();

void noMemory()
	{ puts("**** crunch Fatal ****\nCould not allocate enough memory!\n**** crunch Fatal ****"); }

#ifndef _WINDOWS
typedef void *(*malloc_t)(size_t);
malloc_t malloc_ = NULL;

void *malloc(size_t size)
{
	if (allocCount >= 0 && !allocCount--)
		return NULL;
	else if (!malloc_)
		malloc_ = (malloc_t)dlsym(RTLD_NEXT, "malloc");
	return malloc_(size);
}
#endif

void newline()
{
	if (isTTY != 0)
	{
#ifdef _MSC_VER
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		testPrintf("\n");
#else
		testPrintf(NEWLINE);
#endif
	}
	else
		testPrintf("\n");
}

int testRunner(void *testPtr)
{
	test *theTest = testPtr;
	if (isTTY != 0)
#ifndef _MSC_VER
		testPrintf(INFO);
#else
		SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
	testPrintf("%s...", theTest->testName);
	if (isTTY != 0)
		newline();
	else
		testPrintf(" ");
	theTest->testFunc();
	allocCount = -1;
	// Did the test switch logging on?
	if (!loggingTests && logger)
		// Yes, switch it back off again
		stopLogging(logger);
	logResult(RESULT_SUCCESS, "");
	return THREAD_SUCCESS;
}

void printStats()
{
	uint64_t total = passes + failures;
	testPrintf("Total tests: %" PRIu64 ",  Failures: %" PRIu32 ",  Pass rate: ", total, failures);
	if (total == 0)
		testPrintf("--\n");
	else
		testPrintf("%0.2f%%\n", ((double)passes) / ((double)total) * 100.0);
}

uint8_t getTests()
{
	uint32_t i, j, n;
	for (n = 0; parsedArgs[n] != NULL; n++);
	namedTests = malloc(sizeof(constParsedArg_t) * (n + 1));
	if (!namedTests)
	{
		noMemory();
		return FALSE;
	}

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
		free((void *)namedTests);
		return FALSE;
	}
	else
	{
		namedTests = realloc((void *)namedTests, sizeof(parsedArg_t *) * (j + 1));
		if (!namedTests)
		{
			noMemory();
			return FALSE;
		}
		numTests = j;
		return TRUE;
	}
}

uint8_t tryRegistration(void *testSuite)
{
	registerFn registerTests;
	registerTests = (registerFn)dlsym(testSuite, "registerTests");
	if (registerTests == NULL)
	{
		dlclose(testSuite);
		return FALSE;
	}
	registerTests();
	return TRUE;
}

int runTests()
{
	uint32_t i;
	test *currTest;
	testLog *logFile = NULL;

	constParsedArg_t logging = findArg(parsedArgs, "--log", NULL);
	if (logging)
	{
		logFile = startLogging(logging->params[0]);
		loggingTests = 1;
	}

	for (i = 0; i < numTests; i++)
	{
		char *testLib = formatString("%s/%s." LIBEXT, workingDir, namedTests[i]->value);
		if (!testLib)
		{
			noMemory();
			return THREAD_ABORT;
		}
		void *testSuite = dlopen(testLib, RTLD_LAZY);
		free(testLib);
		if (testSuite == NULL || tryRegistration(testSuite) == FALSE)
		{
			if (testSuite == NULL)
			{
				if (isTTY != 0)
#ifndef _MSC_VER
					testPrintf(FAILURE);
#else
					SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
				testPrintf("Could not open test library: %s", dlerror());
				newline();
			}
			if (isTTY != 0)
#ifndef _MSC_VER
				testPrintf(FAILURE);
#else
				SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
			testPrintf("Test library %s was not a valid library, skipping", namedTests[i]->value);
			newline();
			continue;
		}
		if (isTTY != 0)
#ifndef _MSC_VER
			testPrintf(COLOUR("1;35"));
#else
			SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
		testPrintf("Running test suit %s...", namedTests[i]->value);
		newline();
		currTest = tests;
		while (currTest->testFunc)
		{
			int retVal = THREAD_ABORT;
			thrd_t testThread;
			thrd_create(&testThread, testRunner, currTest);
			thrd_join(testThread, &retVal);
			allocCount = -1;
			if (retVal == THREAD_ABORT)
				return retVal;
			++currTest;
		}
	}

	printStats();
	if (logging)
		stopLogging(logFile);
	return THREAD_SUCCESS;
}

#ifdef _WINDOWS
#define UNUSED(x) (void)x
void invalidHandler(const wchar_t *expr, const wchar_t *func, const wchar_t *file,
	const uint32_t line, const uintptr_t res)
{
	UNUSED(expr);
	UNUSED(func);
	UNUSED(file);
	UNUSED(line);
	UNUSED(res);
}
#endif

uint8_t handleVersionOrHelp()
{
	constParsedArg_t version = findArg(parsedArgs, "--version", NULL);
	constParsedArg_t versionShort = findArg(parsedArgs, "-v", NULL);
	constParsedArg_t help = findArg(parsedArgs, "--help", NULL);
	constParsedArg_t helpShort = findArg(parsedArgs, "-h", NULL);

	if (help || helpShort)
		puts(CRUNCH_HELP);
	else if (version || versionShort)
		testPrintf("crunch %s (%s %s %s-%s)\n", CRUNCH_VERSION, CRUNCH_COMPILER,
			CRUNCH_COMPILER_VERSION, CRUNCH_SYSTEM, CRUNCH_ARCH);
	else
		return FALSE;
	return TRUE;
}

void callFreeParsedArgs() { parsedArgs = freeParsedArgs(parsedArgs); }

int main(int argc, char **argv)
{
#ifdef _WINDOWS
	_set_invalid_parameter_handler(invalidHandler);
	_CrtSetReportMode(_CRT_ASSERT, 0);
	_CrtSetReportMode(_CRT_ERROR, 0);
#endif
	registerArgs(crunchArgs);
	parsedArgs = parseArguments(argc, (const char **)argv);
	if (parsedArgs && handleVersionOrHelp())
	{
		callFreeParsedArgs();
		return 0;
	}
	else if (!parsedArgs || !getTests())
	{
		callFreeParsedArgs();
		testPrintf("Fatal error: There are no tests to run given on the command line!\n");
		return 2;
	}
	workingDir = getcwd(NULL, 0);
#ifndef _MSC_VER
	isTTY = isatty(STDOUT_FILENO);
#else
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!console)
	{
		printf("Error: could not grab console!");
		return 1;
	}
	isTTY = (uint8_t)isatty(fileno(stdout));
#endif
	const int result = runTests();
	free((void *)namedTests);
	free((void *)workingDir);
	callFreeParsedArgs();
	if (result != THREAD_SUCCESS)
		return result == THREAD_ERROR ? 0 : 2;
	return failures == 0 ? 0 : 1;
}
