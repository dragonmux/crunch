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
#include "StringFuncs.h"

#define nullptr (void *)0

void testNull()
{
	const char *argv[2] = {"test", "--dummy"};

	registerArgs(nullptr);
	assertNull(parseArguments(0, nullptr));
	assertNull(parseArguments(1, nullptr));
	assertNull(parseArguments(2, nullptr));
	assertNull(parseArguments(1, argv));
	assertNull(parseArguments(2, argv));
	assertNull(parseArguments((uint32_t)-1, nullptr));
	assertNull(parseArguments((uint32_t)-1, argv));
}

void testEmpty()
{
	const char *const argv[2] = {"test", "--dummy"};
	const arg args[1] = { {nullptr, 0, 0, 0} };

	registerArgs(args);
	parsedArg **parsedArgs = parseArguments(2, argv);
	assertNotNull(parsedArgs);
	assertNotNull(parsedArgs[0]);
	assertConstNotNull(parsedArgs[0]->value);
	assertStringEqual(parsedArgs[0]->value, "--dummy");
	assertIntEqual(parsedArgs[0]->paramsFound, 0);
	assertIntEqual(parsedArgs[0]->flags, 0);
	assertNull(parsedArgs[0]->params);
}

BEGIN_REGISTER_TESTS()
	TEST(testNull)
	TEST(testEmpty)
END_REGISTER_TESTS()
