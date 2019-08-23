/*
 * This file is part of crunch
 * Copyright © 2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <crunch.h>
#include "ArgsParser.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "StringFuncs.h"

#define nullptr (void *)0

void testNull()
{
	const char *argv[2] = {"test", "--dummy"};

	registerArgs(nullptr);
	assertConstNull(parseArguments(0, nullptr));
	assertConstNull(parseArguments(1, nullptr));
	assertConstNull(parseArguments(2, nullptr));
	assertConstNull(parseArguments(1, argv));
	assertConstNull(parseArguments(2, argv));
	assertConstNull(parseArguments((uint32_t)-1, nullptr));
	assertConstNull(parseArguments((uint32_t)-1, argv));
}

void testEmpty()
{
	const char *const argv[2] = {"test", "--dummy"};
	const arg_t args[1] = { {nullptr, 0, 0, 0} };

	registerArgs(args);
	parsedArgs_t parsedArgs = parseArguments(2, argv);
	assertConstNotNull(parsedArgs);
	assertConstNotNull(parsedArgs[0]);
	assertConstNotNull(parsedArgs[0]->value);
	assertStringEqual(parsedArgs[0]->value, "--dummy");
	assertIntEqual(parsedArgs[0]->paramsFound, 0);
	assertIntEqual(parsedArgs[0]->flags, 0);
	assertConstNull(parsedArgs[0]->params);
	freeParsedArgs(parsedArgs);
}

void testIncomplete()
{
	const char *const argv_1[2] = {"test", "--arg="};
	const char *const argv_2[2] = {"test", "--arg=test"};
	const arg_t args_1[2] =
	{
		{"--arg=", 0, 0, ARG_INCOMPLETE},
		{nullptr, 0, 0, 0}
	};
	const arg_t args_2[2] =
	{
		{"--arg", 0, 0, 0},
		{nullptr, 0, 0, 0}
	};

	parsedArgs_t parsedArgs;
	registerArgs(args_1);

	parsedArgs = parseArguments(2, argv_1);
	assertConstNotNull(parsedArgs);
	assertConstNotNull(parsedArgs[0]);
	assertConstNotNull(parsedArgs[0]->value);
	assertStringEqual(parsedArgs[0]->value, "--arg=");
	assertIntEqual(parsedArgs[0]->paramsFound, 0);
	assertIntEqual(parsedArgs[0]->flags, ARG_INCOMPLETE);
	assertConstNull(parsedArgs[0]->params);
	freeParsedArgs(parsedArgs);

	parsedArgs = parseArguments(2, argv_2);
	assertConstNotNull(parsedArgs);
	assertConstNotNull(parsedArgs[0]);
	assertConstNotNull(parsedArgs[0]->value);
	assertStringEqual(parsedArgs[0]->value, "--arg=test");
	assertIntEqual(parsedArgs[0]->flags, ARG_INCOMPLETE);
	assertConstNull(parsedArgs[0]->params);
	freeParsedArgs(parsedArgs);

	startLogging("/dev/null");
	registerArgs(args_2);
	assertConstNull(parseArguments(2, argv_1));
	assertConstNull(parseArguments(2, argv_2));
}

void testInvalid()
{
	const char *const argv[3] = {"test", "--arg", "--arg"};
	const arg_t args[2] =
	{
		{"--arg", 0, 0, 0},
		{nullptr, 0, 0, 0}
	};
	parsedArgs_t parsedArgs;
	parsedArg_t *parsedArg;

	registerArgs(args);
	parsedArgs = malloc(sizeof(constParsedArg_t *) * 2);
	assertConstNotNull(parsedArgs);
	parsedArg = malloc(sizeof(parsedArg_t));
	assertNotNull(parsedArg);
	parsedArg->value = strdup("--arg");
	parsedArgs[0] = parsedArg;
	assertTrue(checkAlreadyFound(parsedArgs, parsedArg));
	free((void *)parsedArg->value);
	free(parsedArg);
	free((void *)parsedArgs);

	assertConstNull(findArg(nullptr, "", nullptr));

	// This checks that duplicate parameters work correctly by dropping the second copy of the parameter
	parsedArgs = parseArguments(3, argv);
	assertConstNotNull(parsedArgs);
	assertConstNotNull(parsedArgs[0]);
	assertConstNull(parsedArgs[1]);
	freeParsedArgs(parsedArgs);

	assertTrue(freeParsedArg(NULL));
	assertNull(freeParsedArgs(NULL));
}

void testArgCounting()
{
	const char *const argv_1[5] = {"test", "-o", "test", "-a", "-o"};
	const char *const argv_2[5] = {"test", "-o", "test", "me", "please"};
	const arg_t args[3] =
	{
		{"-o", 0, 2, 0},
		{"-a", 1, 1, 0},
		{nullptr, 0, 0, 0}
	};
	parsedArgs_t parsedArgs;

	registerArgs(args);
	assertIntEqual(checkParams(5, argv_1, 2, &args[0], args), 1);
	assertIntEqual(checkParams(5, argv_1, 4, &args[1], args), -1);
	assertIntEqual(checkParams(5, argv_1, 5, &args[0], args), 0);
	assertIntEqual(checkParams(5, argv_2, 2, &args[0], args), 2);

	parsedArgs = parseArguments(5, argv_1);
	assertConstNull(parsedArgs);

	parsedArgs = parseArguments(5, argv_2);
	assertConstNotNull(parsedArgs);
	assertConstNotNull(parsedArgs[0]);
	assertConstNotNull(parsedArgs[0]->value);
	assertStringEqual(parsedArgs[0]->value, "-o");
	assertIntEqual(parsedArgs[0]->paramsFound, 2);
	assertConstNotNull(parsedArgs[0]->params[0]);
	assertConstNotNull(parsedArgs[0]->params[1]);
	assertStringEqual(parsedArgs[0]->params[0], "test");
	assertStringEqual(parsedArgs[0]->params[1], "me");
	freeParsedArgs(parsedArgs);
}

void testAllocs()
{
	const arg_t args[3] =
	{
		{"-o", 0, 2, 0},
		{"-a", 1, 1, 0},
		{nullptr, 0, 0, 0}
	};
	const char *const argv[7] = {"test", "-o", "test", "me", "please", "-a", "file"};

	puts("Testing overall alloc fail");
	registerArgs(args);
	allocCount = 0;
	assertNull(parseArguments(7, argv));
	assertIntEqual(allocCount, -1);
	puts("Testing arg alloc fail");
	allocCount = 1;
	assertNull(parseArguments(7, argv));
	assertIntEqual(allocCount, -1);
	/*puts("Testing arg strdup alloc fail");
	allocCount = 2;
	assertNull(parseArguments(7, argv));
	assertIntEqual(allocCount, -1);*/
	puts("Testing params alloc fail");
	allocCount = 3;
	assertNull(parseArguments(7, argv));
	assertIntEqual(allocCount, -1);
	/*puts("Testing opt strdup alloc fail");
	allocCount = 4;
	assertNull(parseArguments(7, argv));
	assertIntEqual(allocCount, -1);
	puts("Testing overall alloc fail");*/
	allocCount = 7;
	assertNull(parseArguments(7, argv));
	assertIntEqual(allocCount, -1);
	allocCount = 10;
	assertNull(parseArguments(7, argv));
	assertIntEqual(allocCount, -1);
}

BEGIN_REGISTER_TESTS()
	TEST(testNull)
	TEST(testEmpty)
	TEST(testIncomplete)
	TEST(testInvalid)
	TEST(testArgCounting)
	TEST(testAllocs)
END_REGISTER_TESTS()
