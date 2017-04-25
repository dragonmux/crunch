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
#include <string.h>
#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#define R_OK 6
#endif
#include <array>
#include <string>

using namespace std;

parsedArgs_t parsedArgs;
parsedArgs_t inclDirs, libDirs;
parsedArgs_t linkLibs, linkObjs;
parsedArgs_t namedTests;
uint32_t numTests = 0, numInclDirs = 0, numLibDirs = 0, numLibs = 0, numObjs = 0;

#ifndef _MSC_VER
#ifdef crunch_PREFIX
#define OPTS_EXTRA " -I" crunch_PREFIX "/include -L" crunch_LIBDIR " -Wl,-rpath," crunch_LIBDIR
#else
#define OPTS_EXTRA ""
#endif
#ifdef crunch_GUESSCOMPILER
#ifdef __x86_64__
const char *const cc = "gcc -m64 -fPIC -DPIC" OPTS_EXTRA;
const char *const cxx = "g++ -m64 -fPIC -DPIC -std=c++11" OPTS_EXTRA;
#else
const char *const cc = "gcc -m32" OPTS_EXTRA;
const char *const cxx = "g++ -m32 -std=c++11" OPTS_EXTRA;
#endif
#else
const char *const cc = crunch_GCC OPTS_EXTRA;
const char *const cxx = crunch_GXX " -std=c++11" OPTS_EXTRA;
#endif
#define OPTS	"-shared %s%s%s%s-lcrunch%s -O2 %s -o "
const string libExt = ".so";
#else
// _M_64
const char *const cc = "cl";
const char *const cxx = "cl";
#define OPTS	"/Gd /Ox /Ob2 /Oi /Oy- /GF /GS /Gy /EHsc /GL /GT /LD /D_WINDOWS /nologo %s%s%s%slibcrunch%s.lib %s /Fe"
const string libExt = ".tlib";
#endif

const array<const char *, 8> exts = {".c", ".cpp", ".cc", ".cxx", ".i", ".s", ".S", ".sx"};
const array<const char *, 3> cxxExts = {".cpp", ".cc", ".cxx"};

const arg_t args[] =
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
	{"-Wl", 0, 0, ARG_INCOMPLETE},
	{nullptr, 0, 0, 0}
};

bool getTests()
{
	uint32_t n, j = 0;
	for (n = 0; parsedArgs[n] != nullptr; n++);
	namedTests = makeUnique<constParsedArg_t []>(n + 1);
	if (!namedTests)
		return false;

	for (uint32_t i = 0; i < n; i++)
	{
		if (!findArgInArgs(parsedArgs[i]->value.get()))
		{
			namedTests[j] = parsedArgs[i];
			j++;
		}
	}
	if (j == 0)
		return false;
	parsedArgs_t tests = makeUnique<constParsedArg_t []>(j);
	if (!tests)
		return false;
	std::copy(namedTests.get(), namedTests.get() + j, tests.get());
	namedTests = std::move(tests);
	numTests = j;
	return true;
}

inline void getLinkFunc(parsedArgs_t &var, uint32_t &num, const char *find)
{
	uint32_t i, n;
	for (n = 0; parsedArgs[n] != nullptr; n++)
		continue;
	var = makeUnique<constParsedArg_t []>(n + 1);
	for (num = 0, i = 0; i < n; i++)
	{
		if (strncmp(parsedArgs[i]->value.get(), find, 2) == 0)
		{
			var[num] = parsedArgs[i];
			num++;
		}
	}
	if (num == 0)
		var = nullptr;

	parsedArgs_t vars = makeUnique<constParsedArg_t []>(num);
	std::copy(var.get(), var.get() + num, vars.get());
	var = std::move(vars);
}

void getLinkLibs() { getLinkFunc(linkLibs, numLibs, "-l"); }
void getLinkObjs() { getLinkFunc(linkObjs, numObjs, "-o"); }
void getInclDirs() { getLinkFunc(inclDirs, numInclDirs, "-I"); }
void getLibDirs() { getLinkFunc(libDirs, numLibDirs, "-L"); }

bool validExt(const char *file)
{
	const char *dot = strrchr(file, '.');
	if (dot == nullptr)
		return false;
	for (auto &ext : exts)
		if (strcmp(dot, ext) == 0)
			return true;
	return false;
}

bool isCXX(const char *file)
{
	const char *dot = strrchr(file, '.');
	for (auto &ext : cxxExts)
		if (strcmp(dot, ext) == 0)
			return true;
	return false;
}

std::unique_ptr<char []> toSO(const char *const file)
{
	const char *const dot = strrchr(file, '.');
	const size_t dotPos = dot - file;
	auto soFile = makeUnique<char []>(dotPos + libExt.size() + 1);
	memcpy(soFile.get(), file, dotPos);
	memcpy(soFile.get() + dotPos, libExt.data(), libExt.size() + 1);
	return soFile;
}

inline std::unique_ptr<char []> argsToString(parsedArgs_t &var, const uint32_t num, const  uint32_t offset)
{
	std::unique_ptr<char []> ret = stringDup("");
	for (uint32_t i = 0; i < num; i++)
		ret = std::move(formatString("%s%s ", ret.get(), var[i]->value.get() + offset));
	var = nullptr;
	return ret;
}

std::unique_ptr<char []> inclDirFlagsToString() { return argsToString(inclDirs, numInclDirs, 0); }
std::unique_ptr<char []> libDirFlagsToString() { return argsToString(libDirs, numLibDirs, 0); }
std::unique_ptr<char []> objsToString() { return argsToString(linkObjs, numObjs, 2); }
std::unique_ptr<char []> libsToString() { return argsToString(linkLibs, numLibs, 0); }

int compileTests()
{
	uint32_t i, ret = 0;
	auto inclDirFlags = inclDirFlagsToString();
	auto libDirFlags = libDirFlagsToString();
	auto objs = objsToString();
	auto libs = libsToString();
	bool silent = bool(findArg(parsedArgs, "--silent", nullptr));
	testLog *logFile = nullptr;
	constParsedArg_t logParam = findArg(parsedArgs, "--log", nullptr);
	const bool logging = bool(logParam);
	bool quiet = bool(findArg(parsedArgs, "--quiet", nullptr));
	const bool pthread = bool(findArg(parsedArgs, "-pthread", nullptr));
	if (logging)
		logFile = startLogging(logParam->params[0].get());
	if (!silent)
		silent = bool(findArg(parsedArgs, "-s", nullptr));
	if (!quiet)
		quiet = bool(findArg(parsedArgs, "-q", nullptr));

	for (i = 0; i < numTests; i++)
	{
		if (access(namedTests[i]->value.get(), R_OK) == 0 && validExt(namedTests[i]->value.get()))
		{
			const bool mode = isCXX(namedTests[i]->value.get());
			const char *const compiler = mode ? cxx : cc;
			std::unique_ptr<char []> soFile = toSO(namedTests[i]->value.get());
			std::unique_ptr<char []> compileString = formatString("%s %s " OPTS "%s", compiler,
				namedTests[i]->value.get(), inclDirFlags.get(), libDirFlags.get(), objs.get(), libs.get(),
				mode ? "++" : "", pthread ? "" : "-pthread", soFile.get());
			if (!silent)
			{
				std::unique_ptr<char []> displayString;
				if (quiet)
					displayString = formatString(" CCLD  %s => %s", namedTests[i]->value.get(), soFile.get());
				else
					displayString = stringDup(compileString.get());
				printf("%s\n", displayString.get());
			}
			ret = system(compileString.get());
			if (ret != 0)
				break;
		}
		else
			testPrintf("Error, %s does not exist, skipping..\n", namedTests[i]->value.get());
	}
	if (logging)
		stopLogging(logFile);
	return ret;
}

int main(int argc, char **argv)
{
	registerArgs(args);
	parsedArgs = parseArguments(argc, argv);
	if (!parsedArgs || !getTests())
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
