// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef LOGGER__H
#define LOGGER__H

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "crunch.h"
#include <stdarg.h>

typedef enum _resultType
{
	RESULT_FAILURE,
	RESULT_SUCCESS,
	RESULT_ABORT
} resultType;

CRUNCH_API uint8_t isTTY;
#ifdef _MSC_VER
CRUNCH_API HANDLE console;
#endif
CRUNCH_API testLog *logger;

CRUNCH_API size_t vaTestPrintf(const char *format, va_list args);
CRUNCH_API size_t testPrintf(const char *format, ...);
CRUNCH_API void logResult(resultType type, const char *message, ...);

#define COLOUR(Code) "\x1B[" Code "m"
#define NORMAL COLOUR("0;39")
#define SUCCESS COLOUR("1;32")
#define FAILURE COLOUR("1;31")
#define BRACKET COLOUR("1;34")
#define INFO COLOUR("1;36")

#define CURS_UP "\x1B[1A"
#define SET_COL "\x1B[%dG"

#define NEWLINE NORMAL "\n"

#endif /*LOGGER__H*/
