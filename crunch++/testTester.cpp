#include <crunch++.h>

struct testException_t final : std::exception
{
public:
	const char *what() const noexcept final override
		{ return "Boo!"; }
};

class throwTest final : public testsuit
{
public:
	void testThrow()
		{ throw testException_t(); }

	void registerTests() final override
	{
		CXX_TEST(testThrow)
	}
};


CRUNCH_API void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<throwTest>();
}
