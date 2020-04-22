/*
 * This file is part of crunch
 * Copyright © 2013-2020 Rachel Mant (dx-mon@users.sourceforge.net)
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
#include <stdlib.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#define R_OK 6
#endif
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <core.hxx>
#include <logger.hxx>
#include <argsParser.hxx>
#include <stringFuncs.hxx>
#include "crunchCompiler.hxx"
#include <crunchMake.h>

using namespace std;

parsedArgs_t parsedArgs;
vector<string> inclDirs, libDirs;
vector<string> linkLibs, linkObjs;
vector<string> linkArgs, tests;
uint32_t numLinkArgs = 0;

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
bool silent, quiet, pthread, codeCoverage, debugBuild;

const arg_t args[] =
{
	{"-l", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-I", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-D", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
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
	{"--debug", 0, 0, 0},
	{"-fsanitize=", 0, 0, ARG_INCOMPLETE},
	{{}, 0, 0, 0}
};

bool isObj(const char *file)
{
	const char *dot = strrchr(file, '.');
	for (auto &ext : objExts)
		if (strcmp(dot, ext) == 0)
			return true;
	return false;
}
bool isObj(const string &file)
	{ return isObj(file.data()); }

bool getTests()
{
	for (const auto &parsedArg : parsedArgs)
	{
		const auto &value = parsedArg.value;
		if (!findArgInArgs(value.data()) && !isObj(value))
			tests.emplace_back(value);
	}
	return tests.size();
}

inline void getLinkFunc(vector<string> &var, const char *find)
{
	for (const auto &parsedArg : parsedArgs)
	{
		const auto &value = parsedArg.value;
		if (strncmp(value.data(), find, 2) == 0)
			var.emplace_back(value);
	}
}

void getInclDirs()
{
	getLinkFunc(inclDirs, "-I");
	getLinkFunc(inclDirs, "-D");
}
void getLinkLibs() { getLinkFunc(linkLibs, "-l"); }
void getLibDirs() { getLinkFunc(libDirs, "-L"); }

void getLinkObjs()
{
	for (const auto &parsedArg : parsedArgs)
	{
		const auto &value = parsedArg.value;
		if (!findArgInArgs(value.data()) && isObj(value))
			linkObjs.emplace_back(value);
	}
}

inline string argToString(const parsedArg_t &arg)
{
	string ret{arg.value};
	ret += ' ';
	for (uint32_t i = 0; i < arg.paramsFound; ++i)
		ret += arg.params[i] + ' ';
	return ret;
}

void getLinkArgs()
{
	for (const auto &parsedArg : parsedArgs)
	{
		if (parsedArg.matches("-z"))
			linkArgs.emplace_back(argToString(parsedArg));
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

#ifndef _MSC_VER
string toO(const string &file)
{
	const size_t dotPos = file.find_last_of('.');
	auto objFile = file.substr(0, dotPos);
	return objFile += ".o"s;
}
#else
string toO(const string &file)
{
	const size_t dotPos = file.find_last_of('.');
	auto objFile = file.substr(0, dotPos);
	return objFile += ".obj"s;
}
#endif

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
		return output->params[0];
	return toSO(file);
}

inline string argsToString(const vector<string> &var)
{
	string ret{""};
	for (const auto &value : var)
		ret += value + ' ';
	return ret;
}

#ifdef _MSC_VER
string convertLibDirs()
{
	string ret{""};
	for (const auto &value : libDirs)
		ret += "-libpath:" + value.substr(2) + ' ';
	return ret;
}
#endif

void inclDirFlagsToString() { inclDirFlags = argsToString(inclDirs); }
#ifndef _MSC_VER
void libDirFlagsToString() { libDirFlags = argsToString(libDirs); }
#else
void libDirFlagsToString() { libDirFlags = convertLibDirs(); }
#endif
void objsToString() { objs = argsToString(linkObjs); }
void libsToString() { libs = argsToString(linkLibs) + argsToString(linkArgs); }

#ifndef _MSC_VER
string standardVersion(constParsedArg_t version)
{
	if (!version)
		return "-std=c++11"s;
	const auto str = version->value.data() + 5;
	if (strlen(str) != 5 || strncmp(str, "c++", 3) != 0 || str[3] == '8' || str[3] == '9')
	{
		testPrintf("Warning, standard version must be at least C++11");
		return "-std=c++11"s;
	}
	return version->value;
}

void buildCXXString()
{
	const auto standard = findArg(parsedArgs, "-std=", nullptr);
	cxxCompiler += standardVersion(standard) + ' ';
}
#else
int32_t compileMSVC(const string &test)
{
	const bool mode = isCXX(test);
	auto soFile = computeSOName(test);
	auto objFile = computeObjName(test);
	auto compileString = format("cl %s " COMPILE_OPTS " " LINK_OPTS ""s, test,
		inclDirFlags, objs, soFile, objFile, libDirFlags, mode ? "++" : "", libs);
	if (!silent)
	{
		if (quiet)
		{
			auto displayString = format(" CCLD  %s => %s"s, test, soFile);
			puts(displayString.get());
		}
		else
			puts(compileString.get());
	}
	return system(compileString.get());
}
#endif

void handleSanitizers()
{
	const auto sanitizer = findArg(parsedArgs, "-fsanitize=", nullptr);
	if (!sanitizer)
		return;
	cCompiler += sanitizer->value + ' ';
	cxxCompiler += sanitizer->value + ' ';
}

int compileTests()
{
	int32_t ret = 0;
	inclDirFlagsToString();
	libDirFlagsToString();
	objsToString();
	libsToString();
#ifndef _MSC_VER
	buildCXXString();
#endif
	testLog *logFile = nullptr;
	const auto logParam = findArg(parsedArgs, "--log", nullptr);
	const bool logging = bool(logParam);
	if (logging)
		logFile = startLogging(logParam->params[0].data());
	if (!silent)
		silent = bool(findArg(parsedArgs, "-s", nullptr));
	if (!quiet)
		quiet = bool(findArg(parsedArgs, "-q", nullptr));

	handleSanitizers();

	for (const auto &test : tests)
	{
		if (access(test.data(), R_OK) == 0 && validExt(test))
		{
#ifndef _MSC_VER
			ret = compileTest(test);
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
	if (parsedArgs.empty() || !getTests())
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
	debugBuild = bool(findArg(parsedArgs, "--debug", nullptr));
	return compileTests();
}
