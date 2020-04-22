// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <substrate/utility>
#include "crunch++.h"
#include "stringFuncs.hxx"

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
