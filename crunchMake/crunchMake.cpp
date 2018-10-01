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
vector<string> inclDirs, libDirs;
vector<string> linkLibs, linkObjs;
vector<string> linkArgs, tests;
uint32_t numLinkArgs = 0;

#ifndef _MSC_VER
#define OPTS_VIS " -fvisibility=hidden -fvisibility-inlines-hidden"
#ifdef crunch_PREFIX
#define INCLUDE_OPTS_EXTRA " -I" crunch_PREFIX "/include"
#define LINK_OPTS_EXTRA " -L" crunch_LIBDIR " -Wl,-rpath," crunch_LIBDIR
#define OPTS_EXTRA INCLUDE_OPTS_EXTRA LINK_OPTS_EXTRA
#else
#define INCLUDE_OPTS_EXTRA ""
#define LINK_OPTS_EXTRA ""
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
#define COMPILE_OPTS "-c" INCLUDE_OPTS_EXTRA " %s -O2 %s -o "
#define LINK_OPTS "-shared " LINK_OPTS_EXTRA " %s%s%s%s-lcrunch%s -O2 %s -o "
const string libExt = ".so"_s;
#else
// _M_64
const string cc = "cl"_s;
const string cxx = "cl"_s;
#define COMPILE_OPTS "/Gd /Ox /Ob2 /Oi /Oy- /GF /GS /Gy /EHsc /GL /GT /D_WINDOWS /nologo %s%s%slibcrunch%s.lib /Fe"
#define LINK_OPTS "/LD /link %s"
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

template<typename T, typename... values_t> constexpr array<T, sizeof...(values_t)> makeArray(values_t &&... values)
	{ return {forward<values_t>(values)...}; }

const auto exts = makeArray<const char *>(".c", ".cpp", ".cc", ".cxx", ".i", ".s", ".S", ".sx");
const auto cxxExts = makeArray<const char *>(".cpp", ".cc", ".cxx");
const auto objExts = makeArray<const char *>(".o", ".obj", ".a");

string inclDirFlags, libDirFlags, objs, libs;
bool silent, quiet, pthread, codeCoverage;

const arg_t args[] =
{
	{"-l", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-I", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-L", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-o", 1, 1, 0},
	{"--log", 1, 1, 0},
	{"--silent", 0, 0, 0},
	{"-s", 0, 0, 0},
	{"--quiet", 0, 0, 0},
	{"-q", 0, 0, 0},
	{"-pthread", 0, 0, 0},
	{"-Wl", 0, 0, ARG_INCOMPLETE},
	{"-std=", 0, 0, ARG_INCOMPLETE},
	{"-z", 1, 1, ARG_REPEATABLE},
	{"--coverage", 0, 0, 0},
	{nullptr, 0, 0, 0}
};

bool isObj(const char *file)
{
	const char *dot = strrchr(file, '.');
	for (auto &ext : objExts)
		if (strcmp(dot, ext) == 0)
			return true;
	return false;
}
bool isObj(const std::unique_ptr<const char []> &file)
	{ return isObj(file.get()); }

bool getTests()
{
	for (uint32_t i = 0; parsedArgs[i] != nullptr; ++i)
	{
		const auto &value = parsedArgs[i]->value;
		if (!findArgInArgs(value) && !isObj(value))
			tests.emplace_back(value.get());
	}
	return tests.size();
}

inline void getLinkFunc(vector<string> &var, const char *find)
{
	for (uint32_t i = 0; parsedArgs[i] != nullptr; ++i)
	{
		const auto &value = parsedArgs[i]->value;
		if (strncmp(value.get(), find, 2) == 0)
			var.emplace_back(value.get());
	}
}

void getLinkLibs() { getLinkFunc(linkLibs, "-l"); }
void getInclDirs() { getLinkFunc(inclDirs, "-I"); }
void getLibDirs() { getLinkFunc(libDirs, "-L"); }

void getLinkObjs()
{
	for (uint32_t i = 0; parsedArgs[i] != nullptr; ++i)
	{
		const auto &value = parsedArgs[i]->value;
		if (!findArgInArgs(value) && isObj(value))
			linkObjs.emplace_back(value.get());
	}
}

inline string argToString(const parsedArg_t &arg)
{
	string ret{arg.value.get()};
	ret += ' ';
	for (uint32_t i = 0; i < arg.paramsFound; ++i)
	{
		ret += arg.params[i].get();
		ret += ' ';
	}
	return ret;
}

void getLinkArgs()
{
	for (uint32_t i = 0; parsedArgs[i] != nullptr; ++i)
	{
		if (strcmp(parsedArgs[i]->value.get(), "-z") == 0)
			linkArgs.emplace_back(argToString(*parsedArgs[i]));
	}
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
bool validExt(const string &file)
	{ return validExt(file.data()); }

bool isCXX(const char *file)
{
	const char *dot = strrchr(file, '.');
	for (auto &ext : cxxExts)
		if (strcmp(dot, ext) == 0)
			return true;
	return false;
}
bool isCXX(const string &file)
	{ return isCXX(file.data()); }

string toO(const string &file)
{
	const size_t dotPos = file.find_last_of('.');
	auto soFile = file.substr(0, dotPos);
	return soFile += ".o"_s;
}
string toO(const unique_ptr<const char []> &file)
	{ return toO(file.get()); }

string computeObjName(const string &file)
{
	const auto output = findArg(parsedArgs, "-o", nullptr);
	if (output)
		return toO(output->params[0]);
	return toO(file);
}

string toSO(const string &file)
{
	const size_t dotPos = file.find_last_of('.');
	auto soFile = file.substr(0, dotPos);
	return soFile += libExt;
}

string computeSOName(const string &file)
{
	const auto output = findArg(parsedArgs, "-o", nullptr);
	if (output)
		return output->params[0].get();
	return toSO(file);
}

inline string argsToString(vector<string> &var)
{
	string ret{""};
	for (const auto &value : var)
		ret += value + ' ';
	var.clear();
	return std::move(ret);
}

void inclDirFlagsToString() { inclDirFlags = argsToString(inclDirs); }
void libDirFlagsToString() { libDirFlags = argsToString(libDirs); }
void objsToString() { objs = argsToString(linkObjs); }
void libsToString() { libs = argsToString(linkLibs) + argsToString(linkArgs); }

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
	const auto standard = findArg(parsedArgs, "-std=", nullptr);
	cxx += standardVersion(standard);
#endif
}

int32_t compileGCC(const string &test)
{
	const bool mode = isCXX(test);
	const string &compiler = mode ? cxx : cc;
	auto soFile = computeSOName(test);
	auto compileString = format("%s %s " OPTS "%s"_s, compiler, test, inclDirFlags,
		libDirFlags, objs, libs, codeCoverage ? "-lgcov " : "",  mode ? "++" : "",
		pthread ? "" : "-pthread", soFile);
	if (!silent)
	{
		if (quiet)
		{
			auto displayString = format(" CCLD  %s => %s"_s, test, soFile);
			puts(displayString.get());
		}
		else
			puts(compileString.get());
	}
	return system(compileString.get());
}

int32_t compileClang(const string &test)
{
	const bool mode = isCXX(test);
	const string &compiler = mode ? cxx : cc;
	auto oFile = computeObjName(test);
	auto compileString = format("%s %s " COMPILE_OPTS "%s"_s, compiler, test,
		inclDirFlags, pthread ? "" : "-pthread", oFile);
	if (!silent)
	{
		if (quiet)
		{
			auto displayString = format(" CC    %s => %s"_s, test, oFile);
			puts(displayString.get());
		}
		else
			puts(compileString.get());
	}
	int32_t ret = system(compileString.get());
	if (ret)
		return ret;

	auto soFile = computeSOName(test);
	auto linkString = format("%s %s " LINK_OPTS "%s"_s, compiler, oFile, libDirFlags, objs,
		libs, codeCoverage ? "--coverage " : "", mode ? "++" : "", pthread ? "" : "-pthread", soFile);
	if (!silent)
	{
		if (quiet)
		{
			auto displayString = format(" CCLD  %s => %s"_s, oFile, soFile);
			puts(displayString.get());
		}
		else
			puts(linkString.get());
	}
	return system(linkString.get());
}

int32_t compileMSVC(const string &test)
{
	auto soFile = computeSOName(test);
	auto compileString = format("cl %s " COMPILE_OPTS "%s " LINK_OPTS ""_s, test,
		inclDirFlags, objs, libs, mode ? "++" : "", soFile, libDirFlags);
	if (!silent)
	{
		if (quiet)
		{
			auto displayString = format(" CCLD  %s => %s"_s, test, soFile);
			puts(displayString.get());
		}
		else
			puts(compileString.get());
	}
	return system(compileString.get());
}

int compileTests()
{
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

	for (const auto &test : tests)
	{
		if (access(test.data(), R_OK) == 0 && validExt(test))
		{
#ifndef _MSC_VER
			if (crunch_COMPILER == "clang"_s)
				ret = compileClang(test);
			else if (crunch_COMPILER == "gcc"_s)
				ret = compileGCC(test);
#else
			ret = compileMSVC(test);
#endif
			if (ret)
				break;
		}
		else
			testPrintf("Error, %s does not exist, skipping..\n", test.data());
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
