#ifndef __INTTYPES_H__
#ifndef _WINDOWS
#include <inttypes.h>
#define __INTTYPES_H__
#else
#define __INTTYPES_H__

/*
 * These give an approximated try at being compatible with Linux|Mac's inttypes
 * but they may be wrong for certain target CPUs which is why Windows not having
 * the header is such bad news
 */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#if defined(_MSC_VER) && _MSC_VER <= 1200
typedef unsigned __int64 uint64_t;
#else
typedef unsigned long long uint64_t;
#endif

/*
 * There are several libraries that do a similar job as this header via #if sections
 * but they define nothing to say these types are already defined, etc, and they
 * don't ensure signed-ness of the following types which has in the past caused problems
 * so there are some exceptions added here to when these are or are not defined.
 * Use the other definitions at your own risk of the /J flag.
 */

#if !defined(MP4V2_MP4V2_H) && !defined(WAVPACK_H)
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
#if defined(_MSC_VER) && _MSC_VER <= 1200
typedef signed __int64 int64_t;
#else
typedef signed long long int64_t;
#endif
#endif

#endif /*_WINDOWS*/
#endif /*__INTTYPES_H__*/