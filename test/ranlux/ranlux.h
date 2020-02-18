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

typedef struct ranlux32_t ranlux32_t;
typedef struct ranlux64_t ranlux64_t;

// Seed value readers
uint32_t randomSeed32();
uint64_t randomSeed64();

// RANLUX32 w/ a Luxury value of 389
// word size (w) = 32, short lag (s) = 3, long lag (r) = 17
ranlux32_t *initRanlux32(const uint32_t seed);
void freeRanlux32(ranlux32_t *const state);
uint32_t genRanlux32(ranlux32_t *const state);

// RANLUX64
// word size (w) = 64, short lag (s) = 5, long lag (r) = 62
ranlux64_t *initRanlux64(const uint64_t seed);
void freeRanlux64(ranlux64_t *const state);
uint64_t genRanlux64(ranlux64_t *const state);

#ifdef __cplusplus
}
#endif

#endif /*RANLUX__H*/
