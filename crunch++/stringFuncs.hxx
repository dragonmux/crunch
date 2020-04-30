// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef STRINGFUNCS__H
#define STRINGFUNCS__H

#include <memory>
#include <string>
#include "crunch++.h"

CRUNCHpp_API const char *boolToString(bool value);
CRUNCHpp_API std::unique_ptr<char []> formatString(const char *format, ...) noexcept; // NOLINT

inline std::string operator ""_s(const char *string, const std::size_t length)
	{ return std::string{string, length}; }

#endif /*STRINGFUNCS__H*/
