// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef LOGGER__HXX
#define LOGGER__HXX

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "crunch++.h"
#include <cstdarg>

namespace crunch
{
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
#ifdef _WIN32
	CRUNCHpp_API HANDLE console;
#endif
	CRUNCHpp_API testLog *logger;

	CRUNCHpp_API size_t vaTestPrintf(const char *format, va_list args);
	CRUNCHpp_API size_t testPrintf(const char *format, ...);
	CRUNCHpp_API int16_t getColumns();
	CRUNCHpp_API void echoAborted();
	CRUNCHpp_API void logResult(resultType type, const char *message, ...);
	CRUNCHpp_API void newline();
} // namespace crunch

#define COLOUR(Code) "\x1B[" Code "m"
#define NORMAL COLOUR("0;39")
#define SUCCESS COLOUR("1;32")
#define FAILURE COLOUR("1;31")
#define WARNING COLOUR("1;33")
#define BRACKET COLOUR("1;34")
#define INFO COLOUR("1;36")

#define CURS_UP "\x1B[1A"
#define SET_COL "\x1B[%hiG"

#define NEWLINE NORMAL "\n"

#endif /*LOGGER__HXX*/
