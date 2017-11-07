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

#include "crunch++.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Logger.h"
#include <stdlib.h>
#include <string.h>
#include <new>

const arg_t *args = nullptr;

void registerArgs(const arg_t *allowedArgs) noexcept
{
	args = allowedArgs;
#ifdef _MSC_VER
	stdout = __acrt_iob_func(1);
#endif
}

bool checkAlreadyFound(const parsedArgs_t &parsedArgs, const parsedArg_t &toCheck) noexcept
{
	for (uint32_t i = 0; parsedArgs[i] != nullptr; ++i)
	{
		if (strcmp(parsedArgs[i]->value.get(), toCheck.value.get()) == 0)
			return true;
	}
	return false;
}

uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos, const arg_t &argument, const arg_t *const args) noexcept
{
	uint32_t n = 0;
	const uint32_t min = argument.numMinParams, max = argument.numMaxParams;
	bool eoa = false;
	for (uint32_t i = argPos; i < argc && n < max && !eoa; ++i)
	{
		const arg_t *currArg = args;
		while (currArg->value != nullptr && !eoa)
		{
			if (strcmp(currArg->value, argv[i]) == 0)
				eoa = true;
			++currArg;
		}
		if (eoa || ++n == max)
			break;
	}
	if (n < min)
		return -1;
	return n;
}

parsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv) noexcept
{
	if (argc < 1 || (argc >> 31) == 1 || !argv || !args)
		return nullptr;

	auto ret(makeUnique<constParsedArg_t []>(argc));
	if (!ret)
		return nullptr;
	uint32_t n = 0;
	for (uint32_t i = 1; i < argc; ++i)
	{
		bool found = false, skip = false;
		const arg_t *argument = args;
		auto argRet(makeUnique<parsedArg_t>());
		if (!argRet)
			return nullptr;
		while (argument->value != nullptr)
		{
			found = strcmp(argument->value, argv[i]) == 0 || ((argument->flags & ARG_INCOMPLETE) &&
				strncmp(argument->value, argv[i], strlen(argument->value)) == 0);
			if (found)
			{
				argRet->value = strNewDup(argv[i]);
				if (!(argument->flags & ARG_REPEATABLE) && checkAlreadyFound(ret, *argRet))
				{
					printf("Duplicate argument found: %s\n", argRet->value.get());
					skip = true;
					break;
				}
				argRet->paramsFound = checkParams(argc, argv, i + 1, *argument, args);
				if (argRet->paramsFound == uint32_t(-1))
				{
					printf("Not enough parameters given for argument %s\n", argv[i]);
					return nullptr;
				}
				// Only allocate for the params if there are any found, otherwise let the pointer dwell as nullptr.
				if (argRet->paramsFound)
				{
					argRet->params = makeUnique<parsedArg_t::strPtr_t []>(argRet->paramsFound);
					if (!argRet->params)
						return nullptr;
					for (uint32_t j = 0; j < argRet->paramsFound; ++j)
					{
						argRet->params[j] = strNewDup(argv[i + j + 1]);
						if (!argRet->params[j])
							return nullptr;
					}
				}
				i += argRet->paramsFound;
				argRet->flags = argument->flags;
				break;
			}
			else if ((argument->flags & ARG_INCOMPLETE) && strncmp(argument->value, argv[i], strlen(argument->value)) == 0)
			{
				printf("Badly formatted argument (%s)\n", argv[i]);
				return nullptr;
			}
			++argument;
		}
		if (skip)
			continue;
		else if (!found)
		{
			argRet->value = strNewDup(argv[i]);
			if (!argRet->value)
				return nullptr;
			argRet->paramsFound = 0;
			argRet->flags = argument->flags;
		}
		ret[n++] = argRet.release();
	}
	/* Shrink as appropriate */
	auto result(makeUnique<constParsedArg_t []>(n + 1));
	if (!result)
		return nullptr;
	std::copy(ret.get(), ret.get() + n, result.get());
	result[n] = nullptr;
	return result;
}

constParsedArg_t findArg(constParsedArg_t *const args, const char *const value, const constParsedArg_t defaultValue)
{
	if (!args || !value)
		return defaultValue;
	for (uint32_t n = 0; args[n] != nullptr; ++n)
	{
		if (strcmp(args[n]->value.get(), value) == 0 || ((args[n]->flags & ARG_INCOMPLETE) &&
			strncmp(args[n]->value.get(), value, strlen(value)) == 0))
			return args[n];
	}
	return defaultValue;
}

const arg_t *findArgInArgs(const char *const value)
{
	const arg_t *curr = args;
	while (curr->value != nullptr)
	{
		if (((curr->flags & ARG_INCOMPLETE) == 0 && strcmp(curr->value, value) == 0) ||
			strncmp(curr->value, value, strlen(curr->value)) == 0)
			return curr;
		++curr;
	}
	return nullptr;
}
