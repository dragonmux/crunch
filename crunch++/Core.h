// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __CORE_H__
#define __CORE_H__

#include <utility>
#include "crunch++.h"

struct cxxTestClass final
{
	std::unique_ptr<testsuite> testClass;
	const char *testClassName;

	cxxTestClass(std::unique_ptr<testsuite> &&suite, const char *const name) :
		testClass{std::move(suite)}, testClassName{name} { }
};

CRUNCHpp_API uint32_t passes, failures;
CRUNCHpp_API bool loggingTests;
CRUNCHpp_API std::vector<cxxTestClass> cxxTests;

#endif /* __CORE_H__ */
