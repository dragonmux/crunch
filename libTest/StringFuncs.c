#include <string.h>
#include "StringFuncs"

const char *boolToString(uint8_t value)
{
	if (value == FALSE)
		return "false";
	else
		return "true";
}
