#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>

typedef enum _resultType
{
	RESULT_FAILURE,
	RESULT_SUCCESS
} resultType;

typedef struct _log
{
	FILE *file;
	int fd, stdout;
} log;

extern size_t vaTestPrintf(const char *format, va_list args);
extern size_t testPrintf(char *format, ...);
extern void logResult(resultType type, char *message, ...);
extern log *startLogging(char *fileName);
extern void stopLogging(log *logFile);

#endif /* __LOGGER_H__ */
