#include <crunch.h>

void *genPtr()
{
#if defined(_M_X64) || defined(__X86_64__)
	return (((void *)(long)rand()) << 32) | ((void *)(long)rand());
#else
	return (void *)(long)rand();
#endif
}

void testAssertTrue()
{
	assertTrue(TRUE);
}

void testAssertFalse()
{
	assertFalse(FALSE);
}

void testAssertIntEqual()
{
	int num;
	srand(time(NULL));
	num = rand();
	assertIntEqual(num, num);
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
	assertIntNotEqual(num, 0);
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
	assertPtrNotEqual(ptr, 0);
}

void testAssertNull()
{
	assertNull(NULL);
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
	log *logFile;
	assertGreaterThan(printf("Print to console test\n"), 0);
	logFile = startLogging("test.log");
	assertNotNull(logFile);
	assertGreaterThan(printf("Print to file test\n"), 0);
	stopLogging(logFile);
	assertIntEqual(unlink("test.log"), 0);
}

BEGIN_REGISTER_TESTS()
	TEST(testAssertTrue)
	TEST(testAssertFalse)
	TEST(testAssertIntEqual)
	TEST(testAssertIntNotEqual)
	TEST(testAssertPtrEqual)
	TEST(testAssertPtrNotEqual)
	TEST(testAssertNull)
	TEST(testAssertNotNull)
	TEST(testAssertGreaterThan)
	TEST(testAssertLessThan)
	TEST(testLogging)
END_REGISTER_TESTS()
