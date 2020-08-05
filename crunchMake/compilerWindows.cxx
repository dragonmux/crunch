// SPDX-License-Identifier: LGPL-3.0-or-later
#include "crunchCompiler.hxx"
#include <crunchMake.h>
#include <logger.hxx>

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

#ifdef _DEBUG
	static const auto compileOptsExtra{"/Oi /D_DEBUG /MDd "s}; // NOLINT(cert-err58-cpp)
	//" /Zi /FS"
	static const auto linkOpts{"/LDd /link /DEBUG "s}; // NOLINT(cert-err58-cpp)
#else
	static const auto compileOptsExtra{"/Ox /Ob2 /Oi /Oy /GL /MD "s}; // NOLINT(cert-err58-cpp)
	static const auto linkOpts{"/LD /link "s}; // NOLINT(cert-err58-cpp)
#endif
	static const auto compileOpts{"/permissive- /Zc:__cplusplus /Gd /GF /GS /Gy /EHsc /GT /D_WINDOWS /nologo "s}; // NOLINT(cert-err58-cpp)

	std::string cCompiler{"cl "s};
	std::string cxxCompiler{"cl "s};
	const std::string libExt{".dll"s}; // NOLINT(cert-err58-cpp)

	inline std::string crunchLib(const bool isCXX)
	{
		if (isCXX)
			return "libcrunch++.lib "s;
		else
			return "libcrunch.lib "s;
	}

	std::string standardVersion(constParsedArg_t version)
	{
		if (!version)
			return "-std:c++11"_sv.toString();
		const auto *const str = version->value.data() + 5;
		if (strlen(str) != 5 || strncmp(str, "c++", 3) != 0 || str[3] == '8' || str[3] == '9')
		{
			testPrintf("Warning, standard version must be at least C++11\n");
			return "-std:c++11"_sv.toString();
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
		const auto compileString{compiler + test + " "s + compileOpts + compileOptsExtra +
			inclDirFlags + includeOptsExtra + objs + "/Fe"s + soFile + " /Fo"s + objFile +
			" "s + linkOpts + linkOptsExtra + libDirFlags + crunchLib(mode) + libs};
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
