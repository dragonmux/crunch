#ifndef RANLUX__H
#define RANLUX__H

#ifdef __cplusplus
#include <cstdint>
extern "C"
{
#else
#include <stdint.h>
#endif

// Implemented based on https://github.com/boostorg/random/issues/57

// RANLUX32 w/ a Luxury value of 389
// word size (w) = 32, short lag (s) = 3, long lag (r) = 17
extern void *initRanlux32(const uint32_t seed);
extern void freeRanlux32(void *const state);
extern uint32_t genRanlux32(void *const state);

// RANLUX64
extern void initRanlux64(const uint16_t luxury, const uint64_t seed);
extern uint64_t genRanlux64();

#ifdef __cplusplus
}
#endif

#endif /*RANLUX__H*/
