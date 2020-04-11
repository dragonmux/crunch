#ifndef CRUNCH_COMPILER__HXX
#define CRUNCH_COMPILER__HXX

#include <cstdint>
#include <string>

extern std::string inclDirFlags, libDirFlags, objs, libs;
extern bool silent, quiet, pthread, codeCoverage, debugBuild;

extern bool isCXX(const std::string &file);
extern std::string computeObjName(const std::string &file);
extern std::string computeSOName(const std::string &file);

extern int32_t compileTest(const std::string &test);

#endif /*CRUNCH_COMPILER__HXX*/
