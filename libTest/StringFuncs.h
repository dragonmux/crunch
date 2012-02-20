#ifndef __STRINGFUNCS_H__
#define __STRINGFUNCS_H__

#include <inttypes.h>

extern const char *boolToString(uint8_t value);
extern char *formatString(const char *format, ...);

#endif /* __STRINGFUNCS_H__ */
