// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "crunch++.h"
#include <stdarg.h>

enum resultType
{
	RESULT_FAILURE,
	RESULT_SUCCESS,
	RESULT_SKIP,
	RESULT_ABORT
};

struct testLog
{
	FILE *file;
	FILE *stdout_;
	FILE *realStdout;
	int fd;
};

CRUNCHpp_API bool isTTY;
#ifdef _MSC_VER
CRUNCHpp_API HANDLE console;
#endif
CRUNCHpp_API testLog *logger;

CRUNCHpp_API size_t vaTestPrintf(const char *format, va_list args);
CRUNCHpp_API size_t testPrintf(const char *format, ...);
CRUNCHpp_API int getColumns();
CRUNCHpp_API void echoAborted();
CRUNCHpp_API void logResult(resultType type, const char *message, ...);
CRUNCHpp_API void newline();

#define COLOUR(Code) "\x1B[" Code "m"
#define NORMAL COLOUR("0;39")
#define SUCCESS COLOUR("1;32")
#define FAILURE COLOUR("1;31")
#define WARNING COLOUR("1;33")
#define BRACKET COLOUR("1;34")
#define INFO COLOUR("1;36")

#define CURS_UP "\x1B[1A"
#define SET_COL "\x1B[%dG"

#define NEWLINE NORMAL "\n"

#endif /* __LOGGER_H__ */
