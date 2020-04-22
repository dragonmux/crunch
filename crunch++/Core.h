/*
 * This file is part of crunch
 * Copyright © 2013-2020 Rachel Mant (dx-mon@users.sourceforge.net)
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
