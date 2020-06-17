#include <crunch++.h>

class testSuite final : public testsuite
{
private:
	void testCase()
	{
		assertNotNull(this);
		assertTrue(true);
	}

	void testSkip()
	{
		skip("Intentionally skipping this test");
	}

public:
	void registerTests() final
	{
		CRUNCHpp_TEST(testCase)
		CRUNCHpp_TEST(testSkip)
	}
};

class testSkipSuite final : public testsuite
{
public:
	void registerTests() final
	{
		skip("Intentionally skipping this suite");
	}
};

CRUNCHpp_TESTS(testSuite, testSkipSuite)
