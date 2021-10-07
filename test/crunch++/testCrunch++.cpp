// SPDX-License-Identifier: LGPL-3.0-or-later
#include <crunch++.h>
#include <cstdint>
#include <climits>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <memory>
#include <random>
#include <functional>
#include <core.hxx>
#include <stringFuncs.hxx>
#include <logger.hxx>

using std::default_random_engine;
using std::uniform_real_distribution;
using std::uniform_int_distribution;
using crunch::operator ""_sv;
using crunch::logResult;
using crunch::RESULT_ABORT;
using crunch::failures;

class crunchTests final : public testsuite
{
private:
	constexpr static auto testStr1{"abcdefghijklmnopqrstuvwxyz"_sv};
	constexpr static auto testStr2{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"_sv};

#if __cplusplus >= 201703L
	constexpr static std::string_view testStdStringView1{testStr1.data(), testStr1.length()};
	constexpr static std::string_view testStdStringView2{testStr2.data(), testStr2.length()};
#endif

	default_random_engine rngGen{std::random_device{}()};
	uniform_real_distribution<double> dblRng{-1.0, 1.0};
	uniform_int_distribution<int64_t> intRng{INT64_MIN, INT64_MAX};
	uniform_int_distribution<uint64_t> uintRng{0, UINT64_MAX};
	uniform_int_distribution<uintptr_t> ptrRng{0, UINTPTR_MAX};
	std::string testStdString1{testStr1.data()};
	std::string testStdString2{testStr2.data()};

	void tryShouldFail(const std::function<void()> &test)
	{
		try
			{ test(); }
		catch (threadExit_t &)
		{
			--failures;
			return;
		}
		fail("Expected threadExit_t exception not thrown");
	}

	inline void *genPtr()
	{
		return reinterpret_cast<void *>(ptrRng(rngGen)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
	}

public:
	crunchTests() = default;

private:
	void testAssertTrue()
	{
		assertTrue(true);
		tryShouldFail([this]() { assertTrue(false); });
	}

	void testAssertFalse()
	{
		assertFalse(false);
		tryShouldFail([this]() { assertFalse(true); });
	}

	void testAssertIntEqual()
	{
		auto num8{static_cast<int8_t>(intRng(rngGen))};
		auto num16{static_cast<int16_t>(intRng(rngGen))};
		auto num32{static_cast<int32_t>(intRng(rngGen))};
		auto num64{intRng(rngGen)};

		assertEqual(num8, num8);
		assertEqual(num16, num16);
		assertEqual(num32, num32);
		assertEqual(num64, num64);

		while (!num8)
			num8 = int8_t(intRng(rngGen));
		while (!num16)
			num16 = int16_t(intRng(rngGen));
		while (!num32)
			num32 = int32_t(intRng(rngGen));
		while (!num64)
			num64 = intRng(rngGen);

		tryShouldFail([=]() { assertEqual(int8_t{0}, num8); });
		tryShouldFail([=]() { assertEqual(int16_t{0}, num16); });
		tryShouldFail([=]() { assertEqual(int32_t{0}, num32); });
		tryShouldFail([=]() { assertEqual(int64_t{0}, num64); });
	}

	void testAssertIntNotEqual()
	{
		int8_t num8{};
		int16_t num16{};
		int32_t num32{};
		int64_t num64{};

		do
			num8 = int8_t(intRng(rngGen));
		while (!num8);
		do
			num16 = int16_t(intRng(rngGen));
		while (!num16);
		do
			num32 = int32_t(intRng(rngGen));
		while (!num32);
		do
			num64 = intRng(rngGen);
		while (!num64);

		assertNotEqual(num8, 0);
		assertNotEqual(num16, 0);
		assertNotEqual(num32, 0);
		assertNotEqual(num64, 0);

		tryShouldFail([=]() { assertNotEqual(num8, num8); });
		tryShouldFail([=]() { assertNotEqual(int8_t{0}, int8_t{0}); });
		tryShouldFail([=]() { assertNotEqual(num16, num16); });
		tryShouldFail([=]() { assertNotEqual(int16_t{0}, int16_t{0}); });
		tryShouldFail([=]() { assertNotEqual(num32, num32); });
		tryShouldFail([=]() { assertNotEqual(int32_t{0}, int32_t{0}); });
		tryShouldFail([=]() { assertNotEqual(num64, num64); });
		tryShouldFail([=]() { assertNotEqual(int64_t{0}, int64_t{0}); });
	}

	void testAssertUintEqual()
	{
		auto num8{static_cast<uint8_t>(uintRng(rngGen))};
		auto num16{static_cast<uint16_t>(uintRng(rngGen))};
		auto num32{static_cast<uint32_t>(uintRng(rngGen))};
		auto num64{uintRng(rngGen)};

		assertEqual(num8, num8);
		assertEqual(num16, num16);
		assertEqual(num32, num32);
		assertEqual(num64, num64);

		tryShouldFail([this]() { assertEqual(uint8_t{0}, uint8_t{1}); });
		tryShouldFail([this]() { assertEqual(uint16_t{0}, uint16_t{1}); });
		tryShouldFail([this]() { assertEqual(uint32_t{0}, uint32_t{1}); });
		tryShouldFail([this]() { assertEqual(uint64_t{0}, uint64_t{1}); });
	}

	void testAssertUintNotEqual()
	{
		uint8_t num8{};
		uint16_t num16{};
		uint32_t num32{};
		uint64_t num64{};

		do
			num8 = uint8_t(uintRng(rngGen));
		while (!num8);
		do
			num16 = uint16_t(uintRng(rngGen));
		while (!num16);
		do
			num32 = uint32_t(uintRng(rngGen));
		while (!num32);
		do
			num64 = uintRng(rngGen);
		while (!num64);

		assertNotEqual(num8, 0);
		assertNotEqual(num16, 0);
		assertNotEqual(num32, 0);
		assertNotEqual(num64, 0);

		tryShouldFail([=]() { assertNotEqual(num8, num8); });
		tryShouldFail([=]() { assertNotEqual(num16, num16); });
		tryShouldFail([=]() { assertNotEqual(num32, num32); });
		tryShouldFail([=]() { assertNotEqual(num64, num64); });
	}

	void testAssertDoubleEqual()
	{
		const auto num{dblRng(rngGen)};
		assertEqual(num, num);
		tryShouldFail([=]() { assertEqual(0.0, 0.1); });
	}

	void testAssertDoubleNotEqual()
	{
		const auto numA{dblRng(rngGen)};
		auto numB{dblRng(rngGen)};
		while (numA == numB)
			numB = dblRng(rngGen);
		assertNotEqual(numA, numB);
		tryShouldFail([=]() { assertNotEqual(numA, numA); });
		tryShouldFail([=]() { assertNotEqual(numB, numB); });
	}

	void testAssertPtrEqual()
	{
		auto *ptr{genPtr()};
		assertEqual(ptr, ptr);
		while (!ptr)
			ptr = genPtr();
		tryShouldFail([=]() { assertEqual(static_cast<void *>(nullptr), ptr); });
	}

	void testAssertPtrNotEqual()
	{
		void *ptr{nullptr};
		do
			ptr = genPtr();
		while (!ptr);
		assertNotEqual(ptr, nullptr);
		tryShouldFail([=]() { assertNotEqual(ptr, ptr); });
	}

	void testAssertStrEqual()
	{
		assertEqual(testStr1.data(), testStr1.data());
		assertEqual(testStr2.data(), testStr2.data());
		tryShouldFail([=]() { assertEqual(testStr1.data(), testStr2.data()); });
		assertEqual(testStdString1, testStdString1);
		assertEqual(testStdString2, testStdString2);
		tryShouldFail([=]() { assertEqual(testStdString1, testStdString2); });
#if __cplusplus >= 201703L
		assertEqual(testStdStringView1, testStdStringView1);
		assertEqual(testStdStringView2, testStdStringView2);
		tryShouldFail([=]() { assertEqual(testStdStringView1, testStdStringView2); });
#endif
	}

	void testAssertStrNotEqual()
	{
		assertNotEqual(testStr1.data(), testStr2.data());
		tryShouldFail([=]() { assertNotEqual(testStr1.data(), testStr1.data()); });
		tryShouldFail([=]() { assertNotEqual(testStr2.data(), testStr2.data()); });
		assertNotEqual(testStdString1, testStdString2);
		tryShouldFail([=]() { assertNotEqual(testStdString1, testStdString1); });
		tryShouldFail([=]() { assertNotEqual(testStdString2, testStdString2); });
#if __cplusplus >= 201703L
		assertNotEqual(testStdStringView1, testStdStringView2);
		tryShouldFail([=]() { assertNotEqual(testStdStringView1, testStdStringView1); });
		tryShouldFail([=]() { assertNotEqual(testStdStringView2, testStdStringView2); });
#endif
	}

	void testAssertMemEqual()
	{
		assertEqual(testStr1.data(), testStr1.data(), 27);
		assertEqual(testStr2.data(), testStr2.data(), 27);
		tryShouldFail([=]() { assertEqual(testStr1.data(), testStr2.data(), 27); });
	}

	void testAssertMemNotEqual()
	{
		assertNotEqual(testStr1.data(), testStr2.data(), 27);
		tryShouldFail([=]() { assertNotEqual(testStr1.data(), testStr1.data(), 27); });
		tryShouldFail([=]() { assertNotEqual(testStr2.data(), testStr2.data(), 27); });
	}

	void testAssertNull()
	{
		assertNull(nullptr);
		tryShouldFail([=]() { assertNull(testStr1.data()); });
		tryShouldFail([=]() { assertNull(testStr2.data()); });
	}

	void testAssertNotNull()
	{
		void *ptr{nullptr};
		do
			ptr = genPtr();
		while (!ptr);
		assertNotNull(ptr);
		tryShouldFail([=]() { assertNotNull(nullptr); });
	}

	void testAssertGreaterThan()
	{
#if LONG_MAX == INT32_MAX
		uint32_t value{};
		do
			value = uint32_t(uintRng(rngGen));
		while (!value);
		value &= ~(UINT32_C(1) << 31U);
		assertGreaterThan(value, 0);
		tryShouldFail([=]() { assertGreaterThan(value, value); });
		tryShouldFail([=]() { assertGreaterThan(0, value); });
#else
		uint64_t value{};
		do
			value = uintRng(rngGen);
		while (!value);
		value &= ~(UINT64_C(1) << 63U);
		assertGreaterThan(value, 0);
		tryShouldFail([=]() { assertGreaterThan(value, value); });
		tryShouldFail([=]() { assertGreaterThan(0, value); });
#endif
	}

	void testAssertLessThan()
	{
#if LONG_MAX == INT32_MAX
		uint32_t value{};
		do
			value = uint32_t(uintRng(rngGen));
		while (!value);
		value &= ~(UINT32_C(1) << 31U);
		assertLessThan(0, value);
		tryShouldFail([=]() { assertLessThan(value, value); });
		tryShouldFail([=]() { assertLessThan(value, 0); });
#else
		uint64_t value{};
		do
			value = uintRng(rngGen);
		while (!value);
		value &= ~(UINT64_C(1) << 63U);
		assertLessThan(0, value);
		tryShouldFail([=]() { assertLessThan(value, value); });
		tryShouldFail([=]() { assertLessThan(value, 0); });
#endif
	}

	void testLogging()
	{
		const auto fileName{"test.log"_s};
		const auto fileString{"Print to file test"_s};
		assertGreaterThan(puts("Print to console test"), -1);
		assertNull(startLogging(nullptr));
		assertNull(startLogging(""));
		auto *const logFile{startLogging(fileName.data())};
		assertNotNull(logFile);
		// Checks that trying to begin logging while already logging causes the framework to ignore the second request
		assertNull(startLogging(fileName.data()));
		stopLogging(nullptr); // code coverage stuff.. this shouldn't affect the next line.
		assertGreaterThan(puts(fileString.data()), -1);
		stopLogging(logFile);
		stopLogging(logFile); // code coverage stuff.. this should be harmless.
#ifndef _WINDOWS
		auto *const file{fopen(fileName.data(), "r")}; // NOLINT(cppcoreguidelines-owning-memory)
		assertNotNull(file);
		struct stat fileStat{};
		assertEqual(fstat(fileno(file), &fileStat), 0);
		assertGreaterThan(fileStat.st_size, fileString.length());
		for (char refChar : fileString)
		{
			char inputChar{};
			assertEqual(fread(&inputChar, 1, 1, file), 1);
			assertEqual(inputChar, refChar);
		}
		// We are intentionally not checking the newline generated by puts() because
		// the logic to do so is gnarly, and having checked the line itself made it,
		// it seems like there's very little point.
		assertEqual(fclose(file), 0); // NOLINT(cppcoreguidelines-owning-memory)
#endif
		assertEqual(unlink(fileName.data()), 0);
	}

	void testBoolConv()
	{
		assertEqual(boolToString(true), "true");
		assertEqual(boolToString(false), "false");
	}

	void testFail()
	{
		tryShouldFail([=]() { fail("This is only a test"); });
		// This next line tests that the rest of the test is skipped but intentionally makes it so the runner sees success.
		throw threadExit_t{0};
	}

	void testAbort()
	{
		try
			{ logResult(RESULT_ABORT, "This message is only a test"); }
		catch (threadExit_t &val)
			{ assertEqual(val, 2); }
	}

public:
	void registerTests() final
	{
		CRUNCHpp_TEST(testAssertTrue)
		CRUNCHpp_TEST(testAssertFalse)
		CRUNCHpp_TEST(testAssertIntEqual)
		CRUNCHpp_TEST(testAssertIntNotEqual)
		CRUNCHpp_TEST(testAssertUintEqual)
		CRUNCHpp_TEST(testAssertUintNotEqual)
		CRUNCHpp_TEST(testAssertDoubleEqual)
		CRUNCHpp_TEST(testAssertDoubleNotEqual)
		CRUNCHpp_TEST(testAssertPtrEqual)
		CRUNCHpp_TEST(testAssertPtrNotEqual)
		CRUNCHpp_TEST(testAssertStrEqual)
		CRUNCHpp_TEST(testAssertStrNotEqual)
		CRUNCHpp_TEST(testAssertMemEqual)
		CRUNCHpp_TEST(testAssertMemNotEqual)
		CRUNCHpp_TEST(testAssertNull)
		CRUNCHpp_TEST(testAssertNotNull)
		CRUNCHpp_TEST(testAssertGreaterThan)
		CRUNCHpp_TEST(testAssertLessThan)
		CRUNCHpp_TEST(testLogging)
		CRUNCHpp_TEST(testBoolConv)
		CRUNCHpp_TEST(testFail)
		CRUNCHpp_TEST(testAbort)
	}
};

class registrationTest final : public testsuite
{
public:
	void registerTests() final
		{ throw std::bad_alloc(); }
};

CRUNCHpp_TESTS(crunchTests, registrationTest)
