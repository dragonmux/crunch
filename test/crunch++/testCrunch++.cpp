/*
 * This file is part of crunch
 * Copyright © 2013-2018 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <crunch++.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <memory>
#include <random>
#include <functional>
#include "Core.h"
#include "StringFuncs.h"
#include "Logger.h"

using namespace std;

class crunchTests final : public testsuit
{
private:
	void *genPtr()
	{
#if defined(_M_X64) || defined(__X86_64__)
		return (((void *)(long)rand()) << 32) | ((void *)(long)rand());
#else
		return (void *)(long)rand();
#endif
	}

	const char *const testStr1 = "abcdefghijklmnopqrstuvwxyz";
	const char *const testStr2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	default_random_engine rngGen;
	unique_ptr<uniform_real_distribution<double>> rng;

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

public:
	crunchTests() : rng(new uniform_real_distribution<double>(-1.0, 1.0))
	{
		rngGen.seed(time(nullptr));
	}

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
		srand(time(nullptr));
		int8_t num8 = rand();
		int16_t num16 = rand();
		int32_t num32 = rand();
		int64_t num64 = (int64_t(rand()) << 32) | int64_t(rand());

		assertEqual(num8, num8);
		assertEqual(num16, num16);
		assertEqual(num32, num32);
		assertEqual(num64, num64);

		while (!num8)
			num8 = rand();
		while (!num16)
			num16 = rand();
		while (!num32)
			num32 = rand();
		while (!num32)
			num64 = (int64_t(rand()) << 32) | int64_t(rand());

		tryShouldFail([=]() { assertEqual(int8_t(0), num8); });
		tryShouldFail([=]() { assertEqual(int16_t(0), num16); });
		tryShouldFail([=]() { assertEqual(int32_t(0), num32); });
		tryShouldFail([=]() { assertEqual(int64_t(0), num64); });
	}

	void testAssertIntNotEqual()
	{
		srand(time(nullptr));
		int8_t num8;
		int16_t num16;
		int32_t num32;
		int64_t num64;

		do
			num8 = rand();
		while (!num8);
		do
			num16 = rand();
		while (!num16);
		do
			num32 = rand();
		while (!num32);
		do
			num64 = (int64_t(rand()) << 32) | int64_t(rand());
		while (!num64);

		assertNotEqual(num8, 0);
		assertNotEqual(num16, 0);
		assertNotEqual(num32, 0);
		assertNotEqual(num64, 0);

		tryShouldFail([=]() { assertNotEqual(num8, num8); });
		tryShouldFail([=]() { assertNotEqual(int8_t(0), int8_t(0)); });
		tryShouldFail([=]() { assertNotEqual(num16, num16); });
		tryShouldFail([=]() { assertNotEqual(int16_t(0), int16_t(0)); });
		tryShouldFail([=]() { assertNotEqual(num32, num32); });
		tryShouldFail([=]() { assertNotEqual(int32_t(0), int32_t(0)); });
		tryShouldFail([=]() { assertNotEqual(num64, num64); });
		tryShouldFail([=]() { assertNotEqual(int64_t(0), int64_t(0)); });
	}

	void testAssertUintEqual()
	{
		srand(time(nullptr));
		uint8_t num8 = rand();
		uint16_t num16 = rand();
		uint32_t num32 = rand();
		uint64_t num64 = (int64_t(rand()) << 32) | int64_t(rand());

		assertEqual(num8, num8);
		assertEqual(num16, num16);
		assertEqual(num32, num32);
		assertEqual(num64, num64);

		tryShouldFail([this]() { assertEqual(uint8_t(0), uint8_t(1)); });
		tryShouldFail([this]() { assertEqual(uint16_t(0), uint16_t(1)); });
		tryShouldFail([this]() { assertEqual(uint32_t(0), uint32_t(1)); });
		tryShouldFail([this]() { assertEqual(uint64_t(0), uint64_t(1)); });
	}

	void testAssertUintNotEqual()
	{
		srand(time(nullptr));
		uint8_t num8;
		uint16_t num16;
		uint32_t num32;
		uint64_t num64;

		do
			num8 = rand();
		while (!num8);
		do
			num16 = rand();
		while (!num16);
		do
			num32 = rand();
		while (!num32);
		do
			num64 = (int64_t(rand()) << 32) | int64_t(rand());
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
		double num = (*rng)(rngGen);
		assertEqual(num, num);
		tryShouldFail([=]() { assertEqual(0.0, 0.1); });
	}

	void testAssertDoubleNotEqual()
	{
		double numA = (*rng)(rngGen);
		double numB = (*rng)(rngGen);
		assertNotEqual(numA, numB);
		tryShouldFail([=]() { assertNotEqual(numA, numA); });
		tryShouldFail([=]() { assertNotEqual(numB, numB); });
	}

	void testAssertPtrEqual()
	{
		void *ptr;
		srand(time(nullptr));
		ptr = genPtr();
		assertEqual(ptr, ptr);
		while (!ptr)
			ptr = genPtr();
		tryShouldFail([=]() { assertEqual(static_cast<void *>(nullptr), ptr); });
	}

	void testAssertPtrNotEqual()
	{
		void *ptr;
		srand(time(nullptr));
		do
			ptr = genPtr();
		while (ptr == nullptr);
		assertNotEqual(ptr, nullptr);
		tryShouldFail([=]() { assertNotEqual(ptr, ptr); });
	}

	void testAssertStrEqual()
	{
		assertEqual(testStr1, testStr1);
		assertEqual(testStr2, testStr2);
		tryShouldFail([=]() { assertEqual(testStr1, testStr2); });
	}

	void testAssertStrNotEqual()
	{
		assertNotEqual(testStr1, testStr2);
		tryShouldFail([=]() { assertNotEqual(testStr1, testStr1); });
		tryShouldFail([=]() { assertNotEqual(testStr2, testStr2); });
	}

	void testAssertMemEqual()
	{
		assertEqual(testStr1, testStr1, 27);
		assertEqual(testStr2, testStr2, 27);
		tryShouldFail([=]() { assertEqual(testStr1, testStr2, 27); });
	}

	void testAssertMemNotEqual()
	{
		assertNotEqual(testStr1, testStr2, 27);
		tryShouldFail([=]() { assertNotEqual(testStr1, testStr1, 27); });
		tryShouldFail([=]() { assertNotEqual(testStr2, testStr2, 27); });
	}

	void testAssertNull()
	{
		assertNull(static_cast<void *>(nullptr));
		assertNull(static_cast<const void *>(nullptr));
		tryShouldFail([=]() { assertNull(const_cast<char *>(testStr1)); });
		tryShouldFail([=]() { assertNull(const_cast<const char *const>(testStr1)); });
		tryShouldFail([=]() { assertNull(const_cast<char *>(testStr2)); });
		tryShouldFail([=]() { assertNull(const_cast<const char *const>(testStr2)); });
	}

	void testAssertNotNull()
	{
		void *ptr;
		srand(time(nullptr));
		do
			ptr = genPtr();
		while (ptr == nullptr);
		assertNotNull(ptr);
		assertNotNull((const void *)ptr);
		tryShouldFail([=]() { assertNotNull(static_cast<void *>(nullptr)); });
		tryShouldFail([=]() { assertNotNull(static_cast<const void *const>(nullptr)); });
	}

	void testAssertGreaterThan()
	{
		long value;
		srand(time(nullptr));
		do
			value = long(genPtr());
		while (!value);
		assertGreaterThan(value, 0);
		tryShouldFail([=]() { assertGreaterThan(value, value); });
		tryShouldFail([=]() { assertGreaterThan(0, value); });
	}

	void testAssertLessThan()
	{
		long value;
		srand(time(nullptr));
		do
			value = long(genPtr());
		while (!value);
		assertLessThan(0, value);
		tryShouldFail([=]() { assertLessThan(value, value); });
		tryShouldFail([=]() { assertLessThan(value, 0); });
	}

	void testLogging()
	{
		testLog *logFile;
		assertGreaterThan(printf("Print to console test\n"), 0);
		logFile = startLogging("test.log");
		assertNotNull(logFile);
		// Checks that trying to begin logging while already logging causes the framework to ignore the second request
		assertNull(startLogging("test.log"));
		assertGreaterThan(printf("Print to file test\n"), 0);
		stopLogging(logFile);
		assertEqual(unlink("test.log"), 0);
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
		throw threadExit_t(0);
	}

	void testAbort()
	{
		try
			{ logResult(RESULT_ABORT, "This message is only a test"); }
		catch (threadExit_t &val)
			{ assertEqual(val, 2); }
	}

	void registerTests() final override
	{
		CXX_TEST(testAssertTrue)
		CXX_TEST(testAssertFalse)
		CXX_TEST(testAssertIntEqual)
		CXX_TEST(testAssertIntNotEqual)
		CXX_TEST(testAssertUintEqual)
		CXX_TEST(testAssertUintNotEqual)
		CXX_TEST(testAssertDoubleEqual)
		CXX_TEST(testAssertDoubleNotEqual)
		CXX_TEST(testAssertPtrEqual)
		CXX_TEST(testAssertPtrNotEqual)
		CXX_TEST(testAssertStrEqual)
		CXX_TEST(testAssertStrNotEqual)
		CXX_TEST(testAssertMemEqual)
		CXX_TEST(testAssertMemNotEqual)
		CXX_TEST(testAssertNull)
		CXX_TEST(testAssertNotNull)
		CXX_TEST(testAssertGreaterThan)
		CXX_TEST(testAssertLessThan)
		CXX_TEST(testLogging)
		CXX_TEST(testBoolConv)
		CXX_TEST(testFail)
		CXX_TEST(testAbort)
	}
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<crunchTests>();
}