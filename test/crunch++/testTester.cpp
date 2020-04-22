// SPDX-License-Identifier: LGPL-3.0-or-later
#include <crunch++.h>

struct testException_t final : std::exception
{
public:
	const char *what() const noexcept final
		{ return "Boo!"; }
};

class throwTest final : public testsuite
{
private:
	void testSkip()
		{ skip("I'm a skipped test"); }

	void testThrow()
		{ throw testException_t(); }

public:
	void registerTests() final
	{
		CXX_TEST(testSkip)
		CXX_TEST(testThrow)
	}
};

class registerExitTest : public testsuite
{
public:
	void registerTests() final
		{ throw threadExit_t{0}; }
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<registerExitTest, throwTest>();
}
