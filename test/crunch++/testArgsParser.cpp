// SPDX-License-Identifier: LGPL-3.0-or-later
#include <substrate/utility>
#include <crunch++.h>
#include <argsParser.hxx>
#include <stringFuncs.hxx>

using namespace crunch;
using rawStrPtr_t = const char *;

class testArgsParser final : public testsuite
{
private:
	void testNull()
	{
		constexpr auto argv{substrate::make_array<rawStrPtr_t>({"test", "--dummy"})};

		registerArgs(nullptr);
		assertTrue(parseArguments(0, nullptr).empty());
		assertTrue(parseArguments(1, nullptr).empty());
		assertTrue(parseArguments(2, nullptr).empty());
		assertTrue(parseArguments(1, argv.data()).empty());
		assertTrue(parseArguments(2, argv.data()).empty());
		assertTrue(parseArguments(uint32_t(-1), nullptr).empty());
		assertTrue(parseArguments(uint32_t(-1), argv.data()).empty());
	}

	void testEmpty()
	{
		constexpr auto argv{substrate::make_array<rawStrPtr_t>({"test", "--dummy"})};
		constexpr auto args{substrate::make_array<arg_t>({{{}, 0, 0, 0}})};

		registerArgs(args.data());
		parsedArgs_t parsedArgs{parseArguments(2, argv.data())};
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 1);
		assertFalse(parsedArgs[0].value.empty());
		assertEqual(parsedArgs[0].value.data(), "--dummy");
		assertEqual(parsedArgs[0].paramsFound, 0);
		assertEqual(parsedArgs[0].flags, 0);
		assertTrue(parsedArgs[0].params.empty());
	}

	void testIncomplete()
	{
		constexpr auto argv_1{substrate::make_array<rawStrPtr_t>({"test", "--arg="})};
		constexpr auto argv_2{substrate::make_array<rawStrPtr_t>({"test", "--arg=test"})};
		constexpr auto args_1{substrate::make_array<arg_t>(
		{
			{"--arg="_sv, 0, 0, ARG_INCOMPLETE},
			{{}, 0, 0, 0}
		})};
		constexpr auto args_2{substrate::make_array<arg_t>(
		{
			{"--arg"_sv, 0, 0, 0},
			{{}, 0, 0, 0}
		})};

		parsedArgs_t parsedArgs{};
		registerArgs(args_1.data());

		parsedArgs = parseArguments(2, argv_1.data());
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 1);
		assertFalse(parsedArgs[0].value.empty());
		assertEqual(parsedArgs[0].value.data(), "--arg=");
		assertEqual(parsedArgs[0].paramsFound, 0);
		assertEqual(parsedArgs[0].flags, ARG_INCOMPLETE);
		assertTrue(parsedArgs[0].params.empty());

		parsedArgs = parseArguments(2, argv_2.data());
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 1);
		assertFalse(parsedArgs[0].value.empty());
		assertEqual(parsedArgs[0].value.data(), "--arg=test");
		assertEqual(parsedArgs[0].flags, ARG_INCOMPLETE);
		assertTrue(parsedArgs[0].params.empty());

		startLogging("/dev/null");
		registerArgs(args_2.data());
		assertFalse(parseArguments(2, argv_1.data()).empty());
		assertFalse(parseArguments(2, argv_2.data()).empty());
	}

	void testInvalid() try
	{
		constexpr auto argv{substrate::make_array<rawStrPtr_t>({"test", "--arg", "--arg"})};
		constexpr auto args{substrate::make_array<arg_t>(
		{
			{"--arg"_sv, 0, 0, 0},
			{{}, 0, 0, 0}
		})};
		parsedArgs_t parsedArgs{1};

		registerArgs(args.data());
		assertFalse(parsedArgs.empty());
		parsedArg_t &parsedArg{parsedArgs[0]};
		parsedArg.value = "--arg"_sv;
		assertTrue(checkAlreadyFound(parsedArgs, parsedArg));

		assertNull(findArg({}, ""_sv, nullptr));

		// This checks that duplicate parameters work correctly by dropping the second copy of the parameter
		startLogging("/dev/null");
		parsedArgs = parseArguments(3, argv.data());
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 1);
	}
	catch (std::bad_alloc &e)
		{ fail(e.what()); }

	void testArgCounting()
	{
		constexpr auto argv_1{substrate::make_array<rawStrPtr_t>({"test", "-o", "test", "-a", "-o"})};
		constexpr auto argv_2{substrate::make_array<rawStrPtr_t>({"test", "-o", "test", "me", "please"})};
		constexpr auto args{substrate::make_array<arg_t>(
		{
			{"-o"_sv, 0, 2, 0},
			{"-a"_sv, 1, 1, 0},
			{{}, 0, 0, 0}
		})};
		parsedArgs_t parsedArgs{};

		registerArgs(args.data());
		assertEqual(checkParams(5, argv_1.data(), 2, args[0], args.data()), 1);
		assertEqual(checkParams(5, argv_1.data(), 4, args[1], args.data()), -1);
		assertEqual(checkParams(5, argv_1.data(), 5, args[0], args.data()), 0);
		assertEqual(checkParams(5, argv_2.data(), 2, args[0], args.data()), 2);

		auto *log{startLogging("/dev/null")};
		parsedArgs = parseArguments(5, argv_1.data());
		stopLogging(log);
		assertTrue(parsedArgs.empty());

		parsedArgs = parseArguments(5, argv_2.data());
		assertFalse(parsedArgs.empty());
		assertEqual(parsedArgs.size(), 2);
		assertFalse(parsedArgs[0].value.empty());
		assertEqual(parsedArgs[0].value.data(), "-o");
		assertEqual(parsedArgs[0].paramsFound, 2);
		assertFalse(parsedArgs[0].params[0].empty());
		assertFalse(parsedArgs[0].params[1].empty());
		assertEqual(parsedArgs[0].params[0].data(), "test");
		assertEqual(parsedArgs[0].params[1].data(), "me");
	}

public:
	void registerTests() final
	{
		CXX_TEST(testNull)
		CXX_TEST(testEmpty)
		CXX_TEST(testIncomplete)
		CXX_TEST(testInvalid)
		CXX_TEST(testArgCounting)
	}
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<testArgsParser>();
}
