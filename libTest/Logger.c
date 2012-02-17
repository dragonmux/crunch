#include "Core.h"
#include "Logger.h"
#include "Memory.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <sys/file.h>

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

#ifdef _MSC_VER
	#define TTY	"CON"
#else
	#define TTY	"/dev/tty"
#endif

FILE *realStdout = NULL;

int getColumns()
{
	struct winsize win;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
	return (win.ws_col == 0 ? 80 : win.ws_col);
}

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

void echoOk()
{
	testPrintf(CURS_UP SET_COL BRACKET "[" SUCCESS "  OK  " BRACKET "]" NEWLINE, COL(getColumns()));
	passes++;
}

void echoFailure()
{
	testPrintf(CURS_UP SET_COL BRACKET "[" FAILURE " FAIL " BRACKET "]" NEWLINE, COL(getColumns()));
	failures++;
}

void echoAborted()
{
	testPrintf(BRACKET "[" FAILURE " **** ABORTED **** " BRACKET "]" NEWLINE);
	libDebugExit(0);
}

void logResult(resultType type, const char *message, ...)
{
	va_list args;

	testPrintf(NORMAL);
	va_start(args, message);
	vaTestPrintf(message, args);
	va_end(args);
	if (type != RESULT_SUCCESS)
		testPrintf(NEWLINE);
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

log *startLogging(const char *fileName)
{
	log *ret = testMalloc(sizeof(log));
	ret->stdout = dup(STDOUT_FILENO);
	realStdout = fdopen(ret->stdout, "w");
	ret->file = freopen(fileName, "w", stdout);
	ret->fd = fileno(ret->file);
	flock(ret->fd, LOCK_EX);
	return ret;
}

void stopLogging(log *logFile)
{
	flock(logFile->fd, LOCK_UN);
	fclose(logFile->file);
	fclose(realStdout);
	realStdout = freopen(TTY, "w", stdout);
	free(logFile);
}
