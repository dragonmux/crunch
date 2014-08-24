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

#include "crunch.h"
#include <thread>
#include <vector>

#define CRUNCHpp_API	CRUNCH_VIS

class testsuit;

struct cxxTest
{
	testsuit *unitClass;
	void (testsuit::* testFunc)();
	const char *testName;
};

struct cxxUnitTest
{
	std::thread testThread;
	cxxTest theTest;
};

class CRUNCHpp_API testsuit
{
protected:
	std::vector<cxxTest> tests;

	void fail(const char *reason);

	void assertTrue(bool value);
	void assertFalse(bool value);

	void assertEqual(int result, int expected);
	void assertEqual(int64_t result, int64_t expected);
	void assertEqual(void *result, void *expected);
	void assertEqual(double result, double expected);
	void assertEqual(const char *result, const char *expected);
	void assertEqual(const void *result, const void *expected, const size_t expectedLength);

	void assertNotEqual(int result, int expected);
	void assertNotEqual(int64_t result, int64_t expected);
	void assertNotEqual(void *result, void *expected);
	void assertNotEqual(double result, double expected);
	void assertNotEqual(const char *result, const char *expected);
	void assertNotEqual(const void *result, const void *expected, const size_t expectedLength);

	void assertNull(void *result);
	void assertNotNull(void *result);
	void assertNull(const void *result);
	void assertNotNull(const void *result);

	void assertGreaterThan(long result, long expected);
	void assertLessThan(long result, long expected);

	testsuit();
	virtual ~testsuit();

private:
	static int testRunner(cxxUnitTest &test);

public:
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

#if 0
#define BEGIN_REGISTER_TESTS() \
CRUNCH_EXPORT void registerCXXTests() \
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
#endif

#endif /*__CRUNCHpp_H__*/
