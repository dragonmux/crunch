/*
 * This file is part of crunch
 * Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * crunch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * crunch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "crunch.h"

typedef enum _resultType
{
	RESULT_FAILURE,
	RESULT_SUCCESS,
	RESULT_ABORT
} resultType;

CRUNCH_API const int errAbort;
CRUNCH_API uint8_t isTTY;
#ifdef _MSC_VER
CRUNCH_API HANDLE console;
#endif
CRUNCH_API uint8_t logging;
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

#endif /* __LOGGER_H__ */
