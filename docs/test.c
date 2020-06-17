#include <stdbool.h>
#include <crunch.h>

void testCase()
{
	assertNull(NULL);
	assertTrue(true);
}

BEGIN_REGISTER_TESTS()
	TEST(testCase)
END_REGISTER_TESTS()
