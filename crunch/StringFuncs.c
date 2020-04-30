// SPDX-License-Identifier: LGPL-3.0-or-later
#include "crunch.h"
#include "StringFuncs.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

const char *boolToString(uint8_t value)
{
	if (value)
		return "true";
	else
		return "false";
}

char *formatString(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	const int len = vsnprintf(NULL, 0, format, args);
	va_end(args);
	char *const ret = (char *)malloc(len + 1);
	if (!ret)
		return NULL;
	va_start(args, format);
	vsprintf(ret, format, args);
	va_end(args);
	return ret;
}
