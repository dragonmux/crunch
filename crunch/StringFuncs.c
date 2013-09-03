#include "crunch.h"
#include "StringFuncs.h"
#include "Memory.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

const char *boolToString(uint8_t value)
{
	if (value == FALSE)
		return "false";
	else
		return "true";
}

char *formatString(const char *format, ...)
{
	int len;
	char *ret;
	va_list args;
	va_start(args, format);
	len = vsnprintf(NULL, 0, format, args);
	va_end(args);
	ret = (char *)testMalloc(len + 1);
	va_start(args, format);
	vsprintf(ret, format, args);
	va_end(args);
	return ret;
}
