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

uint32_t checkParams(int argc, char **argv, int argPos, arg *argument)
{
	// Figure out how to parse for parameters up until the minimum is parsed and then till the max is parsed or we exceed argc-argPos.
	return 0;
}

parsedArg **parseArguments(int argc, char **argv)
{
	parsedArg **ret;
	int i, n;

	if (argc <= 1)
		return NULL;

	ret = testMalloc(sizeof(parsedArg *) * argc);
	for (i = 0, n = 0; i < argc; i++)
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
				argRet->paramsFound = checkParams(argc, argv, i + 1, argument);
				argRet->params = testMalloc(sizeof(char *) * argRet->paramsFound);
				for (j = 0; j < argRet->paramsFound; j++)
					argRet->params[j] = strdup(argv[i + j + 1]);
				i += argRet->paramsFound;
				ret[n] = argRet;
				n++;
			}
		}
	}
	// Shrink as appropriate
	return realloc(ret, sizeof(parsedArg *) * n);
}

parsedArg *findArg(parsedArg **args, const char *value, parsedArg *defaultVal)
{
	return defaultVal;
}
