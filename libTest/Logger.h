#ifndef __LOGGER_H__
#define __LOGGER_H__

typedef enum _logType
{
	LOG_FAILURE,
	LOG_SUCCESS
} logType;

extern void log(logType type, char *message);

#endif /* __LOGGER_H__ */
