// SPDX-License-Identifier: LGPL-3.0-or-later
#include <stdlib.h>
#ifdef __GNUC__
#define __USE_XOPEN_EXTENDED
#endif
#include <string.h>
#include "crunch.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Logger.h"

const arg_t *args = NULL;

void registerArgs(const arg_t *allowedArgs) { args = allowedArgs; }

bool checkAlreadyFound(const constParsedArgs_t parsedArgs, const parsedArg_t *const toCheck)
{
	for (uint32_t i = 0; parsedArgs[i] != NULL; i++)
	{
		if (strcmp(parsedArgs[i]->value, toCheck->value) == 0)
			return true;
	}
	return false;
}

uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos,
	const arg_t *const argument, const arg_t *const args_)
{
	uint32_t n = 0;
	const uint32_t min = argument->numMinParams;
	const uint32_t max = argument->numMaxParams;
	bool eoa = false;
	for (uint32_t i = argPos; i < argc && n < max && !eoa; ++i)
	{
		const arg_t *currArg = args_;
		while (currArg->value != NULL && !eoa)
		{
			if (strcmp(currArg->value, argv[i]) == 0)
				eoa = true;
			++currArg;
		}
		if (eoa || ++n == max)
			break;
	}
	if (n < min)
		return UINT32_MAX;
	return n;
}

void freeParsedArg(parsedArg_t *parsedArg)
{
	if (parsedArg)
	{
		if (parsedArg->params)
		{
			for (uint32_t i = 0; i < parsedArg->paramsFound; ++i)
				free((void *)parsedArg->params[i]);
		}
		free((void *)parsedArg->params);
		free((void *)parsedArg->value);
	}
	free(parsedArg);
}

void *freeParsedArgs_(parsedArgs_t parsedArgs)
{
	if (parsedArgs)
	{
		for (uint32_t i = 0; parsedArgs[i]; ++i)
			freeParsedArg((parsedArg_t *)parsedArgs[i]);
	}
	free(parsedArgs);
	return NULL;
}

void *freeParsedArgs(constParsedArgs_t parsedArgs)
	{ return freeParsedArgs_((parsedArgs_t)parsedArgs); }

constParsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv)
{
	if (argc < 1U || (argc >> 31U) == 1U || !argv || !args)
		return NULL;

	parsedArgs_t ret = malloc(sizeof(constParsedArg_t) * argc);
	if (!ret)
		return NULL;
	memset((void *)ret, 0, sizeof(constParsedArg_t) * argc);
	uint32_t n = 0;
	for (uint32_t i = 1; i < argc; i++)
	{
		bool found = false;
		bool skip = false;
		const arg_t *argument = args;
		parsedArg_t *argRet = malloc(sizeof(parsedArg_t));
		if (!argRet)
			return freeParsedArgs_(ret);
		memset(argRet, 0, sizeof(parsedArg_t));
		while (argument->value)
		{
			found = strcmp(argument->value, argv[i]) == 0 || ((argument->flags & ARG_INCOMPLETE) &&
				strncmp(argument->value, argv[i], strlen(argument->value)) == 0);
			if (found)
			{
				argRet->value = strdup(argv[i]);
				if (!argRet->value)
				{
					freeParsedArg(argRet);
					return freeParsedArgs_(ret);
				}
				skip = !(argument->flags & ARG_REPEATABLE) && checkAlreadyFound(ret, argRet);
				if (skip)
				{
					testPrintf("Duplicate argument found: %s\n", argRet->value);
					freeParsedArg(argRet);
					break;
				}
				argRet->paramsFound = checkParams(argc, argv, i + 1, argument, args);
				if (argRet->paramsFound == (uint32_t)-1)
				{
					printf("Not enough parameters given for argument %s\n", argv[i]);
					freeParsedArg(argRet);
					return freeParsedArgs_(ret);
				}
				// Only allocate for the params if there are any found, otherwise let the pointer dwell as nullptr.
				if (argRet->paramsFound)
				{
					argRet->params = malloc(sizeof(char *) * argRet->paramsFound);
					if (!argRet->params)
					{
						freeParsedArg(argRet);
						return freeParsedArgs_(ret);
					}
					memset((void *)argRet->params, 0, sizeof(char *) * argRet->paramsFound);
					for (uint32_t j = 0; j < argRet->paramsFound; ++j)
					{
						argRet->params[j] = strdup(argv[i + j + 1]);
						if (!argRet->params[j])
						{
							freeParsedArg(argRet);
							return freeParsedArgs_(ret);
						}
					}
				}
				i += argRet->paramsFound;
				argRet->flags = argument->flags;
				break;
			}
			else if (!(argument->flags & ARG_INCOMPLETE) &&
				strncmp(argument->value, argv[i], strlen(argument->value)) == 0)
			{
				printf("Badly formatted argument (%s)\n", argv[i]);
				freeParsedArg(argRet);
				return freeParsedArgs_(ret);
			}
			++argument;
		}
		if (skip)
			continue;
		else if (!found)
		{
			argRet->value = strdup(argv[i]);
			if (!argRet->value)
			{
				freeParsedArg(argRet);
				return freeParsedArgs_(ret);
			}
			argRet->paramsFound = 0;
			argRet->flags = argument->flags;
		}
		ret[n++] = argRet;
	}
	/* Shrink as appropriate */
	parsedArgs_t result = malloc(sizeof(constParsedArg_t) * (n + 1));
	if (!result)
		return freeParsedArgs_(ret);
	memcpy((void *)result, ret, sizeof(constParsedArg_t) * n);
	free((void *)ret);
	result[n] = NULL;
	return result;
}

constParsedArg_t findArg(const constParsedArg_t *const args_, const char *const value, const constParsedArg_t defaultVal)
{
	if (!args_ || !value)
		return defaultVal;
	for (uint32_t n = 0; args_[n] != NULL; n++)
	{
		if (strcmp(args_[n]->value, value) == 0 || ((args_[n]->flags & ARG_INCOMPLETE) &&
			strncmp(args_[n]->value, value, strlen(value)) == 0))
			return args_[n];
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
