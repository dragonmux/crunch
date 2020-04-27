// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef CORE__H
#define CORE__H

#include <utility>
#include "crunch++.h"

struct cxxTestClass final
{
private:
	std::unique_ptr<testsuite> suite_;
	const char *name_;

public:
	cxxTestClass(std::unique_ptr<testsuite> &&suite, const char *const name) :
		suite_{std::move(suite)}, name_{name} { }

	testsuite *suite() const noexcept { return suite_.get(); }
	const char *name() const noexcept { return name_; }
};

CRUNCHpp_API uint32_t passes, failures;
CRUNCHpp_API bool loggingTests;
CRUNCHpp_API std::vector<cxxTestClass> cxxTests;

#endif /*CORE__H*/
