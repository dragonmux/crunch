// SPDX-License-Identifier: LGPL-3.0-or-later
#include <random>
#include <crunch++.h>
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
		CRUNCHpp_TEST(testConstruction)
		CRUNCHpp_TEST(testCorrectness)
	}
};

CRUNCHpp_TESTS(ranlux32Test)
