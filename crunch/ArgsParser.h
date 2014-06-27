/*
 * This file is part of crunch
 * Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
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
	uint8_t flags;
} parsedArg;

#define ARG_REPEATABLE	1
#define ARG_INCOMPLETE	2

#ifdef _MSC_VER
extern void registerArgs(arg *allowedargs);
#endif
extern parsedArg **parseArguments(int argc, char **argv);
extern parsedArg *findArg(parsedArg **args, const char *value, parsedArg *defaultVal);
extern arg *findArgInArgs(const char *value);

#endif /* __ARGSPARSER_H__ */
