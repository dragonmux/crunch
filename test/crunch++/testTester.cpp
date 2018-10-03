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

struct testException_t final : std::exception
{
public:
	const char *what() const noexcept final override
		{ return "Boo!"; }
};

class throwTest final : public testsuit
{
private:
	void testSkip()
		{ skip("I'm a skipped test"); }

	void testThrow()
		{ throw testException_t(); }

public:
	void registerTests() final override
	{
		CXX_TEST(testSkip)
		CXX_TEST(testThrow)
	}
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<throwTest>();
}
