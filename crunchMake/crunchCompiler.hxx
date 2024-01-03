// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef CRUNCH_COMPILER__HXX
#define CRUNCH_COMPILER__HXX

#include <cstdint>
#include <string>
#include <vector>
#include <crunch++.h>
#include <argsParser.hxx>

namespace crunch
{
	extern std::string inclDirFlags, libDirFlags, objs, libs;
	extern bool silent, quiet, pthread, codeCoverage, debugBuild;

	extern std::string cCompiler;
	extern std::string cxxCompiler;

	extern const std::string libExt;

	bool isCXX(const internal::stringView &file);
	std::string computeObjName(const std::string &file);
	std::string computeSOName(const std::string &file);

	std::string standardVersion(constParsedArg_t version);
	int32_t compileTest(const std::string &test);

	std::string argsToString(const std::vector<internal::stringView> &var);

	void libDirFlagsToString(const std::vector<internal::stringView> &libDirs);
	std::string linkLibsToString(const std::vector<internal::stringView> &linkLibs);
}

#endif /*CRUNCH_COMPILER__HXX*/
