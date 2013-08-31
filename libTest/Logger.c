#include "Core.h"
#include "Logger.h"
#include "Memory.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <sys/file.h>

#define COL(val) val - 8
#define WCOL(val) val - 2

#ifdef _MSC_VER
	#define TTY	"CON"
#else
	#define TTY	"/dev/tty"
#endif

FILE *realStdout = NULL;
uint8_t logging = 0;
log *logger = NULL;
uint8_t isTTY = 1;

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
