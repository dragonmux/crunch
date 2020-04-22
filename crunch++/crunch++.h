/*
 * This file is part of crunch
 * Copyright © 2013-2020 Rachel Mant (dx-mon@users.sourceforge.net)
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

#ifndef CRUNCHpp__H
#define CRUNCHpp__H

#ifndef __cplusplus
#error "This is the C++ test harness header, use crunch.h for C"
#endif

#include <cstddef>
#include <cstdint>
#include <thread>
#include <vector>
#include <type_traits>
#include <typeinfo>
#include <functional>
#include <memory>
#include <exception>

#ifdef _MSC_VER
#	ifdef __crunch_lib__
#		define CRUNCH_VIS	__declspec(dllexport)
#	else
#		define CRUNCH_VIS	__declspec(dllimport)
#	endif
#	define CRUNCH_API	extern "C" CRUNCH_VIS
#	define CRUNCH_EXPORT		__declspec(dllexport)
#	define CRUNCH_MAYBE_VIS
#	define CRUNCHpp_TEST	extern "C" CRUNCH_EXPORT
#else
#	if __GNUC__ >= 4
#		define CRUNCH_VIS __attribute__ ((visibility("default")))
#	else
#		error "Your GCC is too old to use crunch++"
#	endif
#	define CRUNCH_API	extern "C" CRUNCH_VIS
#	define CRUNCH_EXPORT		CRUNCH_API
#	define CRUNCH_MAYBE_VIS	CRUNCH_VIS
#	define CRUNCHpp_TEST		CRUNCH_API
#endif
#define CRUNCHpp_API	extern CRUNCH_VIS

#if __cplusplus >= 201402L
#	define CRUNCH_DEPRECATE [[deprecated]]
#else
#	ifdef _WINDOWS
#		define CRUNCH_DEPRECATE __declspec(deprecated)
#	else
#		define CRUNCH_DEPRECATE __attribute__ ((deprecated))
#	endif
#endif

class testsuite;

struct cxxTest
{
	std::function<void()> testFunc;
	const char *testName;
};

struct cxxUnitTest
{
	std::thread testThread;
	cxxTest theTest;
};

namespace crunch
{
	template<typename T> struct isBoolean : std::false_type { };
	template<> struct isBoolean<bool> : std::true_type { };

	template<typename T> struct isNumeric : std::integral_constant<bool, std::is_integral<T>::value && !isBoolean<T>::value> { };
}

class CRUNCH_MAYBE_VIS testsuite
{
private:
	template<typename T> using isNumeric = crunch::isNumeric<T>;
	std::vector<std::exception_ptr> exceptions;

protected:
	std::vector<cxxTest> tests;

	// XXX: Fixme.. this had to be added for some tests chained from a main test class
public:
	CRUNCH_VIS void fail(const char *const reason);
	CRUNCH_VIS void skip(const char *const reason);

	CRUNCH_VIS void assertTrue(const bool value);
	CRUNCH_VIS void assertFalse(const bool value);

	CRUNCH_VIS void assertEqual(const int8_t result, const int8_t expected);
	CRUNCH_VIS void assertEqual(const uint8_t result, const uint8_t expected);
	CRUNCH_VIS void assertEqual(const int16_t result, const int16_t expected);
	CRUNCH_VIS void assertEqual(const uint16_t result, const uint16_t expected);
	CRUNCH_VIS void assertEqual(const int32_t result, const int32_t expected);
	CRUNCH_VIS void assertEqual(const uint32_t result, const uint32_t expected);
	CRUNCH_VIS void assertEqual(const int64_t result, const int64_t expected);
	CRUNCH_VIS void assertEqual(const uint64_t result, const uint64_t expected);
	CRUNCH_VIS void assertEqual(void *result, void *expected);
	CRUNCH_VIS void assertEqual(double result, double expected);
	CRUNCH_VIS void assertEqual(const char *const result, const char *const expected);
	CRUNCH_VIS void assertEqual(const void *const result, const void *const expected, const size_t expectedLength);
	template<typename T, typename U, typename = typename std::enable_if<isNumeric<T>::value && isNumeric<U>::value && !std::is_same<T, U>::value>::type>
		void assertEqual(const T a, const U b) { assertEqual(a, T(b)); }

	CRUNCH_VIS void assertNotEqual(const int8_t result, const int8_t expected);
	CRUNCH_VIS void assertNotEqual(const uint8_t result, const uint8_t expected);
	CRUNCH_VIS void assertNotEqual(const int16_t result, const int16_t expected);
	CRUNCH_VIS void assertNotEqual(const uint16_t result, const uint16_t expected);
	CRUNCH_VIS void assertNotEqual(const int32_t result, const int32_t expected);
	CRUNCH_VIS void assertNotEqual(const uint32_t result, const uint32_t expected);
	CRUNCH_VIS void assertNotEqual(const int64_t result, const int64_t expected);
	CRUNCH_VIS void assertNotEqual(const uint64_t result, const uint64_t expected);
	CRUNCH_VIS void assertNotEqual(void *result, void *expected);
	CRUNCH_VIS void assertNotEqual(double result, double expected);
	CRUNCH_VIS void assertNotEqual(const char *const result, const char *const expected);
	CRUNCH_VIS void assertNotEqual(const void *const result, const void *const expected, const size_t expectedLength);
	template<typename T, typename U, typename = typename std::enable_if<isNumeric<T>::value && isNumeric<U>::value && !std::is_same<T, U>::value>::type>
		void assertNotEqual(const T a, const U b) { assertNotEqual(a, T(b)); }

	CRUNCH_VIS void assertNull(void *result);
	CRUNCH_VIS void assertNotNull(void *result);
	CRUNCH_VIS void assertNull(const void *const result);
	CRUNCH_VIS void assertNotNull(const void *const result);
	template<typename T> void assertNull(std::unique_ptr<T> &result) { assertNull(result.get()); }
	template<typename T> void assertNotNull(std::unique_ptr<T> &result) { assertNotNull(result.get()); }
	template<typename T> void assertNull(const std::unique_ptr<T> &result) { assertNull(result.get()); }
	template<typename T> void assertNotNull(const std::unique_ptr<T> &result) { assertNotNull(result.get()); }

	CRUNCH_VIS void assertGreaterThan(const long result, const long expected);
	CRUNCH_VIS void assertLessThan(const long result, const long expected);

	CRUNCH_VIS testsuite();

private:
	static int testRunner(testsuite &unitClass, cxxUnitTest &test);

public:
	testsuite(const testsuite &) = delete;
	testsuite(testsuite &&) = delete;
	CRUNCH_VIS virtual ~testsuite();
	testsuite &operator =(const testsuite &) = delete;
	testsuite &operator =(testsuite &&) = delete;

	virtual void registerTests() = 0;
	CRUNCH_VIS void test();
};

class CRUNCH_DEPRECATE testsuit : public testsuite { };

struct threadExit_t
{
private:
	int value;

public:
	threadExit_t(int exitValue) noexcept : value(exitValue) { }
	CRUNCH_VIS operator int() const noexcept { return value; }
};

CRUNCHpp_API void crunchTestClass(std::unique_ptr<testsuite> &&tests, const char *name);

namespace crunchpp
{
	template<typename T> using remove_const_t = typename std::remove_const<T>::type;
	template<typename T> using remove_extent_t = typename std::remove_extent<T>::type;

	template<typename T> struct makeUnique_t { using uniqueType = std::unique_ptr<T>; };
	template<typename T> struct makeUnique_t<T []> { using arrayType = std::unique_ptr<T []>; };
	template<typename T, size_t N> struct makeUnique_t<T [N]> { struct invalidType { }; };

	template<typename T, typename... args_t> inline typename makeUnique_t<T>::uniqueType
		makeUnique(args_t &&...args)
	{
		using type_t = remove_const_t<T>;
		return std::unique_ptr<T>{new type_t{std::forward<args_t>(args)...}};
	}

	template<typename T> inline typename makeUnique_t<T>::arrayType
		makeUnique(const size_t num)
	{
		using type_t = remove_const_t<remove_extent_t<T>>;
		return std::unique_ptr<T>{new type_t[num]{{}}};
	}

	template<typename T, typename... args_t> inline typename makeUnique_t<T>::invalidType
		makeUnique(args_t &&...) noexcept = delete;
}

template<typename TestClass> void registerTestClasses()
	{ crunchTestClass(crunchpp::makeUnique<TestClass>(), typeid(TestClass).name()); }

template<typename TestClass, typename ...TestClasses>
typename std::enable_if<sizeof...(TestClasses) != 0, void>::type registerTestClasses()
{
	registerTestClasses<TestClass>();
	registerTestClasses<TestClasses...>();
}

#define CXX_TEST(name) tests.emplace_back(cxxTest{[this](){ this->name(); }, #name});

struct testLog;

CRUNCHpp_API testLog *startLogging(const char *fileName);
CRUNCHpp_API void stopLogging(testLog *logger);

/* Give systems that don't have other calling conventions a dud definition of __cdecl */
#ifndef _WINDOWS
#define __cdecl
#else
#ifdef __GNUC__
#define __cdecl __attribute__((cdecl))
#endif
#endif

#endif /*CRUNCHpp__H*/
