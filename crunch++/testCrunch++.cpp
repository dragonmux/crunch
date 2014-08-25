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

#include <crunch++.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

class crunchTests : public testsuit
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

public:
	void testAssertTrue()
	{
		assertTrue(true);
	}

	void testAssertFalse()
	{
		assertFalse(false);
	}

	void testAssertIntEqual()
	{
		int num;
		srand(time(NULL));
		num = rand();
		assertEqual(num, num);
	}

	void testAssertIntNotEqual()
	{
		int num;
		srand(time(NULL));
		do
		{
			num = rand();
		}
		while (num == 0);
		assertNotEqual(num, 0);
	}

	void testAssertPtrEqual()
	{
		void *ptr;
		srand(time(NULL));
		ptr = genPtr();
		assertPtrEqual(ptr, ptr);
	}

	void testAssertPtrNotEqual()
	{
		void *ptr;
		srand(time(NULL));
		do
		{
			ptr = genPtr();
		}
		while (ptr == NULL);
		assertNotEqual(ptr, 0);
	}

	void testAssertNull()
	{
		assertNull((void *)NULL);
		assertNull((const void *)NULL);
	}

	void testAssertNotNull()
	{
		void *ptr;
		srand(time(NULL));
		do
		{
			ptr = genPtr();
		}
		while (ptr == NULL);
		assertNotNull(ptr);
	}

	void testAssertGreaterThan()
	{
		void *ptr;
		srand(time(NULL));
		do
		{
			ptr = genPtr();
		}
		while (ptr == NULL);
		assertGreaterThan((long)ptr, 0);
	}

	void testAssertLessThan()
	{
		void *ptr;
		srand(time(NULL));
		do
		{
			ptr = genPtr();
		}
		while (ptr == NULL);
		assertLessThan(0, (long)ptr);
	}

	void testLogging()
	{
		testLog *logFile;
		assertGreaterThan(printf("Print to console test\n"), 0);
		logFile = startLogging("test.log");
		assertNotNull(logFile);
		assertGreaterThan(printf("Print to file test\n"), 0);
		stopLogging(logFile);
		assertEqual(unlink("test.log"), 0);
	}

	void registerTests()
	{
		CXX_TEST(testAssertTrue)
		CXX_TEST(testAssertFalse)
		CXX_TEST(testAssertIntEqual)
		CXX_TEST(testAssertIntNotEqual)
		CXX_TEST(testAssertPtrEqual)
		CXX_TEST(testAssertPtrNotEqual)
		CXX_TEST(testAssertNull)
		CXX_TEST(testAssertNotNull)
		CXX_TEST(testAssertGreaterThan)
		CXX_TEST(testAssertLessThan)
		CXX_TEST(testLogging)
	}
};

CRUNCH_API void registerCXXTests()
{
	registerTestClasses<crunchTests>();
}
