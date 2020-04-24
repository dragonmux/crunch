// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef ARGSPARSER__H
#define ARGSPARSER__H

#include <stdint.h>
#include <stdbool.h>
#include "crunch.h"

typedef struct arg_t
{
	const char *const value;
	const uint32_t numMinParams;
	const uint32_t numMaxParams;
	const uint8_t flags;
} arg_t;

typedef struct parsedArg_t
{
	const char *value;
	uint32_t paramsFound;
	const char **params;
	uint8_t flags;
} parsedArg_t;

typedef const parsedArg_t *constParsedArg_t;
typedef constParsedArg_t *parsedArgs_t;
typedef const constParsedArg_t *constParsedArgs_t;

#define ARG_REPEATABLE	1U
#define ARG_INCOMPLETE	2U

CRUNCH_API void registerArgs(const arg_t *allowedArgs);
CRUNCH_API constParsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv);
CRUNCH_API constParsedArg_t findArg(const constParsedArg_t *const args, const char *const value,
	const constParsedArg_t defaultVal);
CRUNCH_API const arg_t *findArgInArgs(const char *const value);
CRUNCH_API bool freeParsedArg(const parsedArg_t *parsedArg);
CRUNCH_API void *freeParsedArgs(constParsedArgs_t parsedArgs);

CRUNCH_VIS bool checkAlreadyFound(const constParsedArgs_t parsedArgs, const parsedArg_t *const toCheck);
CRUNCH_VIS uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos, const arg_t *const argument, const arg_t *const args);

#endif /*ARGSPARSER__H*/
