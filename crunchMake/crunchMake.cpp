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
#include <crunchMake.h>

using namespace std;

parsedArgs_t parsedArgs;
parsedArgs_t inclDirs, libDirs;
parsedArgs_t linkLibs, linkObjs;
parsedArgs_t namedTests, linkArgs;
uint32_t numTests = 0, numInclDirs = 0, numLibDirs = 0, numLibs = 0, numObjs = 0, numLinkArgs = 0;

#ifndef _MSC_VER
#define OPTS_VIS " -fvisibility=hidden -fvisibility-inlines-hidden"
#ifdef crunch_PREFIX
#define INCLUDE_OPTS_EXTRA " -I" crunch_PREFIX "/include"
#define LINK_OPTS_EXTRA " -L" crunch_LIBDIR " -Wl,-rpath," crunch_LIBDIR
#define OPTS_EXTRA INCLUDE_OPTS_EXTRA LINK_OPTS_EXTRA
#else
#define OPTS_EXTRA ""
#endif
#ifdef crunch_GUESSCOMPILER
#ifdef __x86_64__
const string cc = "gcc -m64 -fPIC -DPIC "_s;
string cxx = "g++ -m64 -fPIC -DPIC" OPTS_VIS " "_s;
#else
const string cc = "gcc -m32 "_s;
string cxx = "g++ -m32" OPTS_VIS " "_s;
#endif
#else
const string cc = crunch_GCC;
string cxx = crunch_GXX OPTS_VIS " "_s;
#endif
#define OPTS	"-shared" OPTS_EXTRA " %s%s%s%s%s-lcrunch%s -O2 %s -o "
const string libExt = ".so"_s;
#else
// _M_64
const string cc = "cl"_s;
const string cxx = "cl"_s;
#define OPTS	"/Gd /Ox /Ob2 /Oi /Oy- /GF /GS /Gy /EHsc /GL /GT /LD /D_WINDOWS /nologo %s%s%s%slibcrunch%s.lib %s /Fe"
const string libExt = ".tlib"_s;
#endif

template<typename T> using removeReference = typename std::remove_reference<T>::type;
template<typename type_t> constexpr type_t &&forward_(removeReference<type_t> &value) noexcept
	{ return static_cast<type_t &&>(value); }
template<typename type_t> constexpr type_t &&forward_(removeReference<type_t> &&value) noexcept
{
	static_assert(!std::is_lvalue_reference<type_t>::value,
		"template argument subtituting type_t is an lvalue reference type");
	return static_cast<type_t &&>(value);
}
const char *forward_(const string &value) noexcept { return value.data(); }
const char *forward_(const unique_ptr<char []> &value) noexcept { return value.get(); }
const char *forward_(const unique_ptr<const char []> &value) noexcept { return value.get(); }
template<typename... values_t> inline unique_ptr<char []> format(const string &format, values_t &&... values) noexcept
	{ return formatString(format.data(), forward_(values)...); }

const array<const char *, 8> exts = {".c", ".cpp", ".cc", ".cxx", ".i", ".s", ".S", ".sx"};
const array<const char *, 3> cxxExts = {".cpp", ".cc", ".cxx"};

std::unique_ptr<char []> inclDirFlags, libDirFlags, objs, libs;
bool silent, quiet, pthread, codeCoverage;

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
	{"-std=", 0, 0, ARG_INCOMPLETE},
	{"-z", 1, 1, ARG_REPEATABLE},
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
	for (n = 0; parsedArgs[n] != nullptr; ++n)
		continue;
	var = makeUnique<constParsedArg_t []>(n + 1);
	if (var == nullptr)
		return;
	for (num = 0, i = 0; i < n; ++i)
	{
		if (strncmp(parsedArgs[i]->value.get(), find, 2) == 0)
			var[num++] = parsedArgs[i];
	}
	if (num == 0)
		var = nullptr;

	parsedArgs_t vars = makeUnique<constParsedArg_t []>(num);
	if (vars == nullptr)
		return;
	std::copy(var.get(), var.get() + num, vars.get());
	var = std::move(vars);
}

void getLinkLibs() { getLinkFunc(linkLibs, numLibs, "-l"); }
void getLinkObjs() { getLinkFunc(linkObjs, numObjs, "-o"); }
void getInclDirs() { getLinkFunc(inclDirs, numInclDirs, "-I"); }
void getLibDirs() { getLinkFunc(libDirs, numLibDirs, "-L"); }

void getLinkArgs()
{
	uint32_t i, n, &num = numLinkArgs;
	for (n = 0; parsedArgs[n] != nullptr; ++n)
		continue;
	linkArgs = makeUnique<constParsedArg_t []>(n + 1);
	if (linkArgs == nullptr)
		return;
	for (num = 0, i = 0; i < n; ++i)
	{
		if (strcmp(parsedArgs[i]->value.get(), "-z") == 0)
			linkArgs[num++] = parsedArgs[i];
	}
	if (num == 0)
		linkArgs = nullptr;

	parsedArgs_t vars = makeUnique<constParsedArg_t []>(num);
	if (vars == nullptr)
		return;
	std::copy(linkArgs.get(), linkArgs.get() + num, vars.get());
	linkArgs = std::move(vars);
}

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
bool validExt(const std::unique_ptr<const char []> &file)
	{ return validExt(file.get()); }

bool isCXX(const char *file)
{
	const char *dot = strrchr(file, '.');
	for (auto &ext : cxxExts)
		if (strcmp(dot, ext) == 0)
			return true;
	return false;
}
bool isCXX(const std::unique_ptr<const char []> &file)
	{ return isCXX(file.get()); }

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
	for (uint32_t i = 0; i < num; ++i)
		ret = format("%s%s "_s, ret, var[i]->value.get() + offset);
	var = nullptr;
	return ret;
}

inline std::unique_ptr<char []> argParamsToString(parsedArgs_t &var, const uint32_t num, const  uint32_t offset)
{
	std::unique_ptr<char []> ret = stringDup("");
	for (uint32_t i = 0; i < num; ++i)
	{
		ret = format("%s%s "_s, ret, var[i]->value.get() + offset);
		for (uint32_t param = 0; param < var[i]->paramsFound; ++param)
			ret = format("%s%s "_s, ret, var[i]->params[param]);
	}
	var = nullptr;
	return ret;
}

void inclDirFlagsToString() { inclDirFlags = argsToString(inclDirs, numInclDirs, 0); }
void libDirFlagsToString() { libDirFlags = argsToString(libDirs, numLibDirs, 0); }
void objsToString() { objs = argsToString(linkObjs, numObjs, 2); }
void libsToString()
{
	auto libsString = argsToString(linkLibs, numLibs, 0);
	auto args = argParamsToString(linkArgs, numLinkArgs, 0);
	libs = format("%s%s"_s, libsString, args);
}

#ifndef _MSC_VER
const char *standardVersion(constParsedArg_t version)
{
	if (!version)
		return "-std=c++11";
	const auto str = version->value.get() + 5;
	if (strlen(str) != 5 || strncmp(str, "c++", 3) != 0 || str[3] == '8' || str[3] == '9')
	{
		testPrintf("Warning, standard version must be at least C++11");
		return "-std=c++11";
	}
	return version->value.get();
}
#endif

void buildCXXString()
{
#ifndef _MSC_VER
	constParsedArg_t standard = findArg(parsedArgs, "-std=", nullptr);
	cxx += standardVersion(standard);
#endif
}

int compileTests()
{
	uint32_t i;
	int32_t ret = 0;
	inclDirFlagsToString();
	libDirFlagsToString();
	objsToString();
	libsToString();
	buildCXXString();
	testLog *logFile = nullptr;
	constParsedArg_t logParam = findArg(parsedArgs, "--log", nullptr);
	const bool logging = bool(logParam);
	if (logging)
		logFile = startLogging(logParam->params[0].get());
	if (!silent)
		silent = bool(findArg(parsedArgs, "-s", nullptr));
	if (!quiet)
		quiet = bool(findArg(parsedArgs, "-q", nullptr));

	for (i = 0; i < numTests; i++)
	{
		if (access(namedTests[i]->value.get(), R_OK) == 0 && validExt(namedTests[i]->value))
		{
			const bool mode = isCXX(namedTests[i]->value.get());
			const string &compiler = mode ? cxx : cc;
			std::unique_ptr<char []> soFile = toSO(namedTests[i]->value.get());
			std::unique_ptr<char []> compileString = formatString("%s %s " OPTS "%s", compiler.data(),
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
			if (ret)
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
	getLinkArgs();
	silent = bool(findArg(parsedArgs, "--silent", nullptr));
	quiet = bool(findArg(parsedArgs, "--quiet", nullptr));
	pthread = bool(findArg(parsedArgs, "-pthread", nullptr));
	codeCoverage = bool(findArg(parsedArgs, "--coverage", nullptr));
	return compileTests();
}
