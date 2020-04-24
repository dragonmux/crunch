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
#include "testEngines.hxx"

using ranlux64Base_t = subtract_with_borrow_engine<uint64_t, 64, 5, 62>;
using ranlux64STL_t = std::discard_block_engine<ranlux64Base_t, 1303, 62>;

struct ranlux64Test final : public testsuite
{
private:
	uint64_t seed;

public:
	ranlux64Test() : seed{randomSeed64()} { }

private:
	void testConstruction()
	{
		auto *state{initRanlux64(seed)};
		assertNotNull(state);
		freeRanlux64(state);
	}

	void testCorrectness()
	{
		ranlux64STL_t stdRanlux{seed};
		auto *crunchRanlux{initRanlux64(seed)};
		assertNotNull(crunchRanlux);

		// Check the first 65536 numbers from the two impls are equal..
		for (uint32_t i = 0; i < 65536U; ++i)
		{
			const auto stdResult = stdRanlux();
			const auto crunchResult = genRanlux64(crunchRanlux);
			assertEqual(crunchResult, stdResult);
		}

		freeRanlux64(crunchRanlux);
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
	registerTestClasses<ranlux64Test>();
}
