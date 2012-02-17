#include "libTest.h"
#include "StringFuncs.h"
#include <string.h>

const char *boolToString(uint8_t value)
{
	if (value == FALSE)
		return "false";
	else
		return "true";
}
