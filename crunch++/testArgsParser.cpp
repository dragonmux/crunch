#include <crunch++.h>
#include "ArgsParser.h"

using rawStrPtr_t = const char *;
using strPtr_t = std::unique_ptr<const char []>;

class testArgsParser : public testsuit
{
public:
	void testNull()
	{
		const rawStrPtr_t argv[2] = {"test", "--dummy"};

		registerArgs(nullptr);
		assertNull(parseArguments(0, nullptr));
		assertNull(parseArguments(1, nullptr));
		assertNull(parseArguments(2, nullptr));
		assertNull(parseArguments(1, argv));
		assertNull(parseArguments(2, argv));
		assertNull(parseArguments(uint32_t(-1), nullptr));
		assertNull(parseArguments(uint32_t(-1), argv));
	}

	void testEmpty()
	{
		const rawStrPtr_t argv[2] = {"test", "--dummy"};
		const arg_t args[1] = { {nullptr, 0, 0, 0} };

		registerArgs(args);
		parsedArgs_t parsedArgs = parseArguments(2, argv);
		assertNotNull(parsedArgs);
		assertNotNull(parsedArgs[0]);
		assertNotNull(parsedArgs[0]->value);
		assertEqual(parsedArgs[0]->value.get(), "--dummy");
		assertEqual(parsedArgs[0]->paramsFound, 0);
		assertEqual(parsedArgs[0]->flags, 0);
		assertNull(parsedArgs[0]->params);
	}

	void testIncomplete()
	{
		const rawStrPtr_t argv_1[2] = {"test", "--arg="};
		const rawStrPtr_t argv_2[2] = {"test", "--arg=test"};
		const arg_t args_1[2] =
		{
			{"--arg=", 0, 0, ARG_INCOMPLETE},
			{nullptr, 0, 0, 0}
		};
		const arg_t args_1[2] =
		{
			{"--arg", 0, 0, 0},
			{nullptr, 0, 0, 0}
		};

		parsedArgs_t parsedArgs;
		registerArgs(args_1);

		parsedArgs = parseArguments(2, argv_1);
		assertNotNull(parsedArgs);
		assertNotNull(parsedArgs[0]);
		assertNotNull(parsedArgs[0]->value);
		assertEqual(parsedArgs[0]->value.get(), "--arg=");
		assertEqual(parsedArgs[0]->paramsFound, 0);
		assertEqual(parsedArgs[0]->flags, ARG_INCOMPLETE);
		assertNull(parsedArgs[0]->params);

		parsedArgs = parseArguments(2, argv_2);
		assertNotNull(parsedArgs);
		assertNotNull(parsedArgs[0]);
		assertNotNull(parsedArgs[0]->value);
		assertEqual(parsedArgs[0]->value.get(), "--arg=test");
		assertEqual(parsedArgs[0]->flags, ARG_INCOMPLETE);
		assertNull(parsedArgs[0]->params);

		startLogging("/dev/null");
		registerArgs(args_2);
		assertNull(parseArguments(2, argv_1));
		assertNull(parseArguments(2, argv_2));
	}

	void registerTests()
	{
		CXX_TEST(testNull)
		CXX_TEST(testEmpty)
		CXX_TEST(testIncomplete)
	}
};

CRUNCH_API void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<testArgsParser>();
}