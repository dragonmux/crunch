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

#ifndef __CRUNCHpp_H__
#define __CRUNCHpp_H__

#include <stddef.h>
#include <stdint.h>
#include <thread>
#include <vector>
#include <type_traits>
#include <typeinfo>
#include <functional>
#include <memory>
#include <exception>

#ifdef _MSC_VER
	#ifdef __crunch_lib__
		#define CRUNCH_IMP
		#define CRUNCH_VIS	__declspec(dllexport)
	#else
		#define CRUNCH_IMP	extern
		#define CRUNCH_VIS	__declspec(dllimport)
	#endif
	#ifdef __cplusplus
		#define CRUNCH_API	extern "C" CRUNCH_VIS
	#else
		#define CRUNCH_API	extern CRUNCH_VIS
	#endif
	#define CRUNCH_EXPORT		__declspec(dllexport)

	#ifdef stdout
	#undef stdout
	CRUNCH_API FILE *stdout;
	#endif
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
#define CRUNCHpp_API	extern CRUNCH_VIS

class testsuit;

struct cxxTest
{
	std::function<void()> testFunc;
	const char *testName;
};

struct cxxTestClass
{
	testsuit *testClass;
	const char *testClassName;
};

struct cxxUnitTest
{
	std::thread testThread;
	cxxTest theTest;
};

#ifdef _MSC_VER
CRUNCH_IMP template class CRUNCH_VIS std::allocator<cxxTest>;
CRUNCH_IMP template class CRUNCH_VIS std::vector<cxxTest>;
CRUNCH_IMP template class CRUNCH_VIS std::vector<std::exception_ptr>;
#endif

namespace crunch
{
	template<typename T> struct isBoolean : std::false_type { };
	template<> struct isBoolean<bool> : std::true_type { };

	template<typename T> struct isNumeric : std::integral_constant<bool, std::is_integral<T>::value && !isBoolean<T>::value> { };
}

class CRUNCH_VIS testsuit
{
private:
	template<typename T> using isNumeric = crunch::isNumeric<T>;
	std::vector<std::exception_ptr> exceptions;

protected:
	std::vector<cxxTest> tests;

	// XXX: Fixme.. this had to be added for some tests chained from a main test class
public:
	void fail(const char *const reason);
	void skip(const char *const reason);

	void assertTrue(const bool value);
	void assertFalse(const bool value);

	void assertEqual(const int8_t result, const int8_t expected);
	void assertEqual(const uint8_t result, const uint8_t expected);
	void assertEqual(const int16_t result, const int16_t expected);
	void assertEqual(const uint16_t result, const uint16_t expected);
	void assertEqual(const int32_t result, const int32_t expected);
	void assertEqual(const uint32_t result, const uint32_t expected);
	void assertEqual(const int64_t result, const int64_t expected);
	void assertEqual(const uint64_t result, const uint64_t expected);
	void assertEqual(void *result, void *expected);
	void assertEqual(double result, double expected);
	void assertEqual(const char *const result, const char *const expected);
	void assertEqual(const void *const result, const void *const expected, const size_t expectedLength);
	template<typename T, typename U, typename = typename std::enable_if<isNumeric<T>::value && isNumeric<U>::value && !std::is_same<T, U>::value>::type>
		void assertEqual(const T a, const U b) { assertEqual(a, T(b)); }

	void assertNotEqual(const int8_t result, const int8_t expected);
	void assertNotEqual(const uint8_t result, const uint8_t expected);
	void assertNotEqual(const int16_t result, const int16_t expected);
	void assertNotEqual(const uint16_t result, const uint16_t expected);
	void assertNotEqual(const int32_t result, const int32_t expected);
	void assertNotEqual(const uint32_t result, const uint32_t expected);
	void assertNotEqual(const int64_t result, const int64_t expected);
	void assertNotEqual(const uint64_t result, const uint64_t expected);
	void assertNotEqual(void *result, void *expected);
	void assertNotEqual(double result, double expected);
	void assertNotEqual(const char *const result, const char *const expected);
	void assertNotEqual(const void *const result, const void *const expected, const size_t expectedLength);
	template<typename T, typename U, typename = typename std::enable_if<isNumeric<T>::value && isNumeric<U>::value && !std::is_same<T, U>::value>::type>
		void assertNotEqual(const T a, const U b) { assertNotEqual(a, T(b)); }

	void assertNull(void *result);
	void assertNotNull(void *result);
	void assertNull(const void *const result);
	void assertNotNull(const void *const result);
	template<typename T> void assertNull(std::unique_ptr<T> &result) { assertNull(result.get()); }
	template<typename T> void assertNotNull(std::unique_ptr<T> &result) { assertNotNull(result.get()); }
	template<typename T> void assertNull(const std::unique_ptr<T> &result) { assertNull(result.get()); }
	template<typename T> void assertNotNull(const std::unique_ptr<T> &result) { assertNotNull(result.get()); }

	void assertGreaterThan(const long result, const long expected);
	void assertLessThan(const long result, const long expected);

	testsuit();

private:
	static int testRunner(testsuit &unitClass, cxxUnitTest &test);

public:
	virtual ~testsuit();
	virtual void registerTests() = 0;
	void test();
};

struct threadExit_t
{
private:
	int value;

public:
	threadExit_t(int exitValue) noexcept : value(exitValue) { }

	operator int() const noexcept
	{
		return value;
	}
};

CRUNCHpp_API std::vector<cxxTestClass> cxxTests;

template<typename TestClass> void registerTestClasses()
{
	cxxTestClass testClass = {new TestClass(), typeid(TestClass).name()};
	cxxTests.push_back(testClass);
}

template<typename TestClass, typename ...TestClasses>
typename std::enable_if<sizeof...(TestClasses) != 0, void>::type registerTestClasses()
{
	registerTestClasses<TestClass>();
	registerTestClasses<TestClasses...>();
}

#define CXX_TEST(name) \
{ \
	cxxTest test = {nullptr, #name}; \
	test.testFunc = [this](){ this->name(); }; \
	tests.push_back(test); \
}

typedef struct testLog
{
	FILE *file;
	int fd, stdout;
} testLog;

CRUNCHpp_API testLog *startLogging(const char *fileName);
CRUNCHpp_API void stopLogging(testLog *logFile);

/* Give systems that don't have other calling conventions a dud definition of __cdecl */
#ifndef _WINDOWS
#define __cdecl
#else
#ifdef __GNUC__
#define __cdecl __attribute__((cdecl))
#endif
#endif

#endif /*__CRUNCHpp_H__*/
