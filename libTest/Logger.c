#include "Core.h"
#include "Logger.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdarg.h>

#define COLOUR(Code) "\x1B["Code"m"
#define NORMAL COLOUR("0;39")
#define SUCCESS COLOUR("1;32")
#define FAILURE COLOUR("1;31")
#define BRACKET COLOUR("1;34")

#define CURS_UP "\x1B[1A\x1B[0G"
#define SET_COL "\x1B[%dG"

#define NEWLINE NORMAL "\x1B[1A\x1B[s\x1B[1B\n"
#define MOVE_END_PRINTED "\x1B[u"

#define COL(val) val - 8
#define WCOL(val) val - 2

int getColumns()
{
	struct winsize win;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
	return (win.ws_col == 0 ? 80 : win.ws_col);
}

void echoOk()
{
	printf(CURS_UP SET_COL BRACKET "[" SUCCESS "  OK  " BRACKET "]" NEWLINE, COL(getColumns()));
	passes++;
}

void echoFailure()
{
	printf(CURS_UP SET_COL BRACKET "[" FAILURE " FAIL " BRACKET "]" NEWLINE, COL(getColumns()));
	failures++;
}

void echoAborted()
{
	printf(BRACKET "[" FAILURE " **** ABORTED **** " BRACKET "]" NEWLINE);
	libDebugExit(0);
}

void logResult(resultType type, char *message, ...)
{
	va_list args;

	printf(NORMAL);
	va_start(args, message);
	vprintf(message, args);
	va_end(args);
	if (type != RESULT_SUCCESS)
		printf(NEWLINE);
	switch (type)
	{
		case RESULT_SUCCESS:
			return echoOk();
		case RESULT_FAILURE:
			return echoFailure();
		default:
			echoAborted();
	}
}
