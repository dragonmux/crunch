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

#include "crunch++.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Logger.h"
#include <string.h>

#ifndef _MSC_VER
CRUNCH_API const arg args[];
#else
CRUNCH_API const arg *args = nullptr;

void registerArgs(const arg *allowedArgs)
{
	args = allowedArgs;
	stdout = &__iob_func()[1];
}
#endif

parsedArg *checkAlreadyFound(parsedArg **parsedArgs, parsedArg *toCheck)
{
	uint32_t i;
	for (i = 0; parsedArgs[i] != nullptr; i++)
	{
		parsedArg *arg = parsedArgs[i];
		if (strcmp(arg->value, toCheck->value) == 0)
			return arg;
	}
	return nullptr;
}

uint32_t checkParams(int argc, char **argv, int argPos, arg *argument, arg *args)
{
	uint32_t i, n, min = argument->numMinParams, max = argument->numMaxParams;
	bool eoa = false;
	for (i = argPos, n = 0; i < argc && n < max && !eoa; i++)
	{
		arg *currArg = args;
		while (currArg->value != nullptr && eoa == false)
		{
			if (strcmp(currArg->value, argv[i]) == 0)
				eoa = true;
			currArg++;
		}
		if (eoa)
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

parsedArg **parseArguments(int argc, char **argv)
{
	parsedArg **ret;
	int i, n;

	if (argc <= 1)
		return nullptr;

	ret = new parsedArg *[argc]();
	for (i = 1, n = 0; i < argc; i++)
	{
		bool found = false;
		arg *argument = (arg *)args;
		parsedArg *argRet = new parsedArg();
		while (argument->value != nullptr)
		{
			if (((argument->flags & ARG_INCOMPLETE) == 0 && strcmp(argument->value, argv[i]) == 0) ||
				strncmp(argument->value, argv[i], strlen(argument->value)) == 0)
			{
				found = true;
				argRet->value = strdup(argv[i]);
				if ((argument->flags & ARG_REPEATABLE) == 0 && checkAlreadyFound(ret, argRet) != nullptr)
				{
					testPrintf("Duplicate argument found: %s\n", argRet->value);
					free((void *)argRet->value);
					free(argRet);
					break;
				}
				argRet->paramsFound = checkParams(argc, argv, i + 1, argument, (arg *)args);
				argRet->params = new const char *[argRet->paramsFound];
				for (uint32_t j = 0; j < argRet->paramsFound; j++)
					argRet->params[j] = strNewDup(argv[i + j + 1]);
				i += argRet->paramsFound;
				argRet->flags = argument->flags;
				ret[n] = argRet;
				n++;
				break;
			}
			argument++;
		}
		if (!found)
		{
			argRet->value = strdup(argv[i]);
			argRet->paramsFound = 0;
			argRet->flags = argument->flags;
			ret[n] = argRet;
			n++;
		}
	}
	/* Shrink as appropriate */
	{
		parsedArg **temp = ret;
		ret = new parsedArg *[n + 1]();
		memcpy(ret, temp, sizeof(parsedArg *) * n);
		ret[n] = nullptr;
		delete [] temp;
		return ret;
	}
}

parsedArg *findArg(parsedArg **args, const char *value, parsedArg *defaultVal)
{
	int n;
	if (args == nullptr)
		return defaultVal;
	for (n = 0; args[n] != nullptr; n++)
	{
		if (((args[n]->flags & ARG_INCOMPLETE) == 0 && strcmp(args[n]->value, value) == 0) ||
			strncmp(args[n]->value, value, strlen(value)) == 0)
			return args[n];
	}
	return defaultVal;
}

arg *findArgInArgs(const char *value)
{
	arg *curr = (arg *)args;
	while (curr->value != nullptr)
	{
		if (((curr->flags & ARG_INCOMPLETE) == 0 && strcmp(curr->value, value) == 0) ||
			strncmp(curr->value, value, strlen(curr->value)) == 0)
			return curr;
		curr++;
	}
	return nullptr;
}
