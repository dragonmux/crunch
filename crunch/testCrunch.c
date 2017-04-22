/*
 * This file is part of crunch
 * Copyright © 2013-2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <pthread.h>
#include <crunch.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "Core.h"
#include "Logger.h"

typedef void (*failFn_t)();

void *ptr;
long value;
/*int32_t snum32;
int64_t snum64;
uint32_t unum32;
uint64_t unum64;*/
int32_t num32;
int64_t num64;

void *genPtr()
{
#if defined(_M_X64) || defined(__X86_64__)
	return (((void *)(long)rand()) << 32) | ((void *)(long)rand());
#else
	return (void *)(long)rand();
#endif
}

/* Internal sacrificial thread for testing when assertions fail. */
void *goatThread(void *test)
	{ ((failFn_t)test)(); }

void tryShouldFail(const failFn_t test)
{
	int *retVal;
	pthread_t testThread;
	pthread_attr_t threadAttrs;

	pthread_attr_init(&threadAttrs);
	pthread_attr_setdetachstate(&threadAttrs, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setscope(&threadAttrs, PTHREAD_SCOPE_PROCESS);
	pthread_create(&testThread, &threadAttrs, goatThread, test);
	pthread_join(testThread, (void **)&retVal);
	assertNotNull(retVal);
	assertIntEqual(*retVal, 1);
	--failures;
}

void testAssertTrue1() { assertTrue(FALSE); }
void testAssertTrue()
{
	assertTrue(TRUE);
	tryShouldFail(testAssertTrue1);
}

void testAssertFalse1() { assertFalse(TRUE); }
void testAssertFalse()
{
	assertFalse(FALSE);
	tryShouldFail(testAssertFalse1);
}

void testAssertIntEqual1() { assertIntEqual(0, num32); }
void testAssertIntEqual2() { assertInt64Equal(0, num64); }
void testAssertIntEqual()
{
	srand(time(NULL));
	num32 = rand();
	assertIntEqual(num32, num32);
	num64 = (((int64_t)rand()) << 32) | ((int64_t)rand());
	assertIntEqual(num64, num64);
	while (num32 == 0)
		num32 = rand();
	tryShouldFail(testAssertIntEqual1);
	while (num64 == 0)
		num64 = (((int64_t)rand()) << 32) | ((int64_t)rand());
	tryShouldFail(testAssertIntEqual2);
}

void testAssertIntNotEqual1() { assertIntNotEqual(num32, num32); }
void testAssertIntNotEqual2() { assertIntNotEqual(0, 0); }
void testAssertIntNotEqual3() { assertInt64NotEqual(num64, num64); }
void testAssertIntNotEqual4() { assertInt64NotEqual(0, 0); }
void testAssertIntNotEqual()
{
	srand(time(NULL));
	do
		num32 = rand();
	while (num32 == 0);
	do
		num64 = (((int64_t)rand()) << 32) | ((int64_t)rand());
	while (num64 == 0);
	assertIntNotEqual(num32, 0);
	tryShouldFail(testAssertIntNotEqual1);
	tryShouldFail(testAssertIntNotEqual2);
	tryShouldFail(testAssertIntNotEqual3);
	tryShouldFail(testAssertIntNotEqual4);
}

void testAssertDoubleEqual1() { assertDoubleEqual(0.0, 0.1); }
void testAssertDoubleEqual()
{
	//double num = (*rng)(rngGen);
	assertDoubleEqual(0.0, 0.0);
	//assertDoubleEqual(num, num);
	tryShouldFail(testAssertDoubleEqual1);
}

void testAssertDoubleNotEqual1() { assertDoubleNotEqual(0.0, 0.0); }
//void testAssertDoubleNotEqual2() { assertDoubleNotEqual(numA, numA); }
//void testAssertDoubleNotEqual3() { assertDoubleNotEqual(numB, numB); }
void testAssertDoubleNotEqual()
{
	//double numA = (*rng)(rngGen);
	//double numB = (*rng)(rngGen);
	//assertNotEqual(numA, numB);
	tryShouldFail(testAssertDoubleNotEqual1);
	//tryShouldFail(testAssertDoubleNotEqual2);
	//tryShouldFail(testAssertDoubleNotEqual3);
}

void testAssertPtrEqual1() { assertPtrEqual(ptr, NULL); }
void testAssertPtrEqual()
{
	srand(time(NULL));
	ptr = genPtr();
	assertPtrEqual(ptr, ptr);
	while (ptr == NULL)
		ptr = genPtr();
	tryShouldFail(testAssertPtrEqual1);
}

void testAssertPtrNotEqual1() { assertPtrNotEqual(ptr, ptr); }
void testAssertPtrNotEqual2() { assertPtrNotEqual(NULL, NULL); }
void testAssertPtrNotEqual()
{
	srand(time(NULL));
	do
		ptr = genPtr();
	while (ptr == NULL);
	assertPtrNotEqual(ptr, NULL);
	tryShouldFail(testAssertPtrNotEqual1);
	tryShouldFail(testAssertPtrNotEqual2);
}

void testAssertNull1() { assertNull(ptr); }
void testAssertNull()
{
	srand(time(NULL));
	do
		ptr = genPtr();
	while (ptr == NULL);
	assertNull(NULL);
	tryShouldFail(testAssertNull1);
}

void testAssertNotNull1() { assertNotNull(NULL); }
void testAssertNotNull()
{
	void *ptr;
	srand(time(NULL));
	do
		ptr = genPtr();
	while (ptr == NULL);
	assertNotNull(ptr);
	tryShouldFail(testAssertNotNull1);
}

void testAssertGreaterThan1() { assertGreaterThan(value, value); }
void testAssertGreaterThan2() { assertGreaterThan(0, value); }
void testAssertGreaterThan()
{
	srand(time(NULL));
	do
		value = (long)genPtr();
	while (ptr == NULL);
	assertGreaterThan(value, 0);
	tryShouldFail(testAssertGreaterThan1);
	tryShouldFail(testAssertGreaterThan2);
}

void testAssertLessThan1() { assertLessThan(value, value); }
void testAssertLessThan2() { assertLessThan(value, 0); }
void testAssertLessThan()
{
	srand(time(NULL));
	do
		value = (long)genPtr();
	while (ptr == NULL);
	assertLessThan(0, value);
	tryShouldFail(testAssertLessThan1);
	tryShouldFail(testAssertLessThan2);
}

void testLogging()
{
	testLog *logFile;
	assertGreaterThan(printf("Print to console test\n"), 0);
	logFile = startLogging("test.log");
	assertNotNull(logFile);
	assertGreaterThan(printf("Print to file test\n"), 0);
	stopLogging(logFile);
	assertIntEqual(unlink("test.log"), 0);
}

void testFail1() { fail("This is only a test"); }
void testFail()
{
	tryShouldFail(testFail1);
}

/*void testAbort()
{
	logResult(RESULT_ABORT, "This message is only a test");
}*/

BEGIN_REGISTER_TESTS()
	TEST(testAssertTrue)
	TEST(testAssertFalse)
	TEST(testAssertIntEqual)
	TEST(testAssertIntNotEqual)
	TEST(testAssertDoubleEqual)
	TEST(testAssertDoubleNotEqual)
	TEST(testAssertPtrEqual)
	TEST(testAssertPtrNotEqual)
	TEST(testAssertNull)
	TEST(testAssertNotNull)
	TEST(testAssertGreaterThan)
	TEST(testAssertLessThan)
	TEST(testLogging)
	TEST(testFail)
	//TEST(testAbort)
END_REGISTER_TESTS()
