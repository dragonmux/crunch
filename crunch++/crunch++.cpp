/*
 * This file is part of crunch
 * Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * crunch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * crunch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pthread.h>
#include <string.h>
#include "Core.h"
#include "Logger.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "crunch++.h"
#include <dlfcn.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <io.h>
#endif
#include <exception>
#include <stdlib.h>

using namespace std;

CRUNCH_API const arg args[] =
{
	{"--log", 1, 1, 0},
	{nullptr, 0, 0, 0}
};

#ifdef _MSC_VER
#define LIBEXT "tlib"
#else
#define LIBEXT "so"
#endif

parsedArg **parsedArgs = nullptr;
parsedArg **namedTests = nullptr;
uint32_t numTests = 0;
const char *cwd = nullptr;

typedef void (__cdecl *registerFn)();

#ifdef _MSC_VER
void newline()
{
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	testPrintf("\n");
}
#endif

void printStats()
{
	uint64_t total = passes + failures;
	testPrintf("Total tests: %u,  Failures: %u,  Pass rate: ", total, failures);
	if (total == 0)
		testPrintf("--\n");
	else
		testPrintf("%0.2f%%\n", ((double)passes) / ((double)total) * 100.0);
}

bool getTests()
{
	uint32_t i, j, n;
	for (n = 0; parsedArgs[n] != nullptr; n++);
	namedTests = new parsedArg *[n + 1];

	for (j = 0, i = 0; i < n; i++)
	{
		if (findArgInArgs(parsedArgs[i]->value) == nullptr)
		{
			namedTests[j] = parsedArgs[i];
			j++;
		}
	}
	if (j == 0)
	{
		delete [] namedTests;
		return false;
	}
	else
	{
		parsedArg **tests = new parsedArg *[j + 1];
		memcpy(tests, namedTests, sizeof(parsedArg *) * (j + 1));
		delete [] namedTests;
		namedTests = tests;
		numTests = j;
		return true;
	}
}

bool tryRegistration(void *testSuit)
{
	registerFn registerTests;
	registerTests = (registerFn)dlsym(testSuit, "registerCXXTests");
	if (registerTests == nullptr)
	{
		dlclose(testSuit);
		return false;
	}
	registerTests();
	return true;
}

void runTests()
{
	uint32_t i;
	testLog *logFile = nullptr;

	parsedArg *logging = findArg(parsedArgs, "--log", nullptr);
	if (logging != nullptr)
	{
		logFile = startLogging(logging->params[0]);
		loggingTests = true;
	}

	for (i = 0; i < numTests; i++)
	{
		char *testLib = formatString("%s/%s." LIBEXT, cwd, namedTests[i]->value);
		void *testSuit = dlopen(testLib, RTLD_LAZY);
		free(testLib);
		if (testSuit == nullptr || tryRegistration(testSuit) == false)
		{
			if (testSuit == nullptr)
			{
				if (isTTY != 0)
#ifndef _MSC_VER
					testPrintf(FAILURE);
#else
					SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
				testPrintf("Could not open test library: %s", dlerror());
				if (isTTY != 0)
#ifndef _MSC_VER
					testPrintf(NEWLINE);
#else
					newline();
#endif
				else
					testPrintf("\n");
			}
			if (isTTY != 0)
#ifndef _MSC_VER
				testPrintf(FAILURE);
#else
				SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
			testPrintf("Test library %s was not a valid library, skipping", namedTests[i]->value);
			if (isTTY != 0)
#ifndef _MSC_VER
				testPrintf(NEWLINE);
#else
				newline();
#endif
			else
				testPrintf("\n");
			continue;
		}
		if (isTTY != 0)
#ifndef _MSC_VER
			testPrintf(COLOUR("1;35"));
#else
			SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
		testPrintf("Running test suit %s...", namedTests[i]->value);
		if (isTTY != 0)
#ifndef _MSC_VER
			testPrintf(NEWLINE);
#else
			newline();
#endif
		else
			testPrintf("\n");

		for (auto &test : cxxTests)
		{
			if (isTTY != 0)
#ifndef _MSC_VER
				testPrintf(COLOUR("1;35"));
#else
				SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
			testPrintf("Running tests in class %s...", test.testClassName);
			if (isTTY != 0)
#ifndef _MSC_VER
				testPrintf(NEWLINE);
#else
				newline();
#endif
			else
				testPrintf("\n");
			test.testClass->registerTests();
			test.testClass->test();
			delete test.testClass;
		}
		cxxTests.clear();
	}

	printStats();
	if (logging != nullptr)
		stopLogging(logFile);
}

int main(int argc, char **argv)
{
#ifdef _MSC_VER
	registerArgs(args);
#endif
	parsedArgs = parseArguments(argc, argv);
	if (parsedArgs == nullptr || !getTests())
	{
		testPrintf("Fatal error: There are no tests to run given on the command line!\n");
		return 2;
	}
	cwd = getcwd(nullptr, 0);
#ifndef _MSC_VER
	isTTY = isatty(STDOUT_FILENO);
#else
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (console == nullptr)
	{
		printf("Error: could not grab console!");
		exit(1);
	}
	isTTY = isatty(fileno(stdout));
#endif
	runTests();
	delete [] namedTests;
	free((void *)cwd);
	return 0;
}
