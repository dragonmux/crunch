#include "Core.h"
#include "Logger.h"

#define COLOUR(Code) "\x1B["Code"m"
#define NORMAL COLOUR("0;39")
#define SUCCESS COLOUR("1;32")
#define FAILURE COLOUR("1;31")
#define BRACKET COLOUR("1;34")

#define CURS_UP "\x1B[1A\x1B[0G"
#define SET_COL "\x1B[%dG"

#define NEWLINE NORMAL "\x1B[1A\x1B[s\x1B[1B\n"
#define MOVE_END_PRINTED "\x1B[u"

void echoOk()
{
	printf(CURS_UP SET_COL BRACKET "[" SUCCESS "  OK  " BRACKET "]" NEWLINE, COL(getColumns()));
}

void echoFailure()
{
	printf(CURS_UP SET_COL BRACKET "[" FAILURE " FAIL " BRACKET "]" NEWLINE, COL(getColumns()));
}

void echoAborted()
{
	printf(BRACKET "[" FAILURE " **** ABORTED **** " BRACKET "]" NEWLINE);
	libDebugExit(0);
}

void log(logType type, char *message)
{
	printf(NORMAL "%s" NEWLINE, message);
	switch (type)
	{
		case LOG_SUCCESS:
			return echoOk();
		case LOG_FAIURE:
			return echoFailure();
		default:
			echoAborted();
	}
}
