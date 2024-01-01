// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstring>
#include <cstdlib>
#if !defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
#include <unistd.h>
#else
#include <io.h>
constexpr static const auto R_OK{0x04};
#endif
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <substrate/utility>
#include "core.hxx"
#include "logger.hxx"
#include "stringFuncs.hxx"
#include "crunchCompiler.hxx"
#include "crunchMake.h"
#include "version.hxx"

namespace crunch
{
	parsedArgs_t parsedArgs;
	std::vector<internal::stringView> inclDirs, libDirs;
	std::vector<internal::stringView> linkLibs, linkObjs;
	std::vector<std::string> linkArgs;
	std::vector<internal::stringView> tests;
	uint32_t numLinkArgs = 0;

	constexpr static auto exts{substrate::make_array<internal::stringView>({
		".c"_sv, ".cpp"_sv, ".cc"_sv, ".cxx"_sv, ".i"_sv, ".s"_sv, ".S"_sv, ".sx"_sv, ".asm"_sv
	})};
	constexpr static auto cxxExts{substrate::make_array<internal::stringView>({".cpp"_sv, ".cc"_sv, ".cxx"_sv})};
	constexpr static auto objExts{substrate::make_array<internal::stringView>({
		".o"_sv, ".obj"_sv, ".a"_sv, ".so"_sv, ".dll"_sv, ".dylib"_sv
	})};

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
		{"-Wl,"_sv, 0, 0, ARG_INCOMPLETE},
		{"-std="_sv, 0, 0, ARG_INCOMPLETE},
		{"-std:"_sv, 0, 0, ARG_INCOMPLETE},
		{"-z"_sv, 1, 1, ARG_REPEATABLE},
		{"--coverage"_sv, 0, 0, 0},
		{"--debug"_sv, 0, 0, 0},
		{"-fsanitize="_sv, 0, 0, ARG_INCOMPLETE},
		{"-flto"_sv, 0, 0, 0},
		{{}, 0, 0, 0}
	})};

	template<size_t N> constexpr bool checkExt(const internal::stringView &file,
		const std::array<internal::stringView, N> &exts) noexcept
	{
		constexpr auto npos{internal::stringView::npos};
		const auto dot{file.rfind('.')};
		if (dot == npos)
			return false;
		return std::find_if(exts.begin(), exts.end(),
			[=](const internal::stringView ext) { return file.compare(dot, npos, ext) == 0; }
		) != exts.end();
	}

	constexpr bool validExt(const internal::stringView &file)
		{ return checkExt(file, exts); }
	bool isCXX(const internal::stringView &file)
		{ return checkExt(file, cxxExts); }

	bool isObj(const internal::stringView &file)
	{
		// Try the normal extension check first
		if (checkExt(file, objExts))
			return true;
		// Now also check for .so files that have a version after the `.so`
		const auto forwardSlash{file.rfind('/')};
		const auto backwardSlash{file.rfind('\\')};
		// Figure out wehre to substring the file name and search forward for dots after that
		const auto begin
		{
			[&]() -> size_t
			{
				// If we found a '/', use its position + 1 as the starting position for the next operation
				if (forwardSlash != internal::stringView::npos)
					return forwardSlash + 1U;
				// Similarly for '\\' to accomodate Windows
				if (backwardSlash != internal::stringView::npos)
					return backwardSlash + 1U;
				// Otherwise use the start of the file name as it contains no path delimeters
				return 0U;
			}()
		};
		for (size_t offset = begin; offset != internal::stringView::npos; )
		{
			// Find the next '.'
			const auto dot{file.find('.', offset)};
			// Compare if we've found `.so.`
			if (dot != internal::stringView::npos && file.compare(dot, 4U, ".so."_sv) == 0)
				return true;
			// If we did not, then update the offset to just after the new dot position
			offset = dot == internal::stringView::npos ? dot : dot + 1U;
		}
		// If we did not find an instance of `.so.`, or anything from objExts: it's not an object file
		return false;
	}

	bool getTests()
	{
		for (const auto &parsedArg : parsedArgs)
		{
			const auto &value = parsedArg.value;
			if (!findArgInArgs(value) && !isObj(value))
				tests.emplace_back(value);
		}
		return !tests.empty();
	}

	inline void getLinkFunc(std::vector<internal::stringView> &var, const char *find)
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
			if (!findArgInArgs(value) && isObj(value))
			{
#ifdef _MSC_VER
				// If we're on a compiler targeting `link.exe` (or the Clang-cl equiv),
				// we have to filter out any .dll files that wound up on the command line
				// as link cannot consume them and will error.
				const auto dot{value.rfind('.')};
				if (dot != internal::stringView::npos &&
					file.compare(dot, internal::stringView::npos, ".dll"_sv) == 0)
					continue;
#endif
				linkObjs.emplace_back(value);
			}
		}
	}

	inline std::string argToString(const parsedArg_t &arg)
	{
		auto result{arg.value.toString() + ' '};
		for (uint32_t i = 0; i < arg.paramsFound; ++i)
			result += arg.params[i] + ' ';
		return result;
	}

	void getLinkArgs()
	{
		for (const auto &parsedArg : parsedArgs)
		{
			if (parsedArg.matches("-z"_sv) || parsedArg.matches("-Wl,"_sv) ||
				parsedArg.matches("-flto"_sv))
				linkArgs.emplace_back(argToString(parsedArg));
		}
	}

	std::string toO(const std::string &file)
	{
		const auto dotPos{file.find_last_of('.')};
		auto objFile{file.substr(0, dotPos)};
#ifndef _WIN32
		return objFile += ".o"s;
#else
		return objFile += ".obj"s;
#endif
	}

	std::string computeObjName(const std::string &file)
	{
		const auto *const output{findArg(parsedArgs, "-o"_sv, nullptr)};
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
		const auto *const output{findArg(parsedArgs, "-o"_sv, nullptr)};
		if (output)
			return output->params[0];
		return toSO(file);
	}

	inline std::string argsToString(const std::vector<internal::stringView> &var)
	{
		return std::accumulate(var.begin(), var.end(), std::string{},
			[](const std::string &result, const internal::stringView &value)
				{ return result + value.toString() + ' '; }
		);
	}

	inline std::string argsToString(const std::vector<std::string> &var)
	{
		return std::accumulate(var.begin(), var.end(), std::string{},
			[](const std::string &result, const std::string &value)
				{ return result + value + ' '; }
		);
	}

#ifdef _MSC_VER
	inline std::string convertLibDirs()
	{
		std::string ret{};
		for (const auto &value : libDirs)
			ret += "-libpath:" + value.substr(2).toString() + ' ';
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

	const parsedArg_t *fetchStandard()
	{
		const auto *const standardEquals{findArg(parsedArgs, "-std="_sv, nullptr)};
		const auto *const standardColon{findArg(parsedArgs, "-std:"_sv, nullptr)};
		if (standardEquals && standardColon)
			testPrintf("Warning, both forms of -std specified, using -std= form\n");
		return standardEquals ? standardEquals : standardColon;
	}

	void buildCXXString()
	{
		const auto *const standard{fetchStandard()};
		cxxCompiler += standardVersion(standard) + ' ';
	}

#ifndef _WIN32
	void handleSanitizers()
	{
		const auto *const sanitizer{findArg(parsedArgs, "-fsanitize="_sv, nullptr)};
		if (!sanitizer)
			return;
		const internal::stringView sanitizers{sanitizer->value.data() + 11, sanitizer->value.length() - 11};
		for (size_t offset{}; offset < sanitizers.length();)
		{
			const auto length{[](const internal::stringView &value, const size_t offset) noexcept
			{
				const auto result{value.find(',', offset)};
				return (result == internal::stringView::npos ? value.length() : result) - offset;
			}(sanitizers, offset)};
			const auto option{"-fsanitize="_s + sanitizers.substr(offset, length).toString() + ' '};
			cCompiler += option;
			cxxCompiler += option;
			offset += length + 1;
		}
	}
#endif

	int compileTests()
	{
		int32_t ret = 0;
		inclDirFlagsToString();
		libDirFlagsToString();
		objsToString();
		libsToString();
		buildCXXString();
		testLog *logFile = nullptr;
		const auto *const logParam{findArg(parsedArgs, "--log"_sv, nullptr)};
		const auto logging = bool(logParam);
		if (logging)
			logFile = startLogging(logParam->params[0].data());
		if (!silent)
			silent = bool(findArg(parsedArgs, "-s"_sv, nullptr));
		if (!quiet)
			quiet = bool(findArg(parsedArgs, "-q"_sv, nullptr));

#ifndef _WIN32
		handleSanitizers();
#endif

		for (const auto &test : tests)
		{
			if (access(test.data(), R_OK) == 0 && validExt(test))
			{
				ret = compileTest(test.toString());
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
		constParsedArg_t version{findArg(parsedArgs, "--version"_sv, nullptr)};
		constParsedArg_t versionShort{findArg(parsedArgs, "-v"_sv, nullptr)};
		constParsedArg_t help{findArg(parsedArgs, "--help"_sv, nullptr)};
		constParsedArg_t helpShort{findArg(parsedArgs, "-h"_sv, nullptr)};

		if (help || helpShort)
			puts(crunchpp::help.data());
		else if (version || versionShort)
			testPrintf("crunchMake %s (%s %s %s-%s)\n", crunchpp::version.data(), crunchpp::compiler.data(),
				crunchpp::compilerVersion.data(), crunchpp::system.data(), crunchpp::arch.data());
		else
			return false;
		return true;
	}

	int32_t main(const int32_t argc, const char *const *const argv) try
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
		silent = bool(findArg(parsedArgs, "--silent"_sv, nullptr));
		quiet = bool(findArg(parsedArgs, "--quiet"_sv, nullptr));
		pthread = bool(findArg(parsedArgs, "-pthread"_sv, nullptr));
		codeCoverage = bool(findArg(parsedArgs, "--coverage"_sv, nullptr));
		debugBuild = bool(findArg(parsedArgs, "--debug"_sv, nullptr));
		return compileTests();
	}
	catch (const std::out_of_range &error)
	{
		testPrintf("Error during string splicing operations: %s\n", error.what());
		return 2;
	}
} // namespace crunch

int main(int argc, char **argv) { return crunch::main(argc, argv); }
