// SPDX-License-Identifier: LGPL-3.0-or-later
#include "crunchCompiler.hxx"
#include <crunchMake.h>

using std::literals::string_literals::operator ""s;

#ifdef crunch_PREFIX
static const auto includeOptsExtra{"-I"s + crunch_PREFIX "/include "s}; // NOLINT(cert-err58-cpp)
static const auto linkOptsExtra{"-L"s + crunch_LIBDIR " -Wl,-rpath,"s + crunch_LIBDIR " "s}; // NOLINT(cert-err58-cpp)
#else
static const auto includeOptsExtra{""s}; // NOLINT(cert-err58-cpp)
static const auto linkOptsExtra{""s}; // NOLINT(cert-err58-cpp)
#endif

std::string cCompiler{compilerCC " "s};
std::string cxxCompiler{compilerCXX + " -fvisibility=hidden -fvisibility-inlines-hidden "s};
const std::string libExt{".so"s}; // NOLINT(cert-err58-cpp)

inline std::string crunchLib(const bool isCXX)
{
	if (isCXX)
		return "-lcrunch++ "s;
	else
		return "-lcrunch "s;
}

inline std::string coverageFlags() { return codeCoverage ? "-lgcov "s : ""s; }
inline std::string debugFlags() { return debugBuild ? "-O0 -g "s : "-O2 "s; }
inline std::string threadingFlags() { return pthread ? ""s : "-pthread "s; }

#if compilerIsClang
int32_t compileTest(const std::string &test)
{
	const bool mode{isCXX(test)};
	const std::string &compiler = mode ? cxxCompiler : cCompiler;
	const auto objFile{computeObjName(test)};
	const auto compileString{compiler + " "s + test + " -c "s + includeOptsExtra +
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
	const auto linkString{compiler + " "s + objFile + " -shared "s + linkOptsExtra +
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
int32_t compileTest(const std::string &test)
{
	const bool mode{isCXX(test)};
	const std::string &compiler = mode ? cxxCompiler : cCompiler;
	const auto soFile{computeSOName(test)};
	const auto compileString{compiler + " "s + test + " -shared "s + includeOptsExtra +
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
