/*
 * This file is part of crunch
 * Copyright © 2017-2018 Rachel Mant (dx-mon@users.sourceforge.net)
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

class testArgsParser : public testsuit
{
private:
	void testNull()
	{
		const rawStrPtr_t argv[2] = {"test", "--dummy"};

		registerArgs(nullptr);
		assertTrue(parseArguments(0, nullptr).empty());
		assertTrue(parseArguments(1, nullptr).empty());
		assertTrue(parseArguments(2, nullptr).empty());
		assertTrue(parseArguments(1, argv).empty());
		assertTrue(parseArguments(2, argv).empty());
		assertTrue(parseArguments(uint32_t(-1), nullptr).empty());
		assertTrue(parseArguments(uint32_t(-1), argv).empty());
	}

	void testEmpty()
	{
		const rawStrPtr_t argv[2] = {"test", "--dummy"};
		const arg_t args[1] = {{{}, 0, 0, 0}};

		registerArgs(args);
		parsedArgs_t parsedArgs = parseArguments(2, argv);
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 1);
		assertFalse(parsedArgs[0].value.empty());
		assertEqual(parsedArgs[0].value.data(), "--dummy");
		assertEqual(parsedArgs[0].paramsFound, 0);
		assertEqual(parsedArgs[0].flags, 0);
		assertTrue(parsedArgs[0].params.empty());
	}

	void testIncomplete()
	{
		const rawStrPtr_t argv_1[2] = {"test", "--arg="};
		const rawStrPtr_t argv_2[2] = {"test", "--arg=test"};
		const arg_t args_1[2] =
		{
			{"--arg=", 0, 0, ARG_INCOMPLETE},
			{{}, 0, 0, 0}
		};
		const arg_t args_2[2] =
		{
			{"--arg", 0, 0, 0},
			{{}, 0, 0, 0}
		};

		parsedArgs_t parsedArgs;
		registerArgs(args_1);

		parsedArgs = parseArguments(2, argv_1);
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 1);
		assertFalse(parsedArgs[0].value.empty());
		assertEqual(parsedArgs[0].value.data(), "--arg=");
		assertEqual(parsedArgs[0].paramsFound, 0);
		assertEqual(parsedArgs[0].flags, ARG_INCOMPLETE);
		assertTrue(parsedArgs[0].params.empty());

		parsedArgs = parseArguments(2, argv_2);
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 1);
		assertFalse(parsedArgs[0].value.empty());
		assertEqual(parsedArgs[0].value.data(), "--arg=test");
		assertEqual(parsedArgs[0].flags, ARG_INCOMPLETE);
		assertTrue(parsedArgs[0].params.empty());

		startLogging("/dev/null");
		registerArgs(args_2);
		assertFalse(parseArguments(2, argv_1).empty());
		assertFalse(parseArguments(2, argv_2).empty());
	}

	void testInvalid() try
	{
		const rawStrPtr_t argv[3] = {"test", "--arg", "--arg"};
		const arg_t args[2] =
		{
			{"--arg", 0, 0, 0},
			{{}, 0, 0, 0}
		};
		parsedArgs_t parsedArgs{1};

		registerArgs(args);
		assertFalse(parsedArgs.empty());
		parsedArg_t &parsedArg = parsedArgs[0];
		parsedArg.value = "--arg";
		assertTrue(checkAlreadyFound(parsedArgs, parsedArg));

		assertNull(findArg({}, "", nullptr));

		// This checks that duplicate parameters work correctly by dropping the second copy of the parameter
		startLogging("/dev/null");
		parsedArgs = parseArguments(3, argv);
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 1);
	}
	catch (std::bad_alloc &e)
		{ fail(e.what()); }

	void testArgCounting()
	{
		const rawStrPtr_t argv_1[5] = {"test", "-o", "test", "-a", "-o"};
		const rawStrPtr_t argv_2[5] = {"test", "-o", "test", "me", "please"};
		const arg_t args[3] =
		{
			{"-o", 0, 2, 0},
			{"-a", 1, 1, 0},
			{{}, 0, 0, 0}
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
		assertTrue(parsedArgs.empty());

		parsedArgs = parseArguments(5, argv_2);
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 2);
		assertFalse(parsedArgs[0].value.empty());
		assertEqual(parsedArgs[0].value.data(), "-o");
		assertEqual(parsedArgs[0].paramsFound, 2);
		assertFalse(parsedArgs[0].params[0].empty());
		assertFalse(parsedArgs[0].params[1].empty());
		assertEqual(parsedArgs[0].params[0].data(), "test");
		assertEqual(parsedArgs[0].params[1].data(), "me");
	}

public:
	void registerTests()
	{
		CXX_TEST(testNull)
		CXX_TEST(testEmpty)
		CXX_TEST(testIncomplete)
		CXX_TEST(testInvalid)
		CXX_TEST(testArgCounting)
	}
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<testArgsParser>();
}
