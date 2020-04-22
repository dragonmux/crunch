// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __STRINGFUNCS_H__
#define __STRINGFUNCS_H__

#include <stdint.h>
#include "crunch.h"

CRUNCH_API const char *boolToString(uint8_t value);
CRUNCH_API char *formatString(const char *format, ...);

#endif /* __STRINGFUNCS_H__ */
