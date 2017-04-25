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

#include "crunch.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Logger.h"
#include "Memory.h"
#include <string.h>
#include <stdlib.h>

const arg *args = NULL;

void registerArgs(const arg *allowedArgs)
{
	args = allowedArgs;
#ifdef _MSC_VER
	stdout = &__iob_func()[1];
#endif
}

parsedArg *checkAlreadyFound(const parsedArg *const *const parsedArgs, const parsedArg *const toCheck)
{
	uint32_t i;
	for (i = 0; parsedArgs[i] != NULL; i++)
	{
		const parsedArg *const arg = parsedArgs[i];
		if (strcmp(arg->value, toCheck->value) == 0)
			return arg;
	}
	return NULL;
}

uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos, const arg *const argument, const arg *const args)
{
	uint32_t i, n, min = argument->numMinParams, max = argument->numMaxParams;
	uint8_t eoa = FALSE;
	for (i = argPos, n = 0; i < argc && n < max && eoa == FALSE; i++)
	{
		arg *currArg = args;
		while (currArg->value != NULL && eoa == FALSE)
		{
			if (strcmp(currArg->value, argv[i]) == 0)
				eoa = TRUE;
			currArg++;
		}
		if (eoa == TRUE)
			break;
		n++;
		if (n == max)
			break;
	}
	if (n < min)
		return -1;
	else
		return n;
}

parsedArg **parseArguments(const uint32_t argc, const char *const *const argv)
{
	parsedArg **ret;
	int i, n;

	if (argc <= 1)
		return NULL;

	ret = testMalloc(sizeof(parsedArg *) * argc);
	for (i = 1, n = 0; i < argc; i++)
	{
		uint8_t found = FALSE;
		arg *argument = (arg *)args;
		parsedArg *argRet = testMalloc(sizeof(parsedArg));
		while (argument->value != NULL)
		{
			if (((argument->flags & ARG_INCOMPLETE) == 0 && strcmp(argument->value, argv[i]) == 0) ||
				strncmp(argument->value, argv[i], strlen(argument->value)) == 0)
			{
				found = TRUE;
				argRet->value = strdup(argv[i]);
				if ((argument->flags & ARG_REPEATABLE) == 0 && checkAlreadyFound(ret, argRet) != NULL)
				{
					testPrintf("Duplicate argument found: %s\n", argRet->value);
					free((void *)argRet->value);
					free(argRet);
					break;
				}
				argRet->paramsFound = checkParams(argc, argv, i + 1, argument, (arg *)args);
				argRet->params = testMalloc(sizeof(char *) * argRet->paramsFound);
				for (uint32_t j = 0; j < argRet->paramsFound; j++)
					argRet->params[j] = strdup(argv[i + j + 1]);
				i += argRet->paramsFound;
				argRet->flags = argument->flags;
				ret[n] = argRet;
				n++;
				break;
			}
			argument++;
		}
		if (found == FALSE)
		{
			argRet->value = strdup(argv[i]);
			argRet->paramsFound = 0;
			argRet->flags = argument->flags;
			ret[n] = argRet;
			n++;
		}
	}
	/* Shrink as appropriate */
	return testRealloc(ret, sizeof(parsedArg *) * (n + 1));
}

const parsedArg *findArg(const parsedArg *const *const args, const char *const value, const parsedArg *const defaultVal)
{
	if (!args || !value)
		return defaultVal;
	for (uint32_t n = 0; args[n] != NULL; n++)
	{
		if (((args[n]->flags & ARG_INCOMPLETE) == 0 && strcmp(args[n]->value, value) == 0) ||
			strncmp(args[n]->value, value, strlen(value)) == 0)
			return args[n];
	}
	return defaultVal;
}

const arg *findArgInArgs(const char *const value)
{
	const arg *curr = args;
	while (curr->value != NULL)
	{
		if (((curr->flags & ARG_INCOMPLETE) == 0 && strcmp(curr->value, value) == 0) ||
			strncmp(curr->value, value, strlen(curr->value)) == 0)
			return curr;
		++curr;
	}
	return NULL;
}
