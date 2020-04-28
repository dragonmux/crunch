// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstring>
#include <cstdlib>
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
#include <numeric>
#include <substrate/utility>
#include <core.hxx>
#include <logger.hxx>
#include <argsParser.hxx>
#include <stringFuncs.hxx>
#include "crunchCompiler.hxx"
#include <crunchMake.h>

parsedArgs_t parsedArgs;
std::vector<std::string> inclDirs, libDirs;
std::vector<std::string> linkLibs, linkObjs;
std::vector<std::string> linkArgs, tests;
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
const char *forward_(const std::string &value) noexcept { return value.data(); }
const char *forward_(const std::unique_ptr<char []> &value) noexcept { return value.get(); }
const char *forward_(const std::unique_ptr<const char []> &value) noexcept { return value.get(); }
template<typename... values_t> inline std::unique_ptr<char []> format(const std::string &format,
	values_t &&... values) noexcept { return formatString(format.data(), forward_(values)...); }

const auto exts{substrate::make_array<const char *>({".c", ".cpp", ".cc", ".cxx", ".i", ".s", ".S", ".sx"})};
const auto cxxExts{substrate::make_array<const char *>({".cpp", ".cc", ".cxx"})};
const auto objExts{substrate::make_array<const char *>({".o", ".obj", ".a"})};

std::string inclDirFlags{}, libDirFlags{}, objs{}, libs{};
bool silent, quiet, pthread, codeCoverage, debugBuild;

const auto args{substrate::make_array<arg_t>( // NOLINT(cert-err58-cpp)
{
	{"-l"_s, 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-I"_s, 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-D"_s, 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-L"_s, 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-o"_s, 1, 1, 0},
	{"--log"_s, 1, 1, 0},
	{"--silent"_s, 0, 0, 0},
	{"-s"_s, 0, 0, 0},
	{"--quiet"_s, 0, 0, 0},
	{"-q"_s, 0, 0, 0},
	{"-pthread"_s, 0, 0, 0},
	{"-Wl"_s, 0, 0, ARG_INCOMPLETE},
	{"-std="_s, 0, 0, ARG_INCOMPLETE},
	{"-z"_s, 1, 1, ARG_REPEATABLE},
	{"--coverage"_s, 0, 0, 0},
	{"--debug"_s, 0, 0, 0},
	{"-fsanitize="_s, 0, 0, ARG_INCOMPLETE},
	{{}, 0, 0, 0}
})};

bool isObj(const char *file)
{
	const auto *const dot{std::strrchr(file, '.')};
	if (!dot)
		return false;
	for (auto &ext : objExts)
		if (std::strcmp(dot, ext) == 0)
			return true;
	return false;
}
bool isObj(const std::string &file)
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

inline void getLinkFunc(std::vector<std::string> &var, const char *find)
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

inline std::string argToString(const parsedArg_t &arg)
{
	auto result{arg.value + ' '};
	for (uint32_t i = 0; i < arg.paramsFound; ++i)
		result += arg.params[i] + ' ';
	return result;
}

void getLinkArgs()
{
	for (const auto &parsedArg : parsedArgs)
	{
		if (parsedArg.matches("-z"))
			linkArgs.emplace_back(argToString(parsedArg));
	}
}

bool validExt(const char *const file)
{
	const auto *const dot{std::strrchr(file, '.')};
	if (dot == nullptr)
		return false;
	for (auto &ext : exts)
		if (std::strcmp(dot, ext) == 0)
			return true;
	return false;
}
bool validExt(const std::string &file)
	{ return validExt(file.data()); }

bool isCXX(const char *const file)
{
	const auto *const dot{std::strrchr(file, '.')};
	if (!dot)
		return false;
	for (auto &ext : cxxExts)
		if (std::strcmp(dot, ext) == 0)
			return true;
	return false;
}
bool isCXX(const std::string &file)
	{ return isCXX(file.data()); }

std::string toO(const std::string &file)
{
	const auto dotPos{file.find_last_of('.')};
	auto objFile{file.substr(0, dotPos)};
#ifndef _MSC_VER
	return objFile += ".o"s;
#else
	return objFile += ".obj"s;
#endif
}

std::string toO(const std::unique_ptr<const char []> &file)
	{ return toO(file.get()); }

std::string computeObjName(const std::string &file)
{
	const auto output{findArg(parsedArgs, "-o", nullptr)};
	if (output)
		return toO(output->params[0]);
	return toO(file);
}

std::string toSO(const std::string &file)
{
	const auto dotPos{file.find_last_of('.')};
	auto soFile{file.substr(0, dotPos)};
	return soFile += libExt;
}

std::string computeSOName(const std::string &file)
{
	const auto output{findArg(parsedArgs, "-o", nullptr)};
	if (output)
		return output->params[0];
	return toSO(file);
}

inline std::string argsToString(const std::vector<std::string> &var)
{
	return std::accumulate(var.begin(), var.end(), std::string{},
		[](const std::string &result, const std::string &value)
			{ return result + value + ' '; }
	);
}

#ifdef _MSC_VER
string convertLibDirs()
{
	std::string ret{};
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
std::string standardVersion(constParsedArg_t version)
{
	if (!version)
		return "-std=c++11"s;
	const auto *const str = version->value.data() + 5;
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
int32_t compileMSVC(const std::string &test)
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
	const auto logging = bool(logParam);
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
	registerArgs(args.data());
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
