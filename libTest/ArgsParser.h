#ifndef __ARGSPARSER_H__
#define __ARGSPARSER_H__

#include <inttypes.h>

typedef struct _arg
{
	const char *value;
	uint32_t numMinParams;
	uint32_t numMaxParams;
	uint8_t flags;
} arg;

typedef struct _parsedArg
{
	const char *value;
	uint32_t paramsFound;
	const char **params;
} parsedArg;

#define ARG_REPEATABLE	1
#define ARG_INCOMPLETE	2

extern parsedArg **parseArguments(int argc, char **argv);
extern parsedArg *findArg(parsedArg **args, const char *value, parsedArg *defaultVal);
extern arg *findArgInArgs(const char *value);

#endif /* __ARGSPARSER_H__ */
