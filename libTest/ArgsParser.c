#include "libTest.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Logger.h"
#include "Memory.h"
#include <string.h>

extern const arg args[];

parsedArg *checkAlreadyFound(parsedArg **parsedArgs, parsedArg *toCheck)
{
	int i;
	for (i = 0; parsedArgs[i] != NULL; i++)
	{
		parsedArg *arg = parsedArgs[i];
		if (strcasecmp(arg->value, toCheck->value) == 0)
			return arg;
	}
	return NULL;
}

uint32_t checkParams(int argc, char **argv, int argPos, arg *argument, arg *args)
{
	int i, n, min = argument->numMinParams, max = argument->numMaxParams;
	uint8_t eoa = FALSE;
	for (i = argPos, n = 0; i < argc && n < max && eoa == FALSE; i++)
	{
		arg *currArg = args;
		while (currArg->value != NULL && eoa == FALSE)
		{
			if (strcasecmp(currArg->value, argv[i]) == 0)
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

parsedArg **parseArguments(int argc, char **argv)
{
	parsedArg **ret;
	int i, n;

	if (argc < 1)
		return NULL;

	ret = testMalloc(sizeof(parsedArg *) * argc);
	for (i = 1, n = 0; i < argc; i++)
	{
		arg *argument = (arg *)args;
		parsedArg *argRet = testMalloc(sizeof(parsedArg));
		while (argument->value != NULL)
		{
			if (strcasecmp(argument->value, argv[i]) == 0)
			{
				int j;
				argRet->value = strdup(argv[i]);
				if (checkAlreadyFound(ret, argRet) != NULL)
				{
					testPrintf("Duplicate argument found: %s\n", argRet->value);
					free((void *)argRet->value);
					free(argRet);
					break;
				}
				argRet->paramsFound = checkParams(argc, argv, i + 1, argument, (arg *)args);
				argRet->params = testMalloc(sizeof(char *) * argRet->paramsFound);
				for (j = 0; j < argRet->paramsFound; j++)
					argRet->params[j] = strdup(argv[i + j + 1]);
				i += argRet->paramsFound;
				ret[n] = argRet;
				n++;
				break;
			}
			argument++;
		}
	}
	// Shrink as appropriate
	return testRealloc(ret, sizeof(parsedArg *) * (n + 1));
}

parsedArg *findArg(parsedArg **args, const char *value, parsedArg *defaultVal)
{
	int n;
	if (args == NULL)
		return defaultVal;
	for (n = 0; args[n] != NULL; n++)
	{
		if (strcasecmp(args[n]->value, value) == 0)
			return args[n];
	}
	return defaultVal;
}
