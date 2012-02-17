#ifndef __LIBTEST_H__
#define __LIBTEST_H__

#ifdef _MSC_VER
	#ifdef __libTest__
		#define TEST_API	__declspec(dllexport)
	#else
		#define TEST_API	__declspec(dllimport)
	#endif
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
#endif

#define TEST(name) \
	void name()

TEST_API void assertTrue(uint8_t value);
TEST_API void assertFalse(uint8_t value);

#endif /* __LIBTEST_H__ */
