// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstdint>
#include <string>
#include "crunchCompiler.hxx"
#include "crunchMake.h"
#include "crunch++.h"
#include "logger.hxx"

namespace crunch
{
	using namespace std::literals::string_literals;

#ifdef crunch_PREFIX
	static const auto includeOptsExtra{"-I"s + crunch_PREFIX "/include "s}; // NOLINT(cert-err58-cpp)
#if defined(__MINGW32__) && defined(__clang__)
	static const auto linkOptsExtra{"-L"s + crunch_LIBDIR + " "s}; // NOLINT(cert-err58-cpp)
#else
	static const auto linkOptsExtra{"-L"s + crunch_LIBDIR " -Wl,-rpath,"s + crunch_LIBDIR " "s}; // NOLINT(cert-err58-cpp)
#endif
#else
	static const auto includeOptsExtra{""s}; // NOLINT(cert-err58-cpp)
	static const auto linkOptsExtra{""s}; // NOLINT(cert-err58-cpp)
#endif

	std::string cCompiler{compilerCC " "s}; // NOLINT(cert-err58-cpp)
// Workaround namespace'd extern variables being deduplicated incorrectly in macOS
#ifndef __APPLE__
	std::string cxxCompiler{compilerCXX + " -fvisibility=hidden -fvisibility-inlines-hidden "s}; // NOLINT(cert-err58-cpp)
#else
	std::string cxxCompiler{compilerCXX + " -fvisibility-inlines-hidden "s}; // NOLINT(cert-err58-cpp)
#endif
#ifndef _WIN32
	const std::string libExt{".so"s}; // NOLINT(cert-err58-cpp)
#else
	const std::string libExt{".dll"s}; //NOLINT(cert-err58-cpp)
#endif

	inline std::string crunchLib(const bool isCXX)
	{
		if (isCXX)
			return "-lcrunch++ "s;
		else
			return "-lcrunch "s;
	}

	inline std::string debugFlags() { return debugBuild ? "-O0 -g "s : "-O2 "s; }
	inline std::string threadingFlags() { return pthread ? ""s : "-pthread "s; }

	std::string standardVersion(constParsedArg_t version)
	{
		if (!version)
			return "-std=c++11"_sv.toString();
		const auto *const str = version->value.data() + 5;
		if (strlen(str) != 5 || strncmp(str, "c++", 3) != 0 || str[3] == '8' || str[3] == '9')
		{
			testPrintf("Warning, standard version must be at least C++11\n");
			return "-std=c++11"_sv.toString();
		}
		std::string standardStr{version->value.toString()};
		standardStr[4] = '=';
		return standardStr;
	}

	void libDirFlagsToString(const std::vector<internal::stringView> &libDirs)
		{ libDirFlags = argsToString(libDirs); }
	std::string linkLibsToString(const std::vector<internal::stringView> &linkLibs)
		{ return argsToString(linkLibs); }

#if compilerIsClang
	inline std::string coverageFlags() { return codeCoverage ? "--coverage "s : ""s; }

	int32_t compileTest(const std::string &test)
	{
		const bool mode{isCXX(test)};
		const auto &compiler{mode ? cxxCompiler : cCompiler};
		const auto objFile{computeObjName(test)};
		const auto compileString{compiler + test + " -c "s + includeOptsExtra +
			inclDirFlags + debugFlags() + threadingFlags() + "-o "s + objFile};
		if (!silent)
		{
			if (quiet)
			{
				const auto displayString{" CC    "s + test + " => "s + objFile};
				puts(displayString.c_str());
			}
			else
				puts(compileString.c_str());
		}
		const int32_t result{system(compileString.c_str())};
		if (result)
			return result;

		const auto soFile{computeSOName(test)};
		const auto linkString{compiler + objFile + " -shared "s + linkOptsExtra +
			libDirFlags + objs + libs + coverageFlags() + crunchLib(mode) + debugFlags() +
			threadingFlags() + "-o " + soFile};
		if (!silent)
		{
			if (quiet)
			{
				const auto displayString{" CCLD  "s + objFile + " => "s + soFile};
				puts(displayString.c_str());
			}
			else
				puts(linkString.c_str());
		}
		return system(linkString.c_str());
	}
#else
	inline std::string coverageFlags() { return codeCoverage ? "-lgcov "s : ""s; }

	int32_t compileTest(const std::string &test)
	{
		const bool mode{isCXX(test)};
		const auto &compiler{mode ? cxxCompiler : cCompiler};
		const auto soFile{computeSOName(test)};
		const auto compileString{compiler + test + " -shared "s + includeOptsExtra +
			linkOptsExtra + inclDirFlags + libDirFlags + objs + libs + coverageFlags() +
			crunchLib(mode) + debugFlags() + threadingFlags() + "-o "s + soFile};
		if (!silent)
		{
			if (quiet)
			{
				const auto displayString{" CCLD  "s + test + " => "s + soFile};
				puts(displayString.c_str());
			}
			else
				puts(compileString.c_str());
		}
		return system(compileString.c_str());
	}
#endif
} // namespace crunch
