/*
 * This file is part of crunch
 * Copyright © 2013-2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

#ifndef __STRINGFUNCS_H__
#define __STRINGFUNCS_H__

#include "crunch++.h"
#include "memory.hxx"

CRUNCHpp_API const char *boolToString(bool value);
CRUNCHpp_API std::unique_ptr<char []> formatString(const char *format, ...) noexcept;
CRUNCHpp_API std::unique_ptr<char []> stringDup(const char *const str) noexcept;
CRUNCHpp_API std::unique_ptr<const char []> strNewDup(const char *const str) noexcept;

#endif /* __STRINGFUNCS_H__ */
