// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstdio>
#include <substrate/utility>
#ifndef _WINDOWS
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#else
#include <io.h>
#include <sys/locking.h>
#endif
#include "core.hxx"
#include "logger.hxx"

namespace crunch
{
#ifdef _WINDOWS
	HANDLE console;
#endif

	testLog *logger = nullptr;
	bool isTTY = true;

	int16_t getColumns()
	{
#ifndef _WINDOWS
		struct winsize win{};
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
		return (!win.ws_col ? 80 : win.ws_col) - 8;
#else
		CONSOLE_SCREEN_BUFFER_INFO window{};
		GetConsoleScreenBufferInfo(console, &window);
		return (!window.dwSize.X ? 80 : window.dwSize.X) - 8;
#endif
	}

	std::size_t vaTestPrintf(const char *format, va_list args)
	{
		const auto ret{vfprintf(logger ? logger->stdout_ : stdout, format, args)};
		fflush(logger ? logger->stdout_ : stdout);
		return ret;
	}

	std::size_t testPrintf(const char *format, ...) // NOLINT
	{
		va_list args;
		va_start(args, format);
		const auto result{vaTestPrintf(format, args)};
		va_end(args);
		return result;
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
			testPrintf(CURS_UP SET_COL BRACKET "[" SUCCESS "  OK  " BRACKET "]" NEWLINE, getColumns());
		else
			printOk();
		++passes;
	}

	void echoFailure()
	{
		if (isTTY)
			testPrintf(" " SET_COL BRACKET "[" FAILURE " FAIL " BRACKET "]" NEWLINE, getColumns());
		else
			printFailure();
		++failures;
	}

	void echoSkip()
	{
		if (isTTY)
			testPrintf(" " SET_COL BRACKET "[" WARNING " SKIP " BRACKET "]" NEWLINE, getColumns());
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
			cursor.dwCursorPosition.X = getColumns();
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
			cursor.dwCursorPosition.X = getColumns();
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
			cursor.dwCursorPosition.X = getColumns();
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

	void logResult(resultType type, const char *message, ...) // NOLINT
	{
		if (isTTY)
			normal();

		va_list args;
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
} // namespace crunch

using crunch::logger;

crunch::testLog *startLogging(const char *fileName)
{
	if (logger || !fileName)
		return nullptr;
	auto logger_ = substrate::make_unique<crunch::testLog>();
	logger_->file = fopen(fileName, "w"); // NOLINT(cppcoreguidelines-owning-memory)
	if (!logger_->file)
		return nullptr;
	logger_->realStdout = stdout;
#ifndef _MSC_VER
	logger_->fd = dup(STDOUT_FILENO);
	stdout = logger_->file;
#else
	logger_->fd = dup(fileno(stdout));
#endif
	if (logger_->fd == -1)
	{
#ifndef _MSC_VER
		stdout = logger_->realStdout;
#endif
		return nullptr;
	}
	logger = logger_.release();
	logger->stdout_ = fdopen(logger->fd, "w");
	const int fileFD = fileno(logger->file);
#ifndef _MSC_VER
	flock(fileFD, LOCK_EX);
	dup2(fileFD, STDOUT_FILENO);
#else
//	locking(fileFD, LK_LOCK, -1);
	dup2(fileFD, fileno(logger->realStdout));
#endif
	return logger;
}

void stopLogging(crunch::testLog *loggerPtr)
{
	if (!loggerPtr || loggerPtr != logger)
		return;
	std::unique_ptr<crunch::testLog> logger_{loggerPtr};
#ifndef _MSC_VER
	dup2(logger_->fd, STDOUT_FILENO);
	flock(fileno(logger_->file), LOCK_UN);
	stdout = logger_->realStdout;
#else
	dup2(logger_->fd, fileno(logger_->realStdout));
//	locking(fileno(logger_->file), LK_UNLCK, -1);
#endif
	logger = nullptr;
	fclose(logger_->stdout_); // NOLINT(cppcoreguidelines-owning-memory)
	fclose(logger_->file); // NOLINT(cppcoreguidelines-owning-memory)
}
