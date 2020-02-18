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

// RANLUX32 w/ a Luxury value of 389
// word size (w) = 32, short lag (s) = 3, long lag (r) = 17
ranlux32_t *initRanlux32(const uint32_t seed);
void freeRanlux32(ranlux32_t *const state);
uint32_t genRanlux32(ranlux32_t *const state);

// RANLUX64
extern void initRanlux64(const uint16_t luxury, const uint64_t seed);
extern void freeRanlux64(void *const state);
extern uint64_t genRanlux64();

#ifdef __cplusplus
}
#endif

#endif /*RANLUX__H*/
