// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <substrate/utility>
#include "crunch++.h"
#include "stringFuncs.hxx"

const char *boolToString(bool value)
{
	if (value)
		return "true";
	else
		return "false";
}

std::unique_ptr<char []> vaFormatString(const char *format, va_list args) noexcept // NOLINT
{
	va_list lenArgs;
	va_copy(lenArgs, args);
	const auto len{vsnprintf(nullptr, 0, format, lenArgs)};
	va_end(lenArgs);
	if (len <= 0)
		return {};
	auto ret{substrate::make_unique_nothrow<char []>(len + 1)}; // NOLINT
	if (!ret)
		return nullptr;
	vsprintf(ret.get(), format, args);
	return ret;
}

std::unique_ptr<char []> formatString(const char *format, ...) noexcept // NOLINT
{
	va_list args;
	va_start(args, format);
	auto ret = vaFormatString(format, args);
	va_end(args);
	return ret;
}
