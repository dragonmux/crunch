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

#include <crunch++.h>
#include "ArgsParser.h"
#include "memory.hxx"
#include "StringFuncs.h"

using rawStrPtr_t = const char *;
using strPtr_t = std::unique_ptr<const char []>;

class testArgsParser : public testsuit
{
public:
	void testNull()
	{
		const rawStrPtr_t argv[2] = {"test", "--dummy"};

		registerArgs(nullptr);
		assertNull(parseArguments(0, nullptr));
		assertNull(parseArguments(1, nullptr));
		assertNull(parseArguments(2, nullptr));
		assertNull(parseArguments(1, argv));
		assertNull(parseArguments(2, argv));
		assertNull(parseArguments(uint32_t(-1), nullptr));
		assertNull(parseArguments(uint32_t(-1), argv));
	}

	void testEmpty()
	{
		const rawStrPtr_t argv[2] = {"test", "--dummy"};
		const arg_t args[1] = { {nullptr, 0, 0, 0} };

		registerArgs(args);
		parsedArgs_t parsedArgs = parseArguments(2, argv);
		assertNotNull(parsedArgs);
		assertNotNull(parsedArgs[0]);
		assertNotNull(parsedArgs[0]->value);
		assertEqual(parsedArgs[0]->value.get(), "--dummy");
		assertEqual(parsedArgs[0]->paramsFound, 0);
		assertEqual(parsedArgs[0]->flags, 0);
		assertNull(parsedArgs[0]->params);
	}

	void testIncomplete()
	{
		const rawStrPtr_t argv_1[2] = {"test", "--arg="};
		const rawStrPtr_t argv_2[2] = {"test", "--arg=test"};
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
		assertNotNull(parsedArgs);
		assertNotNull(parsedArgs[0]);
		assertNotNull(parsedArgs[0]->value);
		assertEqual(parsedArgs[0]->value.get(), "--arg=");
		assertEqual(parsedArgs[0]->paramsFound, 0);
		assertEqual(parsedArgs[0]->flags, ARG_INCOMPLETE);
		assertNull(parsedArgs[0]->params);

		parsedArgs = parseArguments(2, argv_2);
		assertNotNull(parsedArgs);
		assertNotNull(parsedArgs[0]);
		assertNotNull(parsedArgs[0]->value);
		assertEqual(parsedArgs[0]->value.get(), "--arg=test");
		assertEqual(parsedArgs[0]->flags, ARG_INCOMPLETE);
		assertNull(parsedArgs[0]->params);

		startLogging("/dev/null");
		registerArgs(args_2);
		assertNotNull(parseArguments(2, argv_1));
		assertNotNull(parseArguments(2, argv_2));
	}

	void testInvalid()
	{
		const rawStrPtr_t argv[3] = {"test", "--arg", "--arg"};
		const arg_t args[2] =
		{
			{"--arg", 0, 0, 0},
			{nullptr, 0, 0, 0}
		};
		parsedArgs_t parsedArgs;
		std::unique_ptr<parsedArg_t> parsedArg;

		registerArgs(args);
		parsedArgs = makeUnique<constParsedArg_t []>(2);
		assertNotNull(parsedArgs);
		parsedArg = makeUnique<parsedArg_t>();
		assertNotNull(parsedArg);
		parsedArg->value = strNewDup("--arg");
		parsedArgs[0] = parsedArg.get();
		assertTrue(checkAlreadyFound(parsedArgs, *parsedArg));

		assertNull(findArg(nullptr, "", nullptr));

		// This checks that duplicate parameters work correctly by dropping the second copy of the parameter
		startLogging("/dev/null");
		parsedArgs = parseArguments(3, argv);
		assertNotNull(parsedArgs);
		assertNotNull(parsedArgs[0]);
		assertNull(parsedArgs[1]);
	}

	void testArgCounting()
	{
		const rawStrPtr_t argv_1[5] = {"test", "-o", "test", "-a", "-o"};
		const rawStrPtr_t argv_2[5] = {"test", "-o", "test", "me", "please"};
		const arg_t args[3] =
		{
			{"-o", 0, 2, 0},
			{"-a", 1, 1, 0},
			{nullptr, 0, 0, 0}
		};
		parsedArgs_t parsedArgs;

		registerArgs(args);
		assertEqual(checkParams(5, argv_1, 2, args[0], args), 1);
		assertEqual(checkParams(5, argv_1, 4, args[1], args), -1);
		assertEqual(checkParams(5, argv_1, 5, args[0], args), 0);
		assertEqual(checkParams(5, argv_2, 2, args[0], args), 2);

		auto log = startLogging("/dev/null");
		parsedArgs = parseArguments(5, argv_1);
		stopLogging(log);
		assertNull(parsedArgs);

		parsedArgs = parseArguments(5, argv_2);
		assertNotNull(parsedArgs);
		assertNotNull(parsedArgs[0]);
		assertNotNull(parsedArgs[0]->value);
		assertEqual(parsedArgs[0]->value.get(), "-o");
		assertEqual(parsedArgs[0]->paramsFound, 2);
		assertNotNull(parsedArgs[0]->params[0]);
		assertNotNull(parsedArgs[0]->params[1]);
		assertEqual(parsedArgs[0]->params[0].get(), "test");
		assertEqual(parsedArgs[0]->params[1].get(), "me");
	}

	void registerTests()
	{
		CXX_TEST(testNull)
		CXX_TEST(testEmpty)
		CXX_TEST(testIncomplete)
		CXX_TEST(testInvalid)
		CXX_TEST(testArgCounting)
	}
};

CRUNCH_API void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<testArgsParser>();
}
