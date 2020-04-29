// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstring>
#include <random>
#include "ranlux.h"

uint32_t randomSeed32()
{
	std::random_device randDev{};
	return randDev();
}

uint64_t randomSeed64()
{
	std::random_device randDev{};
	if (sizeof(std::random_device::result_type) >= 64)
		return randDev();
	else
		return (uint64_t{randDev()} << 32U) | randDev();
}

double genDouble(ranlux64_t *generator)
{
	// This masks off the most significant bit of the exponent so
	// we can never end up with the infinities, or any kind of NaN.
	const uint64_t value = genRanlux64(generator) & ~0x4000000000000000U;
	double result = 0.0;
	static_assert(sizeof(double) <= sizeof(uint64_t), "double is too big relative to uint64_t");
	memcpy(&result, &value, sizeof(double));
	return result;
}
