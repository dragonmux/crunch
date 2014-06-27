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
#endif

parsedArg **parsedArgs = NULL;
parsedArg **inclDirs = NULL;
parsedArg **libDirs = NULL;
parsedArg **linkLibs = NULL;
parsedArg **linkObjs = NULL;
parsedArg **namedTests = NULL;
uint32_t numTests = 0, numInclDirs = 0, numLibDirs = 0, numLibs = 0, numObjs = 0;

#ifndef __MSC_VER
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
#else
// _M_64
// TODO: Figure out the trickery needed to get this working!
#define COMPILER	"cl"
#endif

arg args[] =
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
	{0}
};

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

#define getLinkFunc(name, var, num, find) \
void name() \
{ \
	uint32_t i, n; \
	for (n = 0; parsedArgs[n] != NULL; n++); \
	var = testMalloc(sizeof(parsedArg *) * (n + 1)); \
	for (num = 0, i = 0; i < n; i++) \
	{ \
		if (strncmp(parsedArgs[i]->value, find, 2) == 0) \
		{ \
			var[num] = parsedArgs[i]; \
			num++; \
		} \
	} \
	if (num == 0) \
		free(var); \
	else \
		var = testRealloc(var, sizeof(parsedArg *) * (num + 1)); \
}

getLinkFunc(getLinkLibs, linkLibs, numLibs, "-l")
getLinkFunc(getLinkObjs, linkObjs, numObjs, "-o")
getLinkFunc(getInclDirs, inclDirs, numInclDirs, "-I")
getLinkFunc(getLibDirs, libDirs, numLibDirs, "-L")
#undef getLinkFunc

static const char *exts[] = {".c", ".cpp", ".i", ".s", ".S", ".sx"};
static const int numExts = sizeof(exts) / sizeof(*exts);

uint8_t validExt(const char *file)
{
	int i;
	const char *dot = strrchr(file, '.');
	if (dot == NULL)
		return FALSE;
	for (i = 0; i < numExts; i++)
	{
		if (strcmp(dot, exts[i]) == 0)
			return TRUE;
	}
	return FALSE;
}

const char *toSO(const char *file)
{
	char *soFile;
	const char *dot = strrchr(file, '.');
	size_t dotPos = dot - file;
	soFile = testMalloc(dotPos + 4);
	memcpy(soFile, file, dotPos);
	memcpy(soFile + dotPos, ".so", 4);
	return soFile;
}

#define toStringFunc(name, var, num, offset) \
const char *name ## ToString() \
{ \
	int i; \
	const char *ret = strdup(""); \
	for (i = 0; i < num; i++) \
	{ \
		const char *name = formatString("%s%s ", ret, var[i]->value offset); \
		free((void *)ret); \
		ret = name; \
	} \
	return ret; \
}

toStringFunc(inclDirFlags, inclDirs, numInclDirs, )
toStringFunc(libDirFlags, libDirs, numLibDirs, )
toStringFunc(objs, linkObjs, numObjs, + 2)
toStringFunc(libs, linkLibs, numLibs, )
#undef toStringFunc

int compileTests()
{
	int i, ret = 0;
	const char *inclDirFlags = inclDirFlagsToString();
	const char *libDirFlags = libDirFlagsToString();
	const char *objs = objsToString();
	const char *libs = libsToString();
	parsedArg *silent = findArg(parsedArgs, "--silent", NULL);
	log *logFile = NULL;
	parsedArg *logging = findArg(parsedArgs, "--log", NULL);
	parsedArg *quiet = findArg(parsedArgs, "--quiet", NULL);
	parsedArg *pthread = findArg(parsedArgs, "-pthread", NULL);
	if (logging != NULL)
		logFile = startLogging(logging->params[0]);
	if (silent == NULL)
		silent = findArg(parsedArgs, "-s", NULL);
	if (quiet == NULL)
		quiet = findArg(parsedArgs, "-q", NULL);

	for (i = 0; i < numTests; i++)
	{
		if (access(namedTests[i]->value, R_OK) == 0 && validExt(namedTests[i]->value) != FALSE)
		{
			char *displayString;
			const char *soFile = toSO(namedTests[i]->value);
			char *compileString = formatString(COMPILER " %s " OPTS "%s", namedTests[i]->value, inclDirFlags, libDirFlags, objs, libs, (pthread == NULL ? "" : pthread->value), soFile);
			if (quiet != NULL)
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
			free((void *)soFile);
			if (ret != 0)
				break;
		}
		else
			testPrintf("Error, %s does not exist, skipping..\n", namedTests[i]->value);
	}
	free((void *)objs);
	free((void *)libs);
	if (logging != NULL)
		stopLogging(logFile);
	return ret;
}

int main(int argc, char **argv)
{
#ifdef _MSC_VER
	registerArgs(args);
#endif
	parsedArgs = parseArguments(argc, argv);
	if (parsedArgs == NULL || getTests() == FALSE)
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
