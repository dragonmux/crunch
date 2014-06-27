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

#include "Core.h"
#include "Logger.h"
#include "Memory.h"
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#endif
#include <stdarg.h>

#define COL(val) val - 8
#define WCOL(val) val - 2

#ifdef _MSC_VER
	#define TTY	"CON"
HANDLE console;
#else
	#define TTY	"/dev/tty"
#endif

FILE *realStdout = NULL;
uint8_t logging = 0;
log *logger = NULL;
uint8_t isTTY = 1;

#ifndef _MSC_VER
int getColumns()
{
	struct winsize win;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
	return (win.ws_col == 0 ? 80 : win.ws_col);
}
#else
int getColumns()
{
	CONSOLE_SCREEN_BUFFER_INFO window;
	GetConsoleScreenBufferInfo(console, &window);
	return window.dwSize.X;
}
#endif

size_t vaTestPrintf(const char *format, va_list args)
{
	if (realStdout == NULL)
		realStdout = stdout;
	return vfprintf(realStdout, format, args);
}

size_t testPrintf(const char *format, ...)
{
	size_t ret;
	va_list args;
	va_start(args, format);
	ret = vaTestPrintf(format, args);
	va_end(args);
	return ret;
}

#ifndef _MSC_VER
void echoOk()
{
	if (isTTY != 0)
		testPrintf(CURS_UP SET_COL BRACKET "[" SUCCESS "  OK  " BRACKET "]" NEWLINE, COL(getColumns()));
	else
		testPrintf("[  OK  ]\n");
	passes++;
}

void echoFailure()
{
	if (isTTY != 0)
		testPrintf(CURS_UP SET_COL BRACKET "[" FAILURE " FAIL " BRACKET "]" NEWLINE, COL(getColumns()));
	else
		testPrintf("[ FAIL ]\n");
	failures++;
}

void echoAborted()
{
	if (isTTY != 0)
		testPrintf(BRACKET "[" FAILURE " **** ABORTED **** " BRACKET "]" NEWLINE);
	else
		testPrintf("[ **** ABORTED **** ]\n");
	libDebugExit(0);
}
#else
void echoOk()
{
	passes++;
}

void echoFailure()
{
	failures++;
}

void echoAborted()
{
	libDebugExit(0);
}
#endif

void logResult(resultType type, const char *message, ...)
{
	va_list args;

	if (isTTY != 0)
		testPrintf(NORMAL);
	va_start(args, message);
	vaTestPrintf(message, args);
	va_end(args);
	if (type != RESULT_SUCCESS && isTTY != 0)
		testPrintf(NEWLINE);
	switch (type)
	{
		case RESULT_SUCCESS:
			echoOk();
			break;
		case RESULT_FAILURE:
			echoFailure();
			break;
		default:
			echoAborted();
	}
}

log *startLogging(const char *fileName)
{
	log *ret;
	if (logging == 1)
		return NULL;
	ret = testMalloc(sizeof(log));
	logging = 1;
	ret->stdout = dup(STDOUT_FILENO);
	realStdout = fdopen(ret->stdout, "w");
	ret->file = freopen(fileName, "w", stdout);
	ret->fd = fileno(ret->file);
	flock(ret->fd, LOCK_EX);
	logger = ret;
	return ret;
}

void stopLogging(log *logFile)
{
	if (logFile == NULL)
		return;
	flock(logFile->fd, LOCK_UN);
	fclose(logFile->file);
	fclose(realStdout);
	realStdout = freopen(TTY, "w", stdout);
	free(logFile);
	logger = NULL;
	logging = 0;
}
