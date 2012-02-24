#include "Core.h"
#include "Logger.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Memory.h"
#include <string.h>
#include <unistd.h>

parsedArg **parsedArgs = NULL;
parsedArg **linkLibs = NULL;
parsedArg **linkObjs = NULL;
parsedArg **namedTests = NULL;
uint32_t numTests = 0, numLibs = 0, numObjs = 0;

#ifndef __MSC_VER
#ifdef __x86_64__
#define COMPILER	"gcc -m64 -fPIC -DPIC"
#else
#define COMPILER	"gcc -m32"
#endif
#define OPTS	"-shared %s%s-lTest -O2 -o "
#else
// _M_64
// TODO: Figure out the trickery needed to get this working!
#define COMPILER	"cl"
#endif

arg args[] =
{
	{"-l", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{"-o", 0, 0, ARG_REPEATABLE | ARG_INCOMPLETE},
	{0}
};

uint8_t getTests()
{
	uint32_t i, j, n;
	for (n = 0; parsedArgs[n] != NULL; n++);
	namedTests = testMalloc(sizeof(parsedArg *) * (n + 1));

	for (j = 0, i = 0; i < n; i++)
	{
		if (findArgInArgs(parsedArgs[i]->value) == NULL)
		{
			namedTests[j] = parsedArgs[i];
			j++;
		}
	}
	if (j == 0)
	{
		free(namedTests);
		return FALSE;
	}
	else
	{
		namedTests = testRealloc(namedTests, sizeof(parsedArg *) * (j + 1));
		numTests = j;
		return TRUE;
	}
}

#define getLinkFunc(name, var, num, find) \
void name() \
{ \
	uint32_t i, n; \
	for (n = 0; parsedArgs[n] != NULL; n++); \
	var = testMalloc(sizeof(parsedArg *) * (n + 1)); \
	for (num = 0, i = 0; i < n; i++) \
	{ \
		if (strncmp(parsedArgs[i]->value, find, 2) == 0) \
		{ \
			var[num] = parsedArgs[i]; \
			num++; \
		} \
	} \
	if (num == 0) \
		free(var); \
	else \
		var = testRealloc(var, sizeof(parsedArg *) * (num + 1)); \
}

getLinkFunc(getLinkLibs, linkLibs, numLibs, "-l");
getLinkFunc(getLinkObjs, linkObjs, numObjs, "-o");
#undef getLinkFunc

const char *CToSO(const char *file)
{
	// TODO: make this not suck by checking file extensions, etc
	int fileLen = strlen(file);
	char *soFile = malloc(fileLen + 2);
	strcpy(soFile, file);
	soFile[fileLen - 1] = 's';
	soFile[fileLen++] = 'o';
	soFile[fileLen] = '\0';
	return soFile;
}

#define toStringFunc(name, var, num, offset) \
const char *name ## ToString() \
{ \
	int i; \
	const char *ret = strdup(""); \
	for (i = 0; i < num; i++) \
	{ \
		const char *name = formatString("%s%s ", ret, var[i]->value offset); \
		free((void *)ret); \
		ret = name; \
	} \
	return ret; \
}

toStringFunc(objs, linkObjs, numObjs, + 2);
toStringFunc(libs, linkLibs, numLibs, );
#undef toStringFunc

int compileTests()
{
	int i, ret = 0;
	const char *objs = objsToString(), *libs = libsToString();

	for (i = 0; i < numTests; i++)
	{
		if (access(namedTests[i]->value, R_OK) == 0)
		{
			const char *soFile = CToSO(namedTests[i]->value);
			char *compileString = formatString(COMPILER " " OPTS "%s %s", objs, libs, soFile, namedTests[i]->value);
			printf("%s\n", compileString);
			ret = system(compileString);
			free(compileString);
			free((void *)soFile);
			if (ret != 0)
				break;
		}
		else
			testPrintf("Error, %s does not exist, skipping..\n", namedTests[i]->value);
	}
	free((void *)objs);
	free((void *)libs);
	return ret;
}

int main(int argc, char **argv)
{
	parsedArgs = parseArguments(argc, argv);
	if (parsedArgs == NULL || getTests() == FALSE)
	{
		testPrintf("Fatal error: There are no source files to build given on the command line!\n");
		return 2;
	}
	getLinkLibs();
	getLinkObjs();
	return compileTests();
}
