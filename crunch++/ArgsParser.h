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

#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <stdint.h>
#include <memory>
#include "crunch++.h"

struct arg_t final
{
	const char *value;
	const uint32_t numMinParams;
	const uint32_t numMaxParams;
	const uint8_t flags;
};

struct parsedArg_t
{
	using strPtr_t = std::unique_ptr<const char []>;

	strPtr_t value;
	uint32_t paramsFound;
	std::unique_ptr<strPtr_t []> params;
	uint8_t flags;
};

using constParsedArg_t = const parsedArg_t *;
using parsedArgs_t = std::unique_ptr<constParsedArg_t []>;

#define ARG_REPEATABLE	1
#define ARG_INCOMPLETE	2

CRUNCHpp_API void registerArgs(const arg_t *allowedArgs) noexcept;
CRUNCHpp_API parsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv) noexcept;
CRUNCHpp_API constParsedArg_t findArg(constParsedArg_t *const args, const char *const value, const constParsedArg_t defaultVal);
inline constParsedArg_t findArg(const parsedArgs_t &args, const char *const value, const constParsedArg_t defaultVal)
	{ return findArg(args.get(), value, defaultVal); }
CRUNCHpp_API const arg_t *findArgInArgs(const char *const value);

bool checkAlreadyFound(const parsedArgs_t &parsedArgs, const parsedArg_t &toCheck) noexcept;
uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos, const arg_t &argument, const arg_t *const args) noexcept;

#endif /* ARGS_PARSER_H */
