/*
 * This file is part of crunch
 * Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
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

#ifndef __CRUNCH_H__
#define __CRUNCH_H__

#include <stddef.h>

#ifdef _MSC_VER
	#ifdef __crunch_lib__
		#define CRUNCH_VIS	__declspec(dllexport)
	#else
		#define CRUNCH_VIS	__declspec(dllimport)
	#endif
	#ifdef __cplusplus
		#define CRUNCH_API	extern "C" CRUNCH_VIS
	#else
		#define CRUNCH_API	CRUNCH_VIS
	#endif
	#define CRUNCH_EXPORT		__declspec(dllexport)
#else
	#if __GNUC__ >= 4
		#define CRUNCH_VIS __attribute__ ((visibility("default")))
	#else
		#define CRUNCH_VIS
	#endif
	#ifdef __cplusplus
		#define CRUNCH_API	extern "C" CRUNCH_VIS
	#else
		#define CRUNCH_API	extern CRUNCH_VIS
	#endif
	#define CRUNCH_EXPORT		CRUNCH_API
#endif

/* Give systems that don't have other calling conventions a dud definition of __cdecl */
#ifndef _WINDOWS
#define __cdecl
#else
#ifdef __GNUC__
#define __cdecl __attribute__((cdecl))
#endif
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define __MACOS__
#endif

#include <inttypes.h>
#include <stdio.h>

typedef struct _test
{
	void (__cdecl *testFunc)();
	const char *testName;
} test;

#define BEGIN_REGISTER_TESTS() \
CRUNCH_EXPORT void registerTests() \
{ \
	static const test __tests[] = \
	{ \

#define TEST(name) \
	{ name, #name },

#define END_REGISTER_TESTS() \
		{ NULL } \
	}; \
	tests = (test *)__tests; \
}

typedef struct testLog testLog;

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

/* The following two definitions are exactly and precicely what we mean by "boolean" values */
#define TRUE	1
#define FALSE	0

CRUNCH_API void fail(const char *reason);

CRUNCH_API void assertTrue(uint8_t value);
CRUNCH_API void assertFalse(uint8_t value);

CRUNCH_API void assertIntEqual(int32_t result, int32_t expected);
CRUNCH_API void assertUintEqual(uint32_t result, uint32_t expected);
CRUNCH_API void assertInt64Equal(int64_t result, int64_t expected);
CRUNCH_API void assertUint64Equal(uint64_t result, uint64_t expected);
CRUNCH_API void assertPtrEqual(void *result, void *expected);
CRUNCH_API void assertDoubleEqual(double result, double expected);
CRUNCH_API void assertStringEqual(const char *result, const char *expected);
CRUNCH_API void assertMemEqual(const void *result, const void *expected, const size_t expectedLength);

CRUNCH_API void assertIntNotEqual(int32_t result, int32_t expected);
CRUNCH_API void assertUintNotEqual(uint32_t result, uint32_t expected);
CRUNCH_API void assertInt64NotEqual(int64_t result, int64_t expected);
CRUNCH_API void assertUint64NotEqual(uint64_t result, uint64_t expected);
CRUNCH_API void assertPtrNotEqual(void *result, void *expected);
CRUNCH_API void assertDoubleNotEqual(double result, double expected);
CRUNCH_API void assertStringNotEqual(const char *result, const char *expected);
CRUNCH_API void assertMemNotEqual(const void *result, const void *expected, const size_t expectedLength);

CRUNCH_API void assertNull(void *result);
CRUNCH_API void assertNotNull(void *result);
CRUNCH_API void assertConstNull(const void *const result);
CRUNCH_API void assertConstNotNull(const void *const result);

CRUNCH_API void assertGreaterThan(long result, long expected);
CRUNCH_API void assertLessThan(long result, long expected);

CRUNCH_API test *tests;
CRUNCH_API int32_t allocCount;

CRUNCH_API testLog *startLogging(const char *fileName);
CRUNCH_API void stopLogging(testLog *logFile);

#endif /* __CRUNCH_H__ */
