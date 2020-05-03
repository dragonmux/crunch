// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef STRINGFUNCS__HXX
#define STRINGFUNCS__HXX

#include <memory>
#include <string>
#include "crunch++.h"

#ifdef __GNUC__
#define CRUNCH_FORMAT(style, fmtIndex, varIndex) __attribute__ ((format(style, fmtIndex, varIndex)))
#else
#define CRUNCH_FORMAT(style, fmtIndex, varIndex)
#endif

CRUNCHpp_API const char *boolToString(bool value);
CRUNCHpp_API std::unique_ptr<char []> formatString(const char *format, ...) noexcept // NOLINT
	CRUNCH_FORMAT(printf, 1, 2);

inline std::string operator ""_s(const char *string, const std::size_t length)
	{ return std::string{string, length}; }

#endif /*STRINGFUNCS__HXX*/
