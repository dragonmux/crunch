// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef CRUNCH_COMPILER__HXX
#define CRUNCH_COMPILER__HXX

#include <cstdint>
#include <string>
#include <crunch++.h>
#include <argsParser.hxx>

namespace crunch
{
	extern std::string inclDirFlags, libDirFlags, objs, libs;
	extern bool silent, quiet, pthread, codeCoverage, debugBuild;

#ifndef _MSC_VER
	extern std::string cCompiler;
	extern std::string cxxCompiler;
#endif

	extern const std::string libExt;

	extern bool isCXX(const internal::stringView &file);
	extern std::string computeObjName(const std::string &file);
	extern std::string computeSOName(const std::string &file);

	extern std::string standardVersion(constParsedArg_t version);
	extern int32_t compileTest(const std::string &test);
}

#endif /*CRUNCH_COMPILER__HXX*/
