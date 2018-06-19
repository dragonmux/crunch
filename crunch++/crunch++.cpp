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

const arg_t args[] =
{
	{"--log", 1, 1, 0},
	{nullptr, 0, 0, 0}
};

#ifdef _MSC_VER
#define LIBEXT "tlib"
#else
#define LIBEXT "so"
#endif

parsedArgs_t parsedArgs;
parsedArgs_t namedTests;
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
		testPrintf("%0.2f%%\n", double(passes) / double(total) * 100.0);
}

bool getTests()
{
	uint32_t i, j, n;
	for (n = 0; parsedArgs[n] != nullptr; n++)
		continue;
	namedTests = makeUnique<constParsedArg_t []>(n + 1);
	if (!namedTests)
		return false;

	for (j = 0, i = 0; i < n; i++)
	{
		if (!findArgInArgs(parsedArgs[i]->value.get()))
		{
			namedTests[j] = parsedArgs[i];
			j++;
		}
	}
	if (j == 0)
		return false;
	parsedArgs_t tests = makeUnique<constParsedArg_t []>(j + 1);
	if (!tests)
		return false;
	std::copy(namedTests.get(), namedTests.get() + j, tests.get());
	namedTests = std::move(tests);
	namedTests[j] = nullptr;
	numTests = j;
	return true;
}

bool tryRegistration(void *testSuit)
{
	registerFn registerTests;
	registerTests = (registerFn)dlsym(testSuit, "registerCXXTests");
	if (!registerTests)
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

	constParsedArg_t logging = findArg(parsedArgs, "--log", nullptr);
	if (bool(logging))
	{
		logFile = startLogging(logging->params[0].get());
		loggingTests = true;
	}

	for (i = 0; i < numTests; i++)
	{
		auto testLib = formatString("%s/%s." LIBEXT, cwd, namedTests[i]->value.get());
		void *testSuit = dlopen(testLib.get(), RTLD_LAZY);
		if (!testSuit || !tryRegistration(testSuit))
		{
			if (!testSuit)
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
			testPrintf("Test library %s was not a valid library, skipping", namedTests[i]->value.get());
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
		testPrintf("Running test suit %s...", namedTests[i]->value.get());
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
			try
			{
				test.testClass->registerTests();
				test.testClass->test();
			}
			catch (threadExit_t &)
			{
				delete test.testClass;
				cxxTests.clear();
				printStats();
				if (logging != nullptr)
					stopLogging(logFile);
				throw;
			}
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
	registerArgs(args);
	parsedArgs = parseArguments(argc, argv);
	if (!parsedArgs || !getTests())
	{
		testPrintf("Fatal error: There are no tests to run given on the command line!\n");
		return 2;
	}
	cwd = getcwd(nullptr, 0);
#ifndef _MSC_VER
	isTTY = isatty(STDOUT_FILENO);
#else
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!console)
	{
		printf("Error: could not grab console!");
		return 1;
	}
	isTTY = isatty(fileno(stdout));
#endif
	try { runTests(); }
	catch (threadExit_t &val)
	{
		free((void *)cwd);
		return val;
	}
	free((void *)cwd);
	return failures ? 1 : 0;
}
