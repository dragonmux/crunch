// SPDX-License-Identifier: LGPL-3.0-or-later
#include <crunch++.h>

class dummy final : public testsuite
{
public:
	void registerTests() final { }
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<dummy>();
}
