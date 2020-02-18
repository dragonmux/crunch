#include <cstdint>
#include <random>
#include <crunch++/memory.hxx>
#include "ranlux.h"

struct ranlux32Base_t final
{
private:
	std::array<uint32_t, 17> x;
	uint8_t carry;
	uint8_t p;

	constexpr static uint8_t wordSize = 32;
	constexpr static uint8_t shortLag = 3;
	constexpr static uint8_t longLag = 17;
	constexpr static uint32_t defaultSeed = 19780503U;

public:
	ranlux32Base_t(const uint32_t seed) noexcept : x{}, carry{}, p{0}
	{
		std::linear_congruential_engine<uint32_t, 40014U, 0U, 2147483563U> lcg
			{seed ? seed : defaultSeed};

		for (auto &stateWord : x)
			stateWord = lcg();
		carry = !x.back();
	}

	uint32_t operator ()() noexcept
	{
		int16_t shortIndex = p - shortLag;
		if (shortIndex < 0)
			shortIndex += longLag;

		uint32_t newX{x[shortIndex] - x[p] - carry};
		carry = x[shortIndex] < x[p] + carry;
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

struct ranlux32_t final
{
private:
	ranlux32Base_t baseState;
	uint32_t blockUsed;

	constexpr static uint16_t luxury = 389;
	constexpr static uint8_t blockSize = 16;

public:
	ranlux32_t(const uint32_t seed) noexcept : baseState{seed}, blockUsed{0} { }

	uint32_t operator ()() noexcept
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

void *initRanlux32(const uint32_t seed) try
	{ return makeUnique<ranlux32_t>(seed).release(); }
catch (...)
	{ return nullptr; }

void freeRanlux32(void *const state)
	{ std::unique_ptr<ranlux32_t> ranluxState{static_cast<ranlux32_t *>(state)}; }

uint32_t genRanlux32(void *const state)
{
	ranlux32_t &ranluxState = *static_cast<ranlux32_t *>(state);
	return ranluxState();
}
