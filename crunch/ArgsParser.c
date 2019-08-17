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

#include <stdlib.h>
#ifdef __GNUC__
#define __USE_XOPEN_EXTENDED
#endif
#include <string.h>
#include "crunch.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Logger.h"
#include "Memory.h"

const arg_t *args = NULL;

void registerArgs(const arg_t *allowedArgs)
{
	args = allowedArgs;
#if defined(_MSC_VER) && _MSC_VER < 1900L
	stdout = &__iob_func()[1];
#endif
}

uint8_t checkAlreadyFound(const parsedArgs_t parsedArgs, const parsedArg_t *const toCheck)
{
	for (uint32_t i = 0; parsedArgs[i] != NULL; i++)
	{
		if (strcmp(parsedArgs[i]->value, toCheck->value) == 0)
			return TRUE;
	}
	return FALSE;
}

uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos, const arg_t *const argument, const arg_t *const args)
{
	uint32_t n = 0;
	const uint32_t min = argument->numMinParams, max = argument->numMaxParams;
	uint8_t eoa = FALSE;
	for (uint32_t i = argPos; i < argc && n < max && !eoa; ++i)
	{
		const arg_t *currArg = args;
		while (currArg->value != NULL && !eoa)
		{
			if (strcmp(currArg->value, argv[i]) == 0)
				eoa = TRUE;
			++currArg;
		}
		if (eoa || ++n == max)
			break;
	}
	if (n < min)
		return UINT32_MAX;
	return n;
}

uint8_t freeParsedArg(parsedArg_t *parsedArg)
{
	if (!parsedArg)
		return TRUE;
	if (parsedArg->params)
	{
		for (uint32_t i = 0; i < parsedArg->paramsFound; ++i)
			free((void *)parsedArg->params[i]);
		free(parsedArg->params);
	}
	free((void *)parsedArg->value);
	free(parsedArg);
	return TRUE;
}

void *freeParsedArgs(parsedArgs_t parsedArgs)
{
	if (!parsedArgs)
		return NULL;
	for (uint32_t i = 0; parsedArgs[i]; ++i)
		freeParsedArg((void *)parsedArgs[i]);
	free(parsedArgs);
	return NULL;
}

parsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv)
{
	if (argc < 1 || (argc >> 31) == 1 || !argv || !args)
		return NULL;

	parsedArgs_t ret = malloc(sizeof(constParsedArg_t) * argc);
	if (!ret)
		return NULL;
	memset(ret, 0, sizeof(constParsedArg_t) * argc);
	uint32_t n = 0;
	for (uint32_t i = 1; i < argc; i++)
	{
		uint8_t found = FALSE, skip = FALSE;
		const arg_t *argument = args;
		parsedArg_t *argRet = malloc(sizeof(parsedArg_t));
		if (!argRet)
			return freeParsedArgs(ret);
		memset(argRet, 0, sizeof(parsedArg_t));
		while (argument->value != NULL)
		{
			found = strcmp(argument->value, argv[i]) == 0 || ((argument->flags & ARG_INCOMPLETE) &&
				strncmp(argument->value, argv[i], strlen(argument->value)) == 0);
			if (found)
			{
				argRet->value = strdup(argv[i]);
				if (!(argument->flags & ARG_REPEATABLE) && checkAlreadyFound(ret, argRet))
				{
					testPrintf("Duplicate argument found: %s\n", argRet->value);
					skip = freeParsedArg(argRet);
					break;
				}
				argRet->paramsFound = checkParams(argc, argv, i + 1, argument, args);
				if (argRet->paramsFound == (uint32_t)-1)
				{
					printf("Not enough parameters given for argument %s\n", argv[i]);
					return NULL;
				}
				// Only allocate for the params if there are any found, otherwise let the pointer dwell as nullptr.
				if (argRet->paramsFound)
				{
					argRet->params = malloc(sizeof(char *) * argRet->paramsFound);
					if (!argRet->params)
						return freeParsedArg(argRet), freeParsedArgs(ret);
					memset(argRet->params, 0, sizeof(char *) * argRet->paramsFound);
					for (uint32_t j = 0; j < argRet->paramsFound; ++j)
					{
						argRet->params[j] = strdup(argv[i + j + 1]);
						if (!argRet->params[j])
							return freeParsedArg(argRet), freeParsedArgs(ret);
					}
				}
				i += argRet->paramsFound;
				argRet->flags = argument->flags;
				break;
			}
			else if (!(argument->flags & ARG_INCOMPLETE) && strncmp(argument->value, argv[i], strlen(argument->value)) == 0)
			{
				printf("Badly formatted argument (%s)\n", argv[i]);
				return freeParsedArg(argRet), freeParsedArgs(ret);
			}
			++argument;
		}
		if (skip)
			continue;
		else if (!found)
		{
			argRet->value = strdup(argv[i]);
			if (!argRet->value)
				return freeParsedArg(argRet), freeParsedArgs(ret);
			argRet->paramsFound = 0;
			argRet->flags = argument->flags;
		}
		ret[n++] = argRet;
	}
	/* Shrink as appropriate */
	parsedArgs_t result = malloc(sizeof(constParsedArg_t) * (n + 1));
	if (!result)
		return freeParsedArgs(ret);
	memcpy(result, ret, sizeof(constParsedArg_t *) * n);
	free(ret);
	result[n] = NULL;
	return result;
}

constParsedArg_t findArg(constParsedArg_t *const args, const char *const value, const constParsedArg_t defaultVal)
{
	if (!args || !value)
		return defaultVal;
	for (uint32_t n = 0; args[n] != NULL; n++)
	{
		if (strcmp(args[n]->value, value) == 0 || ((args[n]->flags & ARG_INCOMPLETE) &&
			strncmp(args[n]->value, value, strlen(value)) == 0))
			return args[n];
	}
	return defaultVal;
}

const arg_t *findArgInArgs(const char *const value)
{
	const arg_t *curr = args;
	while (curr->value != NULL)
	{
		if (((curr->flags & ARG_INCOMPLETE) == 0 && strcmp(curr->value, value) == 0) ||
			strncmp(curr->value, value, strlen(curr->value)) == 0)
			return curr;
		++curr;
	}
	return NULL;
}
