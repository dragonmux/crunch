/*
 * This file is part of crunch
 * Copyright © 2013-2017 Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * crunch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * crunch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ARGSPARSER_H__
#define __ARGSPARSER_H__

#include <stdint.h>
#include "crunch.h"

typedef struct _arg_t
{
	const char *const value;
	const uint32_t numMinParams;
	const uint32_t numMaxParams;
	const uint8_t flags;
} arg_t;

typedef struct _parsedArg_t
{
	const char *value;
	uint32_t paramsFound;
	const char **params;
	uint8_t flags;
} parsedArg_t;

typedef const parsedArg_t *constParsedArg_t;
typedef constParsedArg_t *parsedArgs_t;

#define ARG_REPEATABLE	1
#define ARG_INCOMPLETE	2

CRUNCH_API void registerArgs(const arg_t *allowedArgs);
CRUNCH_API parsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv);
CRUNCH_API constParsedArg_t findArg(constParsedArg_t *const args, const char *const value, const constParsedArg_t defaultVal);
CRUNCH_API const arg_t *findArgInArgs(const char *const value);
CRUNCH_API uint8_t freeParsedArg(parsedArg_t *parsedArg);
CRUNCH_API void *freeParsedArgs(parsedArgs_t parsedArgs);

CRUNCH_VIS uint8_t checkAlreadyFound(const parsedArgs_t parsedArgs, const parsedArg_t *const toCheck);
CRUNCH_VIS uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos, const arg_t *const argument, const arg_t *const args);

#endif /* __ARGSPARSER_H__ */
