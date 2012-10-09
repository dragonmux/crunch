#ifndef __LIBTEST_H__
#define __LIBTEST_H__

#include <stddef.h>

#ifdef _MSC_VER
	#ifdef __libTest__
		#define TEST_API	__declspec(dllexport)
	#else
		#define TEST_API	__declspec(dllimport)
	#endif
	#define TEST_EXPORT		__declspec(dllexport)
#else
	#if __GNUC__ >= 4
		#define DEFAULT_VISIBILITY __attribute__ ((visibility("default")))
	#else
		#define DEFAULT_VISIBILITY
	#endif
	#ifdef __cplusplus
		#define TEST_API	extern "C" DEFAULT_VISIBILITY
	#else
		#define TEST_API	extern DEFAULT_VISIBILITY
	#endif
	#define TEST_EXPORT		TEST_API
#endif

/* Give systems that don't have other calling conventions a dud definition of __cdecl */
#ifndef _WINDOWS
#define __cdecl
#else
#ifdef __GNUC__
#define __cdecl __attribute__((cdecl))
#endif
#endif

#include <inttypes.h>
#include <stdio.h>

typedef struct _test
{
	void (__cdecl *testFunc)();
	const char *testName;
} test;

#define BEGIN_REGISTER_TESTS() \
TEST_EXPORT void registerTests() \
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

typedef struct _log
{
	FILE *file;
	int fd, stdout;
} log;

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

/* The following two definitions are exactly and precicely what we mean by "boolean" values */
#define TRUE	1
#define FALSE	0

TEST_API void fail(const char *reason);

TEST_API void assertTrue(uint8_t value);
TEST_API void assertFalse(uint8_t value);

TEST_API void assertIntEqual(int result, int expected);
TEST_API void assertPtrEqual(void *result, void *expected);
TEST_API void assertDoubleEqual(double result, double expected);
TEST_API void assertStringEqual(const char *result, const char *expected);

TEST_API void assertIntNotEqual(int result, int expected);
TEST_API void assertPtrNotEqual(void *result, void *expected);
TEST_API void assertDoubleNotEqual(double result, double expected);
TEST_API void assertStringNotEqual(const char *result, const char *expected);

TEST_API void assertNull(void *result);
TEST_API void assertNotNull(void *result);
TEST_API void assertConstNull(const void *result);
TEST_API void assertConstNotNull(const void *result);

TEST_API void assertGreaterThan(long result, long expected);
TEST_API void assertLessThan(long result, long expected);

TEST_API test *tests;

TEST_API log *startLogging(const char *fileName);
TEST_API void stopLogging(log *logFile);

#endif /* __LIBTEST_H__ */
