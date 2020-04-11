#include "crunchCompiler.hxx"
#include <crunchMake.h>

using std::literals::string_literals::operator ""s;

#ifdef _DEBUG
static const auto compileOptsExtra{"/Oi /D_DEBUG "s};
//" /Zi /FS"
static const auto linkOptsExtra{"/LDd /link /DEBUG "s};
#else
static const auto compileOptsExtra{"/Ox /Ob2 /Oi /Oy /GL "s};
static const auto linkOptsExtra{"/LD /link "s};
#endif
static const auto compileOpts{"/Gd /GF /GS /Gy /EHsc /GT /D_WINDOWS /nologo "s};
const std::string libExt{".tlib"s};

inline const std::string crunchLib(const bool isCXX)
{
	if (isCXX)
		return "libcrunch++.lib "s;
	else
		return "libcrunch.lib "s;
}

int32_t compileTest(const std::string &test)
{
	const bool mode{isCXX(test)};
	const auto soFile{computeSOName(test)};
	const auto objFile{computeObjName(test)};
	const auto compileString{"cl "s + test + " "s + compileOptsExtra + compileOpts +
		inclDirFlags + objs + "/Fe"s + soFile + " /Fo"s + objFile + " "s +
		linkOptsExtra + libDirFlags + crunchLib(mode) + libs;
	if (!silent)
	{
		if (quiet)
		{
			const auto displayString{" CCLD  "s + test + " => "s + soFile;
			puts(displayString.c_str());
		}
		else
			puts(compileString.c_str());
	}
	return system(compileString.c_str());
}
