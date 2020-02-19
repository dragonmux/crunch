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

#include <cstdint>
#include <random>
#include <crunch++/memory.hxx>
#include "ranlux.h"

struct ranlux64Base_t final
{
private:
	constexpr static uint8_t wordSize = 64;
	constexpr static uint8_t shortLag = 5;
	constexpr static uint8_t longLag = 62;
	constexpr static uint32_t defaultSeed = 19780503U;

	std::array<uint64_t, longLag> x;
	uint8_t carry;
	uint8_t p;

public:
	ranlux64Base_t(const uint64_t seed) noexcept : x{}, carry{}, p{0}
	{
		std::linear_congruential_engine<uint64_t, 40014U, 0U, 2147483563U> lcg
			{seed ? seed : defaultSeed};

		for (auto &stateWord : x)
			stateWord = (lcg() & 0xFFFFFFFFU) + ((lcg() & 0xFFFFFFFFU) << 32U);
		carry = !x.back();
	}

	uint64_t operator ()() noexcept
	{
		int16_t shortIndex = p - shortLag;
		if (shortIndex < 0)
			shortIndex += longLag;

		uint64_t newX{x[p] - x[shortIndex] - carry};
		carry = x[p] < x[shortIndex] + carry;
		x[p] = newX;
		if (++p >= longLag)
			p = 0;
		return newX;
	}

	void discard(uint16_t values)
	{
		while (values--)
			operator()();
	}
};

struct ranlux64_t final
{
private:
	ranlux64Base_t baseState;
	uint16_t blockUsed;

	constexpr static uint16_t luxury = 1303;
	constexpr static uint8_t blockSize = 62;

public:
	ranlux64_t(const uint64_t seed) noexcept : baseState{seed}, blockUsed{0} { }

	uint64_t operator ()() noexcept
	{
		if (blockUsed >= blockSize)
		{
			baseState.discard(luxury - blockUsed);
			blockUsed = 0;
		}
		++blockUsed;
		return baseState();
	}
};

ranlux64_t *initRanlux64(const uint64_t seed) try
	{ return makeUnique<ranlux64_t>(seed).release(); }
catch (...)
	{ return nullptr; }

void freeRanlux64(ranlux64_t *const state)
	{ std::unique_ptr<ranlux64_t> ranluxState{state}; }

uint64_t genRanlux64(ranlux64_t *const state) { return (*state)(); }
