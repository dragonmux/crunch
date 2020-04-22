// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __STRINGFUNCS_H__
#define __STRINGFUNCS_H__

#include <memory>
#include "crunch++.h"

CRUNCHpp_API const char *boolToString(bool value);
CRUNCHpp_API std::unique_ptr<char []> formatString(const char *format, ...) noexcept;

#endif /* __STRINGFUNCS_H__ */
