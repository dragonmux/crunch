/*
 * This file is part of crunch
 * Copyright © 2013-2019 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include <threads.h>
#include <crunch.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string.h>
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
double dblA, dblB;
const char *const testStr1 = "abcdefghijklmnopqrstuvwxyz";
const char *const testStr2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void *genPtr()
{
#if defined(_M_X64) || defined(__X86_64__)
	return (((void *)(long)rand()) << 32) | ((void *)(long)rand());
#else
	return (void *)(long)rand();
#endif
}

double genDbl()
{
	return ((double)rand()) / ((double)RAND_MAX);
}

/* Internal sacrificial thread for testing when assertions fail. */
int goatThread(void *test)
	{ ((failFn_t)test)(); return 0; }

void tryShouldFail(const failFn_t test)
{
	int retVal;
	thrd_t testThread;

	thrd_create(&testThread, goatThread, test);
	thrd_join(testThread, &retVal);
	assertGreaterThan(retVal, 0);
	assertLessThan(retVal, 3);
	if (retVal == 1)
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
	srand(time(NULL));
	double num = genDbl();
	assertDoubleEqual(0.0, 0.0);
	assertDoubleEqual(num, num);
	tryShouldFail(testAssertDoubleEqual1);
}

void testAssertDoubleNotEqual1() { assertDoubleNotEqual(0.0, 0.0); }
void testAssertDoubleNotEqual2() { assertDoubleNotEqual(dblA, dblA); }
void testAssertDoubleNotEqual3() { assertDoubleNotEqual(dblB, dblB); }
void testAssertDoubleNotEqual()
{
	srand(time(NULL));
	dblA = genDbl();
	dblB = genDbl();
	assertDoubleNotEqual(dblA, dblB);
	tryShouldFail(testAssertDoubleNotEqual1);
	tryShouldFail(testAssertDoubleNotEqual2);
	tryShouldFail(testAssertDoubleNotEqual3);
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

void testAssertStrEqual1() { assertStringEqual(testStr1, testStr2); }
void testAssertStrEqual()
{
	assertStringEqual(testStr1, testStr1);
	assertStringEqual(testStr2, testStr2);
	tryShouldFail(testAssertStrEqual1);
}

void testAssertStrNotEqual1() { assertStringNotEqual(testStr1, testStr1); }
void testAssertStrNotEqual2() { assertStringNotEqual(testStr2, testStr2); }
void testAssertStrNotEqual()
{
	assertStringNotEqual(testStr1, testStr2);
	tryShouldFail(testAssertStrNotEqual1);
	tryShouldFail(testAssertStrNotEqual2);
}

void testAssertMemEqual1() { assertMemEqual(testStr1, testStr2, 27); }
void testAssertMemEqual()
{
	assertMemEqual(testStr1, testStr1, 27);
	assertMemEqual(testStr2, testStr2, 27);
	tryShouldFail(testAssertMemEqual1);
}

void testAssertMemNotEqual1() { assertMemNotEqual(testStr1, testStr1, 27); }
void testAssertMemNotEqual2() { assertMemNotEqual(testStr2, testStr2, 27); }
void testAssertMemNotEqual()
{
	assertMemNotEqual(testStr1, testStr2, 27);
	tryShouldFail(testAssertMemNotEqual1);
	tryShouldFail(testAssertMemNotEqual2);
}

void testAssertNull1() { assertNull(ptr); }
void testAssertNull2() { assertConstNull(ptr); }
void testAssertNull()
{
	srand(time(NULL));
	do
		ptr = genPtr();
	while (ptr == NULL);
	assertNull(NULL);
	assertConstNull(NULL);
	tryShouldFail(testAssertNull1);
	tryShouldFail(testAssertNull2);
}

void testAssertNotNull1() { assertNotNull(NULL); }
void testAssertNotNull2() { assertConstNotNull(NULL); }
void testAssertNotNull()
{
	void *ptr;
	srand(time(NULL));
	do
		ptr = genPtr();
	while (ptr == NULL);
	assertNotNull(ptr);
	assertConstNotNull(ptr);
	tryShouldFail(testAssertNotNull1);
	tryShouldFail(testAssertNotNull2);
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
	const char *const fileName ="test.log";
	const char *const fileString = "Print to file test";
	assertGreaterThan(puts("Print to console test"), -1);
	assertNull(startLogging(NULL));
	assertNull(startLogging(""));
	testLog *const logFile = startLogging(fileName);
	assertNotNull(logFile);
	// Assert that double-logging is not allowed
	assertNull(startLogging(fileName));
	stopLogging(NULL); // code coverage stuff.. this shouldn't affect the next line.
	assertGreaterThan(puts(fileString), -1);
	stopLogging(logFile);
	stopLogging(logFile); // code coverage stuff.. this should be harmless.
	FILE *const file = fopen(fileName, "r");
	assertNotNull(file);
	struct stat fileStat = {};
	assertIntEqual(fstat(fileno(file), &fileStat), 0);
	assertGreaterThan(fileStat.st_size, strlen(fileString));
	for (size_t i = 0; i < strlen(fileString); ++i)
	{
		char inputChar = 0;
		assertIntEqual(fread(&inputChar, 1, 1, file), 1);
		assertIntEqual(inputChar, fileString[i]);
	}
	// We are intentionally not checking the newline generated by puts() because
	// the logic to do so is narly, and having checked the line itself made it,
	// it seems like there's very little point.
	assertIntEqual(fclose(file), 0);
	assertIntEqual(unlink(fileName), 0);
}

void testFail1() { fail("This is only a test"); }
void testFail()
{
	tryShouldFail(testFail1);
}

void testAbort1() { logResult(RESULT_ABORT, "This message is only a test"); }
void testAbort()
{
	tryShouldFail(testAbort1);
}

BEGIN_REGISTER_TESTS()
	TEST(testAssertTrue)
	TEST(testAssertFalse)
	TEST(testAssertIntEqual)
	TEST(testAssertIntNotEqual)
	TEST(testAssertDoubleEqual)
	TEST(testAssertDoubleNotEqual)
	TEST(testAssertPtrEqual)
	TEST(testAssertPtrNotEqual)
	TEST(testAssertStrEqual)
	TEST(testAssertStrNotEqual)
	TEST(testAssertMemEqual)
	TEST(testAssertMemNotEqual)
	TEST(testAssertNull)
	TEST(testAssertNotNull)
	TEST(testAssertGreaterThan)
	TEST(testAssertLessThan)
	TEST(testLogging)
	TEST(testFail)
	TEST(testAbort)
END_REGISTER_TESTS()
