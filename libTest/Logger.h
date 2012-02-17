#ifndef __LOGGER_H__
#define __LOGGER_H__

typedef enum _resultType
{
	RESULT_FAILURE,
	RESULT_SUCCESS
} resultType;

extern void logResult(resultType type, char *message, ...);

#endif /* __LOGGER_H__ */
