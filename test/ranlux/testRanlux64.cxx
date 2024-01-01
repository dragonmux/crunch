// SPDX-License-Identifier: LGPL-3.0-or-later
#include <random>
#include <crunch++.h>
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
		CRUNCHpp_TEST(testConstruction)
		CRUNCHpp_TEST(testCorrectness)
	}
};

CRUNCHpp_TESTS(ranlux64Test)
