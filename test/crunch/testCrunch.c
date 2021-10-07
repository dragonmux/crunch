// SPDX-License-Identifier: LGPL-3.0-or-later
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <float.h>

#include <threading/threadShim.h>
#include <crunch.h>
#include <ranlux.h>

#include <Core.h>
#include <Logger.h>
#include <StringFuncs.h>

typedef void (*failFn_t)();

ranlux32_t *ranlux32;
ranlux64_t *ranlux64;
void *ptr;
/*int32_t snum32;
int64_t snum64;
uint32_t unum32;
uint64_t unum64;*/
int32_t num32;
int64_t num64;
double dblA, dblB;
const char *const testStr1 = "abcdefghijklmnopqrstuvwxyz";
const char *const testStr2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void setup()
{
	ranlux32 = initRanlux32(randomSeed32());
	if (ranlux32 == NULL)
		fail("Failed to initalise 32-bit random number generator");
	ranlux64 = initRanlux64(randomSeed64());
	if (ranlux64 == NULL)
		fail("Failed to initalise 64-bit random number generator");
}

void teardown()
{
	freeRanlux32(ranlux32);
	freeRanlux64(ranlux64);
}

void *genPtr()
{
	uintptr_t ptr = 0;
	if (sizeof(void *) <= 32)
		ptr = genRanlux32(ranlux32);
	else
		ptr = genRanlux64(ranlux64);
	return (void *)ptr;
}

/* Internal sacrificial thread for testing when assertions fail. */
int goatThread(void *test)
	{ ((failFn_t)test)(); return 0; }

void tryShouldFail(const failFn_t test)
{
	int retVal = 0;
	thrd_t testThread; // NOLINT

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
	num32 = genRanlux32(ranlux32);
	assertIntEqual(num32, num32);
	num64 = genRanlux64(ranlux64);
	assertIntEqual(num64, num64);
	while (num32 == 0)
		num32 = genRanlux32(ranlux32);
	tryShouldFail(testAssertIntEqual1);
	while (num64 == 0)
		num64 = genRanlux64(ranlux64);
	tryShouldFail(testAssertIntEqual2);
}

void testAssertIntNotEqual1() { assertIntNotEqual(num32, num32); }
void testAssertIntNotEqual2() { assertIntNotEqual(0, 0); }
void testAssertIntNotEqual3() { assertInt64NotEqual(num64, num64); }
void testAssertIntNotEqual4() { assertInt64NotEqual(0, 0); }
void testAssertIntNotEqual()
{
	num32 = num64 = 0;
	while (num32 == 0)
		num32 = genRanlux32(ranlux32);
	while (num64 == 0)
		num64 = genRanlux64(ranlux64);
	assertIntNotEqual(num32, 0);
	assertInt64NotEqual(num64, 0);
	tryShouldFail(testAssertIntNotEqual1);
	tryShouldFail(testAssertIntNotEqual2);
	tryShouldFail(testAssertIntNotEqual3);
	tryShouldFail(testAssertIntNotEqual4);
}

void testAssertDoubleEqual1() { assertDoubleEqual(0.0, 0.1); }
void testAssertDoubleEqual()
{
	const double num = genDouble(ranlux64);
	assertDoubleEqual(0.0, 0.0);
	assertDoubleEqual(num, num);
	tryShouldFail(testAssertDoubleEqual1);
}

#define DELTA(result, expected) ((result) >= ((expected) - DBL_EPSILON) && (result) <= ((expected) + DBL_EPSILON))

void testAssertDoubleNotEqual1() { assertDoubleNotEqual(0.0, 0.0); }
void testAssertDoubleNotEqual2() { assertDoubleNotEqual(dblA, dblA); }
void testAssertDoubleNotEqual3() { assertDoubleNotEqual(dblB, dblB); }
void testAssertDoubleNotEqual()
{
	dblA = genDouble(ranlux64);
	dblB = dblA;
	while (DELTA(dblB, dblA))
		dblB = genDouble(ranlux64);
	assertDoubleNotEqual(dblA, dblB);
	tryShouldFail(testAssertDoubleNotEqual1);
	tryShouldFail(testAssertDoubleNotEqual2);
	tryShouldFail(testAssertDoubleNotEqual3);
}

void testAssertPtrEqual1() { assertPtrEqual(ptr, NULL); }
void testAssertPtrEqual()
{
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
	ptr = NULL;
	while (ptr == NULL)
		ptr = genPtr();
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
void testAssertNull()
{
	ptr = NULL;
	while (ptr == NULL)
		ptr = genPtr();
	assertNull(NULL);
	tryShouldFail(testAssertNull1);
}

void testAssertNotNull1() { assertNotNull(NULL); }
void testAssertNotNull()
{
	ptr = NULL;
	while (ptr == NULL)
		ptr = genPtr();
	assertNotNull(ptr);
	tryShouldFail(testAssertNotNull1);
}

void testAssertGreaterThan1() { assertGreaterThan(num32, num32); }
void testAssertGreaterThan2() { assertGreaterThan(0, num32); }
void testAssertGreaterThan3() { assertGreaterThan64(num64, num64); }
void testAssertGreaterThan4() { assertGreaterThan64(0, num64); }
void testAssertGreaterThan()
{
	num32 = num64 = 0;
	while (num32 <= 0)
		num32 = genRanlux32(ranlux32);
	while (num64 <= 0)
		num64 = genRanlux64(ranlux64);
	num32 &= ~(UINT32_C(1) << 31U);
	num64 &= ~(UINT64_C(1) << 63U);
	assertGreaterThan(num32, 0);
	assertGreaterThan64(num64, 0);
	tryShouldFail(testAssertGreaterThan1);
	tryShouldFail(testAssertGreaterThan2);
	tryShouldFail(testAssertGreaterThan3);
	tryShouldFail(testAssertGreaterThan4);
}

void testAssertLessThan1() { assertLessThan(num32, num32); }
void testAssertLessThan2() { assertLessThan(num32, 0); }
void testAssertLessThan3() { assertLessThan64(num64, num64); }
void testAssertLessThan4() { assertLessThan64(num64, 0); }
void testAssertLessThan()
{
	num32 = num64 = 0;
	while (num32 <= 0)
		num32 = genRanlux32(ranlux32);
	while (num64 <= 0)
		num64 = genRanlux64(ranlux64);
	num32 &= ~(UINT32_C(1) << 31U);
	num64 &= ~(UINT64_C(1) << 63U);
	assertLessThan(0, num32);
	assertLessThan64(0, num64);
	tryShouldFail(testAssertLessThan1);
	tryShouldFail(testAssertLessThan2);
	tryShouldFail(testAssertLessThan3);
	tryShouldFail(testAssertLessThan4);
}

void testLogging()
{
	const char *const fileName ="test.log";
	const char *const fileString = "Print to file test";
	assertGreaterThan(puts("Print to console test"), -1);
	assertNull(startLogging(NULL));
	assertNull(startLogging(""));
	testLog *logFile = startLogging(fileName);
	assertNotNull(logFile);
	// Assert that double-logging is not allowed
	assertNull(startLogging(fileName));
	stopLogging(NULL); // code coverage stuff.. this shouldn't affect the next line.
	assertGreaterThan(puts(fileString), -1);
	stopLogging(logFile);
	logFile = NULL;
	while (logFile == NULL)
		logFile = genPtr();
	stopLogging(logFile); // code coverage stuff.. this should be harmless.
	FILE *const file = fopen(fileName, "r");
	assertNotNull(file);
	struct stat fileStat;
	assertIntEqual(fstat(fileno(file), &fileStat), 0);
#ifndef _WINDOWS
	assertGreaterThan(fileStat.st_size, strlen(fileString));
	for (size_t i = 0; i < strlen(fileString); ++i)
	{
		char inputChar = 0;
		assertIntEqual(fread(&inputChar, 1, 1, file), 1);
		assertIntEqual(inputChar, fileString[i]);
	}
#endif
	// We are intentionally not checking the newline generated by puts() because
	// the logic to do so is gnarly, and having checked the line itself made it,
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

void testFormatString()
{
	char *result = NULL;
	result = formatString("Test");
	assertNotNull(result);
	assertStringEqual(result, "Test");
	free(result);
	result = formatString("%d.%u\n", -5, 6U);
	assertNotNull(result);
	assertStringEqual(result, "-5.6\n");
	free(result);
	result = formatString("%");
	assertNull(result);
}

#if !defined(_WIN32) && !defined(__APPLE__) && !defined(NO_ALLOC_TEST) && !defined(CRUNCH_ASAN)
void testAllocs()
{
	allocCount = 0;
	assertNull(formatString("a"));
	assertIntEqual(allocCount, -1);
	allocCount = 0;
	assertNull(startLogging(""));
	assertIntEqual(allocCount, -1);
}
#endif

BEGIN_REGISTER_TESTS()
	TEST(setup)
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
	TEST(testFormatString)
#if !defined(_WIN32) && !defined(__APPLE__) && !defined(NO_ALLOC_TEST) && !defined(CRUNCH_ASAN)
	TEST(testAllocs)
#endif
	TEST(teardown)
END_REGISTER_TESTS()
