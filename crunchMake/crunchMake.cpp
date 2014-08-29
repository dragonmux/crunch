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

#include "Core.h"
#include "Logger.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Memory.h"
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#define R_OK 6
#endif

parsedArg **parsedArgs = NULL;
parsedArg **inclDirs = NULL;
parsedArg **libDirs = NULL;
parsedArg **linkLibs = NULL;
parsedArg **linkObjs = NULL;
parsedArg **namedTests = NULL;
uint32_t numTests = 0, numInclDirs = 0, numLibDirs = 0, numLibs = 0, numObjs = 0;

#ifndef _MSC_VER
#ifdef crunch_GUESSCOMPILER
#ifdef __x86_64__
#define COMPILER	"gcc -m64 -fPIC -DPIC"
#else
#define COMPILER	"gcc -m32"
#endif
#else
#define COMPILER crunch_GCC
#endif
#define OPTS	"-shared %s%s%s%s-lcrunch -O2 %s -o "
#define LIBEXT ".so"
#else
// _M_64
#define COMPILER	"cl"
#define OPTS	"/Gd /Ox /Ob2 /Oi /Oy- /GF /GS /Gy /EHsc /GL /GT /LD /D_WINDOWS /nologo %s%s%s%slibcrunch.lib %s /Fe"
#define LIBEXT ".tlib"
#endif

CRUNCH_API const arg args[] =
{
	{"-l", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-o", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-I", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-L", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"--log", 1, 1, 0},
	{"--silent", 0, 0, 0},
	{"-s", 0, 0, 0},
	{"--quiet", 0, 0, 0},
	{"-q", 0, 0, 0},
	{"-pthread", 0, 0, 0},
	{nullptr, 0, 0, 0}
};

bool getTests()
{
	uint32_t n, j = 0;
	for (n = 0; parsedArgs[n] != nullptr; n++);
	namedTests = new parsedArg *[n + 1]();

	for (uint32_t i = 0; i < n; i++)
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
		parsedArg **tests = new parsedArg *[j];
		memcpy(tests, namedTests, sizeof(parsedArg *) * j);
		delete [] namedTests;
		namedTests = tests;
		numTests = j;
		return true;
	}
}

inline void getLinkFunc(parsedArg **&var, uint32_t &num, const char *find)
{
	uint32_t i, n;
	for (n = 0; parsedArgs[n] != nullptr; n++);
	var = new parsedArg *[n + 1]();
	for (num = 0, i = 0; i < n; i++)
	{
		if (strncmp(parsedArgs[i]->value, find, 2) == 0)
		{
			var[num] = parsedArgs[i];
			num++;
		}
	}
	if (num == 0)
	{
		delete [] var;
		var = nullptr;
	}
	else
	{
		parsedArg **vars = new parsedArg *[num]();
		memcpy(vars, var, sizeof(parsedArg *) * num);
		delete [] var;
		var = vars;
	}
}

void getLinkLibs()
{
	getLinkFunc(linkLibs, numLibs, "-l");
}

void getLinkObjs()
{
	getLinkFunc(linkObjs, numObjs, "-o");
}

void getInclDirs()
{
	getLinkFunc(inclDirs, numInclDirs, "-I");
}

void getLibDirs()
{
	getLinkFunc(libDirs, numLibDirs, "-L");
}

static const char *exts[] = {".c", ".cpp", ".cc", ".cxx", ".i", ".s", ".S", ".sx"};
static const int numExts = sizeof(exts) / sizeof(*exts);

bool validExt(const char *file)
{
	int i;
	const char *dot = strrchr(file, '.');
	if (dot == nullptr)
		return false;
	for (i = 0; i < numExts; i++)
	{
		if (strcmp(dot, exts[i]) == 0)
			return true;
	}
	return false;
}

const char *toSO(const char *file)
{
	char *soFile;
	const char *dot = strrchr(file, '.');
	size_t dotPos = dot - file;
	soFile = new char[dotPos + strlen(LIBEXT) + 1]();
	memcpy(soFile, file, dotPos);
	memcpy(soFile + dotPos, LIBEXT, strlen(LIBEXT) + 1);
	return soFile;
}

inline const char *argsToString(parsedArg **var, const uint32_t num, const  uint32_t offset)
{
	const char *ret = strNewDup("");
	for (uint32_t i = 0; i < num; i++)
	{
		const char *name = formatString("%s%s ", ret, var[i]->value + offset);
		delete [] ret;
		ret = name;
	}
	delete [] var;
	return ret;
}

const char *inclDirFlagsToString()
{
	return argsToString(inclDirs, numInclDirs, 0);
}

const char *libDirFlagsToString()
{
	return argsToString(libDirs, numLibDirs, 0);
}

const char *objsToString()
{
	return argsToString(linkObjs, numObjs, 2);
}

const char *libsToString()
{
	return argsToString(linkLibs, numLibs, 0);
}

int compileTests()
{
	uint32_t i, ret = 0;
	const char *inclDirFlags = inclDirFlagsToString();
	const char *libDirFlags = libDirFlagsToString();
	const char *objs = objsToString();
	const char *libs = libsToString();
	parsedArg *silent = findArg(parsedArgs, "--silent", nullptr);
	testLog *logFile = nullptr;
	parsedArg *logging = findArg(parsedArgs, "--log", nullptr);
	bool quiet = findArg(parsedArgs, "--quiet", nullptr) != nullptr;
	parsedArg *pthread = findArg(parsedArgs, "-pthread", nullptr);
	if (logging != nullptr)
		logFile = startLogging(logging->params[0]);
	if (silent == NULL)
		silent = findArg(parsedArgs, "-s", NULL);
	if (!quiet)
		quiet = findArg(parsedArgs, "-q", nullptr) != nullptr;

	for (i = 0; i < numTests; i++)
	{
		if (access(namedTests[i]->value, R_OK) == 0 && validExt(namedTests[i]->value))
		{
			char *displayString;
			const char *soFile = toSO(namedTests[i]->value);
			char *compileString = formatString(COMPILER " %s " OPTS "%s", namedTests[i]->value, inclDirFlags, libDirFlags, objs, libs, (pthread == nullptr ? "" : pthread->value), soFile);
			if (quiet)
				displayString = formatString(" CCLD  %s => %s", namedTests[i]->value, soFile);
			else
				displayString = compileString;
			if (silent == NULL)
			{
				printf("%s\n", displayString);
				if (displayString != compileString)
					free(displayString);
			}
			ret = system(compileString);
			free(compileString);
			delete [] soFile;
			if (ret != 0)
				break;
		}
		else
			testPrintf("Error, %s does not exist, skipping..\n", namedTests[i]->value);
	}
	delete [] inclDirFlags;
	delete [] libDirFlags;
	delete [] objs;
	delete [] libs;
	if (logging != nullptr)
		stopLogging(logFile);
	return ret;
}

int main(int argc, char **argv)
{
#ifdef _MSC_VER
	registerArgs(args);
#endif
	parsedArgs = parseArguments(argc, argv);
	if (parsedArgs == nullptr || !getTests())
	{
		testPrintf("Fatal error: There are no source files to build given on the command line!\n");
		return 2;
	}
	getInclDirs();
	getLibDirs();
	getLinkLibs();
	getLinkObjs();
	return compileTests();
}
