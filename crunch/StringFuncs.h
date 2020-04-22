// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef STRINGFUNCS__H
#define STRINGFUNCS__H

#include <stdint.h>
#include "crunch.h"

CRUNCH_API const char *boolToString(uint8_t value);
CRUNCH_API char *formatString(const char *format, ...);

#endif /*STRINGFUNCS__H*/
