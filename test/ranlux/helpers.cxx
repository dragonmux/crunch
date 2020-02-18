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
		return (uint64_t{randDev()} << 32) | randDev();
}
