#ifndef __LOGGER_H__
#define __LOGGER_H__

typedef enum _resultType
{
	RESULT_FAILURE,
	RESULT_SUCCESS
} resultType;

extern uint8_t isTTY;
extern uint8_t logging;
extern log *logger;

extern size_t vaTestPrintf(const char *format, va_list args);
extern size_t testPrintf(const char *format, ...);
extern void logResult(resultType type, const char *message, ...);

#define COLOUR(Code) "\x1B["Code"m"
#define NORMAL COLOUR("0;39")
#define SUCCESS COLOUR("1;32")
#define FAILURE COLOUR("1;31")
#define BRACKET COLOUR("1;34")
#define INFO COLOUR("1;36")

#define CURS_UP "\x1B[1A\x1B[0G"
#define SET_COL "\x1B[%dG"

#define NEWLINE NORMAL "\x1B[1A\x1B[s\x1B[1B\n"
#define MOVE_END_PRINTED "\x1B[u"

#endif /* __LOGGER_H__ */
