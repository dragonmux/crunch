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

#include <substrate/utility>
#include "crunch++.h"
#include "StringFuncs.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

const char *boolToString(bool value)
{
	if (value == false)
		return "false";
	else
		return "true";
}

std::unique_ptr<char []> vaFormatString(const char *format, va_list args) noexcept
{
	va_list lenArgs;
	va_copy(lenArgs, args);
	const size_t len = vsnprintf(NULL, 0, format, lenArgs) + 1;
	va_end(lenArgs);
	auto ret = substrate::make_unique<char []>(len);
	if (!ret)
		return nullptr;
	vsprintf(ret.get(), format, args);
	return ret;
}

std::unique_ptr<char []> formatString(const char *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	auto ret = vaFormatString(format, args);
	va_end(args);
	return ret;
}
