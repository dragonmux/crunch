// SPDX-License-Identifier: LGPL-3.0-or-later
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
#include <stdexcept>
#include <string>
#if __cplusplus >= 201703L
#include <string_view>
#endif

#ifdef _WIN32
#	ifdef __crunch_lib__
#		define CRUNCH_VIS	__declspec(dllexport)
#	else
#		define CRUNCH_VIS	__declspec(dllimport)
#	endif
#	define CRUNCH_API extern "C" CRUNCH_VIS
#	define CRUNCHpp_EXPORT extern "C" __declspec(dllexport)
#	define CRUNCH_MAYBE_VIS
#else
#	if __GNUC__ >= 4 || defined(__APPLE__)
#		define CRUNCH_VIS __attribute__ ((visibility("default")))
#	else
#		error "Your GCC is too old to use crunch++"
#	endif
#	define CRUNCH_API	extern "C" CRUNCH_VIS
#	define CRUNCHpp_EXPORT		CRUNCH_API
#	define CRUNCH_MAYBE_VIS	CRUNCH_VIS
#endif
#define CRUNCHpp_API	extern CRUNCH_VIS

#if defined(__has_cpp_attribute)
#	define CRUNCH_HAS_CPP_ATTRIBUTE(attr) __has_cpp_attribute(attr)
#else
#	define CRUNCH_HAS_CPP_ATTRIBUTE(attr) 1
#endif

#if __cplusplus >= 201402L && CRUNCH_HAS_CPP_ATTRIBUTE(deprecated)
#	define CRUNCH_DEPRECATE [[deprecated]]
#	define CRUNCH_CXX14_CONSTEXPR constexpr
#	define CRUNCH_CXX14_CONSTEXPR_INLINE CRUNCH_CXX14_CONSTEXPR
#else
#	ifdef _WINDOWS
#		define CRUNCH_DEPRECATE __declspec(deprecated)
#	else
#		define CRUNCH_DEPRECATE __attribute__ ((deprecated))
#	endif
#	define CRUNCH_CXX14_CONSTEXPR
#	define CRUNCH_CXX14_CONSTEXPR_INLINE inline
#endif

#if __cplusplus >= 201703L && CRUNCH_HAS_CPP_ATTRIBUTE(nodiscard)
#	define CRUNCH_NO_DISCARD(x) [[nodiscard]] x
#elif defined(__GNUC__)
#	define CRUNCH_NO_DISCARD(x) x __attribute__((warn_unused_result))
#else
#	define CRUNCH_NO_DISCARD(x) x
#endif

#ifndef __APPLE__
#define CRUNCHpp_MAYBE_NOEXCEPT(x) x noexcept
#else
#define CRUNCHpp_MAYBE_NOEXCEPT(x) x
#endif

namespace crunch
{
	namespace internal
	{
		struct cxxTest;

		template<typename T> struct isBoolean : std::false_type { };
		template<> struct isBoolean<bool> : std::true_type { };

		template<typename T> struct isNumeric : std::integral_constant<bool,
			std::is_integral<T>::value && !isBoolean<T>::value> { };

		template<typename T, typename U> struct areDifferentIntegers : std::integral_constant<bool,
			isNumeric<T>::value && isNumeric<U>::value && !std::is_same<T, U>::value> { };

		template<bool B, typename T = void> using enableIf = typename std::enable_if<B, T>::type;

		struct stringView final
		{
		private:
			std::size_t length_{0};
			const char *data_{nullptr};

			std::size_t check_(const std::size_t pos) const
			{
				if (pos > length_ && length_)
					throw std::out_of_range{"crunch::internal::stringView: pos outside view"};
				else if (!length_)
					return length_;
				return pos;
			}

			constexpr std::size_t clamp_(const std::size_t offset, const std::size_t length) const
				{ return length > length_ - offset ? length_ - offset : length; }

			CRUNCH_CXX14_CONSTEXPR static const char *find_(const char *const str, const std::size_t len,
				const char c) noexcept
			{
				for (std::size_t i{0}; i < len; ++i)
				{
					if (str[i] == c)
						return str + i;
				}
				return nullptr;
			}

			CRUNCH_CXX14_CONSTEXPR static std::ptrdiff_t compare_(const char *const s1,
				const char *const s2, const std::size_t len) noexcept
			{
				for (std::size_t i{0}; i < len; ++i)
				{
					if (s1[i] < s2[i])
						return -1;
					else if (s1[i] > s2[i])
						return 1;
				}
				return 0;
			}

		public:
			constexpr stringView() noexcept = default;
			constexpr stringView(const stringView &) noexcept = default;
			constexpr stringView(stringView &&) noexcept = default;
			constexpr explicit stringView(const char *const data, const std::size_t length) noexcept :
				length_{length}, data_{data} { }
			stringView(const std::string &str) noexcept : length_{str.length()}, data_{str.data()} { }
			~stringView() noexcept = default;
			stringView &operator =(const stringView &) noexcept = default;
			stringView &operator =(stringView &&) noexcept = default;
			CRUNCH_NO_DISCARD(constexpr const char *data() const noexcept) { return data_; }
			CRUNCH_NO_DISCARD(constexpr std::size_t size() const noexcept) { return length_; }
			CRUNCH_NO_DISCARD(constexpr std::size_t length() const noexcept) { return length_; }
			CRUNCH_NO_DISCARD(constexpr bool empty() const noexcept) { return size() == 0; }
			CRUNCH_NO_DISCARD(std::string toString() const noexcept) { return {data_, length_}; }
			CRUNCH_NO_DISCARD(constexpr const char *begin() const noexcept) { return data_; }
			CRUNCH_NO_DISCARD(constexpr const char *end() const noexcept) { return data_ + length_; }

			CRUNCH_NO_DISCARD(stringView substr(const std::size_t pos = 0, const std::size_t n = npos) const)
				{ return stringView{data_ + check_(pos), clamp_(pos, n)}; }

			CRUNCH_NO_DISCARD(CRUNCH_CXX14_CONSTEXPR std::size_t find(const char c,
				const std::size_t pos = 0) const noexcept)
			{
				if (pos < length_)
				{
					const auto length{length_ - pos};
					const auto *const result{find_(data_ + pos, length, c)};
					if (result)
						return result - data_;
				}
				return npos;
			}

			CRUNCH_NO_DISCARD(CRUNCH_CXX14_CONSTEXPR std::size_t rfind(const char c,
				const std::size_t pos = npos) const noexcept)
			{
				if (length_)
				{
					auto idx{length_ > pos ? pos + 1 : length_};
					while (idx-- > 0)
					{
						if (data_[idx] == c)
							return idx;
					}
				}
				return npos;
			}

			CRUNCH_NO_DISCARD(CRUNCH_CXX14_CONSTEXPR std::ptrdiff_t compare(const stringView &str) const noexcept)
			{
				if (size() == str.size())
					return compare_(data(), str.data(), size());
				return size() - str.size();
			}

			CRUNCH_NO_DISCARD(std::ptrdiff_t compare(const std::size_t pos, const std::size_t n,
				const stringView &str) const) { return substr(pos, n).compare(str); }

			CRUNCH_CXX14_CONSTEXPR void swap(stringView &other) noexcept
			{
				auto tmp{*this};
				*this = other;
				other = tmp;
			}

			constexpr static auto npos = static_cast<std::size_t>(-1);
		};

		CRUNCH_CXX14_CONSTEXPR_INLINE bool operator ==(const stringView &lhs, const stringView &rhs) noexcept
			{ return lhs.compare(rhs) == 0; }
	} // namespace internal

	inline namespace literals
	{
		constexpr inline crunch::internal::stringView operator ""_sv(const char *const str,
			std::size_t len) noexcept { return crunch::internal::stringView{str, len}; }
	}
} // namespace crunch

class CRUNCH_MAYBE_VIS testsuite
{
private:
	template<typename T, typename U> using areDifferentIntegers = crunch::internal::areDifferentIntegers<T, U>;
	template<bool B, typename T = void> using enableIf = crunch::internal::enableIf<B, T>;
	using stringView = crunch::internal::stringView;

	std::vector<std::exception_ptr> exceptions;
	std::vector<crunch::internal::cxxTest> tests;

protected:
	CRUNCH_VIS bool registerTest(std::function<void ()> &&func, const char *const name);

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
#if defined(__APPLE__)
	CRUNCH_VIS void assertEqual(const std::size_t result, const std::size_t expected);
#endif
	CRUNCH_VIS void assertEqual(void *result, void *expected);
	CRUNCH_VIS void assertEqual(double result, double expected);
	CRUNCH_VIS void assertEqual(const char *const result, const char *const expected);
	CRUNCH_VIS void assertEqual(const void *const result, const void *const expected, const size_t expectedLength);

	template<typename T, typename U, typename = enableIf<areDifferentIntegers<T, U>::value>>
		void assertEqual(const T a, const U b) { assertEqual(a, T(b)); }

	template<typename T, typename = enableIf<std::is_enum<T>::value>>
		void assertEqual(const T a, const T b)
	{
		using U = typename std::underlying_type<T>::type;
		assertEqual(U(a), U(b));
	}

	void assertEqual(const std::string &result, const std::string &expected)
		{ assertEqual(stringView{result.c_str(), result.length()}, stringView{expected.c_str(), expected.length()}); }
#if __cplusplus >= 201703L
	void assertEqual(const std::string_view &result, const std::string_view &expected)
		{ assertEqual(stringView{result.data(), result.length()}, stringView{expected.data(), expected.length()}); }
#endif

	CRUNCH_VIS void assertNotEqual(const int8_t result, const int8_t expected);
	CRUNCH_VIS void assertNotEqual(const uint8_t result, const uint8_t expected);
	CRUNCH_VIS void assertNotEqual(const int16_t result, const int16_t expected);
	CRUNCH_VIS void assertNotEqual(const uint16_t result, const uint16_t expected);
	CRUNCH_VIS void assertNotEqual(const int32_t result, const int32_t expected);
	CRUNCH_VIS void assertNotEqual(const uint32_t result, const uint32_t expected);
	CRUNCH_VIS void assertNotEqual(const int64_t result, const int64_t expected);
	CRUNCH_VIS void assertNotEqual(const uint64_t result, const uint64_t expected);
#if defined(__APPLE__)
	CRUNCH_VIS void assertNotEqual(const std::size_t result, const std::size_t expected);
#endif
	CRUNCH_VIS void assertNotEqual(void *result, void *expected);
	CRUNCH_VIS void assertNotEqual(double result, double expected);
	CRUNCH_VIS void assertNotEqual(const char *const result, const char *const expected);
	CRUNCH_VIS void assertNotEqual(const void *const result, const void *const expected, const size_t expectedLength);

	template<typename T, typename U, typename = enableIf<areDifferentIntegers<T, U>::value>>
		void assertNotEqual(const T a, const U b) { assertNotEqual(a, T(b)); }

	template<typename T, typename = enableIf<std::is_enum<T>::value>>
		void assertNotEqual(const T a, const T b)
	{
		using U = typename std::underlying_type<T>::type;
		assertNotEqual(U(a), U(b));
	}

	void assertNotEqual(const std::string &result, const std::string &expected)
		{ assertNotEqual(stringView{result.c_str(), result.length()}, stringView{expected.c_str(), expected.length()}); }
#if __cplusplus >= 201703L
	void assertNotEqual(const std::string_view &result, const std::string_view &expected)
		{ assertNotEqual(stringView{result.data(), result.length()}, stringView{expected.data(), expected.length()}); }
#endif

	CRUNCH_VIS void assertNull(const void *const result);
	CRUNCH_VIS void assertNotNull(const void *const result);
	template<typename T> void assertNull(std::unique_ptr<T> &result) { assertNull(result.get()); }
	template<typename T> void assertNotNull(std::unique_ptr<T> &result) { assertNotNull(result.get()); }
	template<typename T> void assertNull(const std::unique_ptr<T> &result) { assertNull(result.get()); }
	template<typename T> void assertNotNull(const std::unique_ptr<T> &result) { assertNotNull(result.get()); }

	CRUNCH_VIS void assertGreaterThan(const long result, const long expected);
	CRUNCH_VIS void assertLessThan(const long result, const long expected);

	CRUNCH_VIS testsuite() noexcept;

private:
	static int32_t testRunner(testsuite &unitClass, crunch::internal::cxxTest &test);
	CRUNCH_VIS void assertEqual(const stringView result, const stringView expected);
	CRUNCH_VIS void assertNotEqual(const stringView result, const stringView expected);

public:
	testsuite(const testsuite &) = delete;
	testsuite(testsuite &&) = delete;
	CRUNCH_VIS virtual ~testsuite() noexcept;
	testsuite &operator =(const testsuite &) = delete;
	testsuite &operator =(testsuite &&) = delete;

	virtual void registerTests() = 0;
	CRUNCH_VIS void test();
};

class CRUNCH_DEPRECATE testsuit : public testsuite { };

struct threadExit_t final : std::exception
{
private:
	int value;

public:
	threadExit_t(int exitValue) noexcept : std::exception{}, value{exitValue} { }
	operator int() const noexcept { return value; }
	const char *what() const noexcept final { return "Test assertion failure, thread exiting"; }
};

namespace crunch
{
	namespace internal
	{
		struct CRUNCH_MAYBE_VIS cxxTest
		{
		private:
			std::function<void ()> testFunc{nullptr};
			const char *testName{nullptr};

		public:
			// clang 5 has a bad time with this if we don't define it this way.
			cxxTest() noexcept { } // NOLINT(modernize-use-equals-default, hicpp-use-equals-default)
			CRUNCH_VIS cxxTest(std::function<void ()> &&func, const char *const name) noexcept;
			cxxTest(const cxxTest &) = default;
			cxxTest(cxxTest &&) = default;
			~cxxTest() noexcept = default;
			cxxTest &operator =(const cxxTest &) = default;
			cxxTest &operator =(cxxTest &&) = default;

			const char *name() const noexcept { return testName; }
			const std::function<void ()> &function() const noexcept { return testFunc; }

			void swap(cxxTest &other) noexcept
			{
				auto tmp{*this};
				*this = other;
				other = tmp;
			}
		};

		CRUNCHpp_API void registerTestClass(std::unique_ptr<testsuite> &&suite, const char *name);
	} // namespace internal

	template<typename T> using remove_const_t = typename std::remove_const<T>::type;
	template<typename T> using remove_extent_t = typename std::remove_extent<T>::type;

	template<typename T> struct makeUnique_t { using uniqueType = std::unique_ptr<T>; }; // NOLINT
	template<typename T> struct makeUnique_t<T []> { using arrayType = std::unique_ptr<T []>; }; // NOLINT
	template<typename T, size_t N> struct makeUnique_t<T [N]> { struct invalidType { }; }; // NOLINT

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
} // namespace crunch

template<typename TestClass> void registerTestClasses()
	{ crunch::internal::registerTestClass(crunch::makeUnique<TestClass>(), typeid(TestClass).name()); }

template<typename TestClass, typename ...TestClasses>
typename std::enable_if<sizeof...(TestClasses) != 0, void>::type registerTestClasses()
{
	registerTestClasses<TestClass>();
	registerTestClasses<TestClasses...>();
}

#define CRUNCHpp_TEST(name) registerTest([this](){ this->name(); }, #name);
#define CXX_TEST(name) CRUNCHpp_TEST(name)

#define CRUNCHpp_TESTS(...) \
CRUNCHpp_EXPORT void registerCXXTests(); \
void registerCXXTests() \
{ \
	registerTestClasses<__VA_ARGS__>(); \
}

namespace crunch { struct testLog; }

CRUNCHpp_API crunch::testLog *startLogging(const char *fileName);
CRUNCHpp_API void stopLogging(crunch::testLog *logger);

#endif /*CRUNCHpp__H*/
