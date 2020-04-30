// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef STRINGFUNCS__H
#define STRINGFUNCS__H

#include <stdint.h>
#include "crunch.h"

#ifdef __GNUC__
#define CRUNCH_FORMAT(style, fmtIndex, varIndex) __attribute__ ((format(style, fmtIndex, varIndex)))
#define CRUNCH_MALLOC __attribute__((malloc))
#else
#define CRUNCH_FORMAT(style, fmtIndex, varIndex)
#define CRUNCH_MALLOC
#endif

CRUNCH_API const char *boolToString(uint8_t value);
CRUNCH_API char *formatString(const char *format, ...) CRUNCH_FORMAT(printf, 1, 2) CRUNCH_MALLOC;

#endif /*STRINGFUNCS__H*/
