#ifndef __ARGSPARSER_H__
#define __ARGSPARSER_H__

#include <inttypes.h>

typedef struct _arg
{
	const char *value;
	uint32_t numParams;
	uint32_t flags;
} arg;

typedef struct _parsedArg
{
	const char *value;
	uint32_t paramsFound;
	const char **params;
} parsedArg;

#define ARG_REQUIRED		1

extern parsedArg **parseArguments(int argc, char **argv);
extern parsedArg *findArg(parsedArg **args, const char *value, parsedArg *defaultVal);

#endif /* __ARGSPARSER_H__ */
