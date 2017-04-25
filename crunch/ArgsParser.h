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

typedef struct _arg
{
	const char *const value;
	const uint32_t numMinParams;
	const uint32_t numMaxParams;
	const uint8_t flags;
} arg;

typedef struct _parsedArg
{
	const char *value;
	uint32_t paramsFound;
	const char **params;
	uint8_t flags;
} parsedArg;

#define ARG_REPEATABLE	1
#define ARG_INCOMPLETE	2

CRUNCH_API void registerArgs(const arg *allowedArgs);
CRUNCH_API parsedArg **parseArguments(const uint32_t argc, const char *const *const argv);
CRUNCH_API const parsedArg *findArg(const parsedArg *const *const args, const char *const value, const parsedArg *const defaultVal);
CRUNCH_API const arg *findArgInArgs(const char *const value);

parsedArg *checkAlreadyFound(const parsedArg *const *const parsedArgs, const parsedArg *const toCheck);
uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos, const arg *const argument, const arg *const args);

#endif /* __ARGSPARSER_H__ */
