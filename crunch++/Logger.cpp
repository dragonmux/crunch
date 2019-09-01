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
#include "memory.hxx"
#ifndef _WINDOWS
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#else
#include <io.h>
#include <sys/locking.h>
#endif
#include <stdio.h>

#ifndef _WINDOWS
#define COL(val) val - 8
#else
#define COL(val) int16_t(val - 9)
#endif
#define WCOL(val) val - 2

#ifndef _WINDOWS
	#define TTY	"/dev/tty"
#else
	#define TTY	"CON"
HANDLE console;
#endif

testLog *logger = nullptr;
bool isTTY = true;

int getColumns()
{
#ifndef _WINDOWS
	struct winsize win{};
	ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
	return !win.ws_col ? 80 : win.ws_col;
#else
	CONSOLE_SCREEN_BUFFER_INFO window{};
	GetConsoleScreenBufferInfo(console, &window);
	return !window.dwSize.X ? 80 : window.dwSize.X;
#endif
}

size_t vaTestPrintf(const char *format, va_list args)
{
	const auto ret = vfprintf(logger ? logger->stdout_ : stdout, format, args);
	fflush(logger ? logger->stdout_ : stdout);
	return ret;
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

void printOk() { testPrintf(" [  OK  ]\n"); }
void printFailure() { testPrintf(" [ FAIL ]\n"); }
void printSkip() { testPrintf(" [ SKIP ]\n"); }
void printAborted() { testPrintf("[ **** ABORTED **** ]\n"); }

void normal()
{
#ifndef _WINDOWS
	testPrintf(NORMAL);
#else
	SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

#ifndef _WINDOWS
void echoOk()
{
	if (isTTY)
		testPrintf(CURS_UP SET_COL BRACKET "[" SUCCESS "  OK  " BRACKET "]" NEWLINE, COL(getColumns()));
	else
		printOk();
	++passes;
}

void echoFailure()
{
	if (isTTY)
		testPrintf(" " SET_COL BRACKET "[" FAILURE " FAIL " BRACKET "]" NEWLINE, COL(getColumns()));
	else
		printFailure();
	++failures;
}

void echoSkip()
{
	if (isTTY)
		testPrintf(" " SET_COL BRACKET "[" WARNING " SKIP " BRACKET "]" NEWLINE, COL(getColumns()));
	else
		printSkip();
	++passes;
}

void echoAborted()
{
	if (isTTY)
		testPrintf("\n" BRACKET "[" FAILURE " **** ABORTED **** " BRACKET "]" NEWLINE);
	else
		printAborted();
	throw threadExit_t(2);
}
#else
void echoOk()
{
	if (isTTY)
	{
		CONSOLE_SCREEN_BUFFER_INFO cursor;
		GetConsoleScreenBufferInfo(console, &cursor);
		cursor.dwCursorPosition.Y--;
		cursor.dwCursorPosition.X = COL(getColumns());
		SetConsoleCursorPosition(console, cursor.dwCursorPosition);
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		testPrintf("[");
		SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		testPrintf("  OK  ");
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		testPrintf("]");
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		testPrintf("\n");
	}
	else
		printOk();
	passes++;
}

void echoFailure()
{
	if (isTTY)
	{
		CONSOLE_SCREEN_BUFFER_INFO cursor;
		GetConsoleScreenBufferInfo(console, &cursor);
		cursor.dwCursorPosition.X = COL(getColumns());
		SetConsoleCursorPosition(console, cursor.dwCursorPosition);
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		testPrintf("[");
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
		testPrintf(" FAIL ");
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		testPrintf("]");
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		testPrintf("\n");
	}
	else
		printFailure();
	failures++;
}

void echoSkip()
{
	if (isTTY)
	{
		CONSOLE_SCREEN_BUFFER_INFO cursor;
		GetConsoleScreenBufferInfo(console, &cursor);
		cursor.dwCursorPosition.X = COL(getColumns());
		SetConsoleCursorPosition(console, cursor.dwCursorPosition);
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		testPrintf("[");
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		testPrintf(" SKIP ");
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		testPrintf("]");
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		testPrintf("\n");
	}
	else
		printSkip();
}

void echoAborted()
{
	if (isTTY)
	{
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		testPrintf("[");
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
		testPrintf(" **** ABORTED **** ");
		SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		testPrintf("]");
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		testPrintf("\n");
	}
	else
		printAborted();
	throw threadExit_t(2);
}
#endif

void logResult(resultType type, const char *message, ...)
{
	va_list args;

	if (isTTY)
		normal();
	va_start(args, message);
	vaTestPrintf(message, args);
	va_end(args);
	switch (type)
	{
		case RESULT_SUCCESS:
			echoOk();
			break;
		case RESULT_FAILURE:
			echoFailure();
			break;
		case RESULT_SKIP:
			echoSkip();
			break;
		default:
			echoAborted();
	}
}

testLog *startLogging(const char *fileName)
{
	if (logger)
		return nullptr;
	auto logger_ = makeUnique<testLog>();
	logger_->file = fopen(fileName, "w");
	if (!logger_->file)
		return nullptr;
	logger_->realStdout = stdout;
#ifndef _MSC_VER
	logger_->fd = dup(STDOUT_FILENO);
	stdout = logger_->file;
#else
	logger_->fd = dup(fileno(stdout));
#endif
	logger = logger_.get();
	logger_->stdout_ = fdopen(logger_->fd, "w");
	const int fileFD = fileno(logger_->file);
#ifndef _MSC_VER
	flock(fileFD, LOCK_EX);
	dup2(fileFD, STDOUT_FILENO);
#else
//	locking(fileFD, LK_LOCK, -1);
	dup2(fileFD, fileno(logger_->realStdout));
#endif
	return logger_.release();
}

void stopLogging(testLog *loggerPtr)
{
	if (!loggerPtr || loggerPtr != logger)
		return;
	std::unique_ptr<testLog> logger_{loggerPtr};
#ifndef _MSC_VER
	dup2(logger_->fd, STDOUT_FILENO);
	flock(fileno(logger_->file), LOCK_UN);
	stdout = logger_->realStdout;
#else
	dup2(logger_->fd, fileno(logger_->realStdout));
//	locking(fileno(logger_->file), LK_UNLCK, -1);
#endif
	logger = nullptr;
	fclose(logger_->stdout_);
	fclose(logger_->file);
}
