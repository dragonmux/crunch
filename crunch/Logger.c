// SPDX-License-Identifier: LGPL-3.0-or-later
#ifdef __GNUC__
#define __USE_POSIX
#define _POSIX_SOURCE
#endif
#include "Core.h"
#include "Logger.h"
#ifndef _MSC_VER
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#else
#include <io.h>
#include <sys/locking.h>
#endif
#include <stdio.h>

#ifndef _MSC_VER
#define COL(val) ((val) - 8)
#else
#define COL(val) (uint16_t)((val) - 9)
#endif

#ifdef _MSC_VER
	#define TTY	"CON"
HANDLE console;
#else
	#define TTY	"/dev/tty"
#endif

struct testLog
{
	FILE *file;
	FILE *stdout_;
	FILE *realStdout;
	int fd;
};

testLog *logger = NULL;
uint8_t isTTY = 1;

#ifndef _MSC_VER
int getColumns()
{
	struct winsize win;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
	return (win.ws_col == 0 ? 80 : win.ws_col);
}
#else
uint16_t getColumns()
{
	CONSOLE_SCREEN_BUFFER_INFO window;
	GetConsoleScreenBufferInfo(console, &window);
	return window.dwSize.X;
}
#endif

size_t vaTestPrintf(const char *format, va_list args)
	{ return vfprintf(logger ? logger->stdout_ : stdout, format, args); }

size_t testPrintf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	const size_t ret = vaTestPrintf(format, args);
	va_end(args);
	return ret;
}

void printOk()
{
	testPrintf("[  OK  ]\n");
}

void printFailure()
{
	testPrintf("[ FAIL ]\n");
}

void printAborted()
{
	testPrintf("[ **** ABORTED **** ]\n");
}

#ifndef _MSC_VER
void echoOk()
{
	if (isTTY != 0)
		testPrintf(CURS_UP SET_COL BRACKET "[" SUCCESS "  OK  " BRACKET "]" NEWLINE, COL(getColumns()));
	else
		printOk();
	passes++;
}

void echoFailure()
{
	if (isTTY != 0)
		testPrintf(" " SET_COL BRACKET "[" FAILURE " FAIL " BRACKET "]" NEWLINE, COL(getColumns()));
	else
		printFailure();
	failures++;
}

void echoAborted()
{
	if (isTTY != 0)
		testPrintf("\n" BRACKET "[" FAILURE " **** ABORTED **** " BRACKET "]" NEWLINE);
	else
		printAborted();
	thrd_exit(THREAD_ABORT);
}
#else
void echoOk()
{
	if (isTTY != 0)
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
	if (isTTY != 0)
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

void echoAborted()
{
	if (isTTY != 0)
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
	thrd_exit(THREAD_ABORT);
}
#endif

void logResult(resultType type, const char *message, ...)
{
	va_list args;

	if (isTTY != 0)
#ifndef _MSC_VER
		testPrintf(NORMAL);
#else
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
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
		default:
			echoAborted();
	}
}

testLog *startLogging(const char *fileName)
{
	if (logger || !fileName)
		return NULL;
	testLog *logger_ = malloc(sizeof(testLog));
	if (!logger_)
		return NULL;
	logger_->file = fopen(fileName, "w");
	if (!logger_->file)
	{
		free(logger_);
		return NULL;
	}
	logger_->realStdout = stdout;
#ifndef _MSC_VER
	logger_->fd = dup(STDOUT_FILENO);
	stdout = logger_->file;
#else
	logger_->fd = dup(fileno(stdout));
#endif
	if (logger_->fd == -1)
	{
		stdout = logger_->realStdout;
		free(logger_);
		return NULL;
	}
	logger = logger_;
	logger_->stdout_ = fdopen(logger_->fd, "w");
	const int fileFD = fileno(logger_->file);
#ifndef _MSC_VER
	flock(fileFD, LOCK_EX);
	dup2(fileFD, STDOUT_FILENO);
#else
//	locking(fileFD, LK_LOCK, -1);
	dup2(fileFD, fileno(logger_->realStdout));
#endif
	return logger_;
}

void stopLogging(testLog *logger_)
{
	if (!logger_ || logger_ != logger)
		return;
#ifndef _MSC_VER
	dup2(logger_->fd, STDOUT_FILENO);
	flock(fileno(logger_->file), LOCK_UN);
	stdout = logger_->realStdout;
#else
	dup2(logger_->fd, fileno(logger_->realStdout));
//	locking(fileno(logger_->file), LK_UNLCK, -1);
#endif
	logger = NULL;
	fclose(logger_->stdout_);
	fclose(logger_->file);
	free(logger_);
}
