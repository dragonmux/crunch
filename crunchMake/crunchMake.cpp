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
#include <algorithm>
#include <numeric>
#include <substrate/utility>
#include <core.hxx>
#include <logger.hxx>
#include <argsParser.hxx>
#include <stringFuncs.hxx>
#include "crunchCompiler.hxx"
#include <crunchMake.h>
#include <version.hxx>

namespace crunch
{
	using internal::stringView;

	parsedArgs_t parsedArgs;
	std::vector<std::string> inclDirs, libDirs;
	std::vector<std::string> linkLibs, linkObjs;
	std::vector<std::string> linkArgs;
	std::vector<internal::stringView> tests;
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
	const char *forward_(const std::unique_ptr<char []> &value) noexcept { return value.get(); } // NOLINT
	const char *forward_(const std::unique_ptr<const char []> &value) noexcept { return value.get(); } // NOLINT
	template<typename... values_t> inline std::unique_ptr<char []> format(const std::string &format, // NOLINT
		values_t &&... values) noexcept { return formatString(format.data(), forward_(values)...); }

	constexpr static auto exts{substrate::make_array<stringView>({
		".c"_sv, ".cpp"_sv, ".cc"_sv, ".cxx"_sv, ".i"_sv, ".s"_sv, ".S"_sv, ".sx"_sv, ".asm"_sv
	})};
	constexpr static auto cxxExts{substrate::make_array<stringView>({".cpp"_sv, ".cc"_sv, ".cxx"_sv})};
	constexpr static auto objExts{substrate::make_array<stringView>({".o"_sv, ".obj"_sv, ".a"_sv})};

	std::string inclDirFlags{}, libDirFlags{}, objs{}, libs{};
	bool silent, quiet, pthread, codeCoverage, debugBuild;

	constexpr static auto args{substrate::make_array<arg_t>(
	{
		{"--help"_sv, 0, 0, 0},
		{"-h"_sv, 0, 0, 0},
		{"--version"_sv, 0, 0, 0},
		{"-v"_sv, 0, 0, 0},
		{"-l"_sv, 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
		{"-I"_sv, 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
		{"-D"_sv, 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
		{"-L"_sv, 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
		{"-o"_sv, 1, 1, 0},
		{"--log"_sv, 1, 1, 0},
		{"--silent"_sv, 0, 0, 0},
		{"-s"_sv, 0, 0, 0},
		{"--quiet"_sv, 0, 0, 0},
		{"-q"_sv, 0, 0, 0},
		{"-pthread"_sv, 0, 0, 0},
		{"-Wl"_sv, 0, 0, ARG_INCOMPLETE},
		{"-std="_sv, 0, 0, ARG_INCOMPLETE},
		{"-z"_sv, 1, 1, ARG_REPEATABLE},
		{"--coverage"_sv, 0, 0, 0},
		{"--debug"_sv, 0, 0, 0},
		{"-fsanitize="_sv, 0, 0, ARG_INCOMPLETE},
		{{}, 0, 0, 0}
	})};

	template<size_t N> constexpr bool checkExt(const internal::stringView &file, const std::array<stringView, N> &exts) noexcept
	{
		constexpr auto npos{internal::stringView::npos};
		const auto dot{file.rfind('.')};
		if (dot == npos)
			return false;
		return std::find_if(exts.begin(), exts.end(),
			[=](const stringView ext) { return file.compare(dot, npos, ext) == 0; }
		) != exts.end();
	}

	constexpr bool validExt(const internal::stringView &file)
		{ return checkExt(file, exts); }
	bool isCXX(const internal::stringView &file)
		{ return checkExt(file, cxxExts); }
	constexpr bool isObj(const internal::stringView &file)
		{ return checkExt(file, objExts); }

	bool getTests()
	{
		for (const auto &parsedArg : parsedArgs)
		{
			const auto &value = parsedArg.value;
			if (!findArgInArgs(value.data()) && !isObj(value))
				tests.emplace_back(value);
		}
		return !tests.empty();
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

	std::string toO(const std::unique_ptr<const char []> &file) // NOLINT
		{ return toO(file.get()); }

	std::string computeObjName(const std::string &file)
	{
		const auto *const output{findArg(parsedArgs, "-o", nullptr)};
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
		const auto *const output{findArg(parsedArgs, "-o", nullptr)};
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
		const auto *const standard{findArg(parsedArgs, "-std=", nullptr)};
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
		const auto *const sanitizer{findArg(parsedArgs, "-fsanitize=", nullptr)};
		if (!sanitizer)
			return;
		const stringView sanitizers{sanitizer->value.data() + 11, sanitizer->value.length() - 11};
		for (size_t offset{}; offset < sanitizers.length();)
		{
			const auto length{[](const stringView &value, const size_t offset) noexcept
			{
				const auto result{value.find(',', offset)};
				return (result == stringView::npos ? value.length() : result) - offset;
			}(sanitizers, offset)};
			const auto option{"-fsanitize="_s + sanitizers.substr(offset, length) + ' '};
			cCompiler += option;
			cxxCompiler += option;
			offset += length + 1;
		}
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
		const auto *const logParam{findArg(parsedArgs, "--log", nullptr)};
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

	bool handleVersionOrHelp()
	{
		constParsedArg_t version{findArg(parsedArgs, "--version", nullptr)};
		constParsedArg_t versionShort{findArg(parsedArgs, "-v", nullptr)};
		constParsedArg_t help{findArg(parsedArgs, "--help", nullptr)};
		constParsedArg_t helpShort{findArg(parsedArgs, "-h", nullptr)};

		if (help || helpShort)
			puts(crunchpp::help.data());
		else if (version || versionShort)
			testPrintf("crunchMake %s (%s %s %s-%s)\n", crunchpp::version.data(), crunchpp::compiler.data(),
				crunchpp::compilerVersion.data(), crunchpp::system.data(), crunchpp::arch.data());
		else
			return false;
		return true;
	}

	int32_t main(const int32_t argc, const char *const *const argv)
	{
		registerArgs(args.data());
		parsedArgs = parseArguments(argc, argv);
		if (!parsedArgs.empty() && handleVersionOrHelp())
			return 0;
		else if (parsedArgs.empty() || !getTests())
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
} // namespace crunch

int main(int argc, char **argv) { return crunch::main(argc, argv); }
