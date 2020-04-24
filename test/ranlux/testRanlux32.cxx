/*
 * This file is part of crunch
 * Copyright © 2020 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <random>
#include <crunch++/crunch++.h>
#include "ranlux.h"

using ranlux32Base_t = std::subtract_with_carry_engine<uint32_t, 32, 3, 17>;
using ranlux32STL_t = std::discard_block_engine<ranlux32Base_t, 389, 16>;

struct ranlux32Test final : public testsuite
{
private:
	uint32_t seed;

public:
	ranlux32Test() : seed{randomSeed32()} { }

private:
	void testConstruction()
	{
		auto *state{initRanlux32(seed)};
		assertNotNull(state);
		freeRanlux32(state);
	}

	void testCorrectness()
	{
		ranlux32STL_t stdRanlux{seed};
		auto *crunchRanlux{initRanlux32(seed)};
		assertNotNull(crunchRanlux);

		// Check the first 65536 numbers from the two impls are equal..
		for (uint32_t i = 0; i < 65536U; ++i)
		{
			const auto stdResult = stdRanlux();
			const auto crunchResult = genRanlux32(crunchRanlux);
			assertEqual(crunchResult, stdResult);
		}

		freeRanlux32(crunchRanlux);
	}

public:
	void registerTests() final
	{
		CXX_TEST(testConstruction)
		CXX_TEST(testCorrectness)
	}
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<ranlux32Test>();
}
