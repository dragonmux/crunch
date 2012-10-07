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
#ifndef __cdecl
#define __cdecl
#endif

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

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

/* The following two definitions are exactly and precicely what we mean by "boolean" values */
#define TRUE	1
#define FALSE	0

#include <inttypes.h>

TEST_API void fail(const char *reason);

TEST_API void assertTrue(uint8_t value);
TEST_API void assertFalse(uint8_t value);

TEST_API void assertIntEqual(int result, int expected);
TEST_API void assertIntNotEqual(int result, int expected);


TEST_API test *tests;

#endif /* __LIBTEST_H__ */
