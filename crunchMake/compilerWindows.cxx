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
	static const auto includeOptsExtra{"/I" crunch_PREFIX "/include "s}; // NOLINT(cert-err58-cpp)
	static const auto linkOptsExtra{"-libpath:" crunch_LIBDIR " "s}; // NOLINT(cert-err58-cpp)
#else
	static const auto includeOptsExtra{""s}; // NOLINT(cert-err58-cpp)
	static const auto linkOptsExtra{""s}; // NOLINT(cert-err58-cpp)
#endif

	// NOLINTNEXTLINE(cert-err58-cpp)
	static const auto compileOpts{"/permissive- /Zc:__cplusplus /Gd /GF /GS /Gy /EHsc /GT /D_WINDOWS /nologo "s};

	// NOLINTNEXTLINE(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
	std::string cCompiler{"cl "s};
	// NOLINTNEXTLINE(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
	std::string cxxCompiler{"cl "s};
	const std::string libExt{".dll"s}; // NOLINT(cert-err58-cpp)

	inline std::string crunchLib(const bool isCXX)
	{
		if (isCXX)
			return "libcrunch++.lib "s;
		else
			return "libcrunch.lib "s;
	}

	inline std::string debugCompileFlags()
	{
		if (debugBuild)
			return "/Oi /D_DEBUG /MDd "s;
			//" /Zi /FS"
		else
			return "/Ox /Ob2 /Oi /Oy /GL /MD "s;
	}

	inline std::string debugLinkFlags()
	{
		if (debugBuild)
			return "/LDd /link /DEBUG "s;
		else
			return "/LD /link "s;
	}

	std::string standardVersion(constParsedArg_t version)
	{
		if (!version)
			return "-std:c++14"_sv.toString();
		const auto *const str = version->value.data() + 5;
		if (strlen(str) != 5 || strncmp(str, "c++", 3) != 0 || str[3] == '8' || str[3] == '9' ||
			(str[3] == '1' && str[4] == '1'))
		{
			testPrintf("Warning, standard version must be at least C++14\n");
			return "-std:c++14"_sv.toString();
		}
		std::string standardStr{version->value.toString()};
		standardStr[4] = ':';
		return standardStr;
	}

	int32_t compileTest(const std::string &test)
	{
		const bool mode{isCXX(test)};
		const auto &compiler{mode ? cxxCompiler : cCompiler};
		const auto soFile{computeSOName(test)};
		const auto objFile{computeObjName(test)};
		const auto compileString{compiler + test + " "s + compileOpts + debugCompileFlags() +
			inclDirFlags + includeOptsExtra + objs + "/Fe"s + soFile + " /Fo"s + objFile +
			" "s + debugLinkFlags() + linkOptsExtra + libDirFlags + crunchLib(mode) + libs};
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
} // namespace crunch
