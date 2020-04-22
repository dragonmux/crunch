// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstring>
#include <cinttypes>
#ifndef _MSC_VER
#include <dlfcn.h>
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <io.h>
#define RTLD_LAZY 0
#define dlopen(fileName, flag) (void *)LoadLibrary(fileName)
#define dlsym(handle, symbol) GetProcAddress(HMODULE(handle), symbol)
#define dlclose(handle) FreeLibrary(HMODULE(handle))

char *dlerror()
{
	const auto error = GetLastError();
	char *message;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, MAKELANGID(LANG_NEUTRAL,
		SUBLANG_DEFAULT), reinterpret_cast<char *>(&message), 0, nullptr);
	return message;
}

#include <crtdbg.h>
#endif
#include <exception>
#include <cstdlib>
#include "core.hxx"
#include "logger.hxx"
#include "argsParser.hxx"
#include "stringFuncs.hxx"
#include "crunch++.h"
#include <version.hxx>

using namespace std;

const arg_t args[] =
{
	{"--log", 1, 1, 0},
	{"--help", 0, 0, 0},
	{"-h", 0, 0, 0},
	{"--version", 0, 0, 0},
	{"-v", 0, 0, 0},
	{{}, 0, 0, 0}
};

#ifdef _MSC_VER
const static auto libExt{"tlib"_s};
#else
const static auto libExt{"so"_s};
#endif

struct freeDelete_t final
{
	void operator ()(void *const ptr) noexcept
		{ std::free(ptr); } // NOLINT(cppcoreguidelines-no-malloc,cppcoreguidelines-owning-memory,hicpp-no-malloc)
};

parsedArgs_t parsedArgs;
parsedRefArgs_t namedTests;
size_t numTests = 0;
std::unique_ptr<char, freeDelete_t> workingDir = {};

using registerFn = void (__cdecl *)();

void red()
{
	if (isTTY)
#ifndef _MSC_VER
		testPrintf(FAILURE);
#else
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
}

void magenta()
{
	if (isTTY)
#ifndef _MSC_VER
		testPrintf(COLOUR("1;35"));
#else
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
}

void printStats()
{
	uint64_t total = passes + failures;
	testPrintf("Total tests: %" PRIu64 ",  Failures: %" PRIu32 ",  Pass rate: ", total, failures);
	if (total == 0)
		testPrintf("--\n");
	else
		testPrintf("%0.2f%%\n", double(passes) / double(total) * 100.0);
}

bool getTests()
{
	namedTests.reserve(parsedArgs.size());
	for (const auto &parsedArg : parsedArgs)
	{
		// this might be as simple as (!parsedArg.minLength) now
		if (!findArgInArgs(parsedArg.value.data()))
			namedTests.push_back(&parsedArg);
	}
	namedTests.shrink_to_fit();
	numTests = namedTests.size();
	return !namedTests.empty();
}

bool tryRegistration(void *testSuite) try
{
	const auto registerTests = reinterpret_cast<registerFn>(dlsym(testSuite, "registerCXXTests")); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) lgtm[cpp/reinterpret-cast]
	if (registerTests)
		registerTests();
	else
		dlclose(testSuite);
	return registerTests;
}
catch (std::bad_alloc &e)
{
	cxxTests.clear();
	red();
	testPrintf("Failed to allocate memory while performing test registration");
	newline();
	return false;
}

void runTests()
{
	testLog *logFile{};
	const auto *const logging{findArg(parsedArgs, "--log", nullptr)};
	if (logging)
	{
		logFile = startLogging(logging->params[0].data());
		loggingTests = true;
	}

	for (size_t i = 0; i < numTests; i++)
	{
		auto testLib = formatString("%s/%s.%s", workingDir.get(), namedTests[i]->value.data(), libExt.data());
		auto *testSuite{dlopen(testLib.get(), RTLD_LAZY)};
		if (!testSuite || !tryRegistration(testSuite))
		{
			if (!testSuite)
			{
				red();
				testPrintf("Could not open test library: %s", dlerror());
				newline();
			}
			red();
			testPrintf("Test library %s was not a valid library, skipping", namedTests[i]->value.data());
			newline();
			continue;
		}
		magenta();
		testPrintf("Running test suite %s...", namedTests[i]->value.data());
		newline();

		for (auto &test : cxxTests)
		{
			magenta();
			testPrintf("Running tests in class %s...", test.testClassName);
			newline();

			try { test.testClass->registerTests(); }
			catch (threadExit_t &) { continue; }
			catch (std::bad_alloc &)
			{
				red();
				testPrintf("Failed to allocate memory while registering suite");
				newline();
				continue;
			}

			try
				{ test.testClass->test(); }
			catch (threadExit_t &)
			{
				cxxTests.clear();
				printStats();
				if (logging != nullptr)
					stopLogging(logFile);
				throw;
			}
		}
		cxxTests.clear();
		cxxTests.shrink_to_fit();
	}

	printStats();
	if (logging != nullptr)
		stopLogging(logFile);
}

#ifdef _WINDOWS
void invalidHandler(const wchar_t *, const wchar_t *, const wchar_t *, const uint32_t, const uintptr_t) { }
#endif

bool handleVersionOrHelp()
{
	constParsedArg_t version{findArg(parsedArgs, "--version", nullptr)};
	constParsedArg_t versionShort{findArg(parsedArgs, "-v", nullptr)};
	constParsedArg_t help{findArg(parsedArgs, "--help", nullptr)};
	constParsedArg_t helpShort{findArg(parsedArgs, "-h", nullptr)};

	if (help || helpShort)
		puts(crunchpp::help.data());
	else if (version || versionShort)
		testPrintf("crunch++ %s (%s %s %s-%s)\n", crunchpp::version.data(), crunchpp::compiler.data(),
			crunchpp::compilerVersion.data(), crunchpp::system.data(), crunchpp::arch.data());
	else
		return false;
	return true;
}

int main(int argc, char **argv)
{
#if _WINDOWS
	_set_invalid_parameter_handler(invalidHandler);
	_CrtSetReportMode(_CRT_ASSERT, 0);
	_CrtSetReportMode(_CRT_ERROR, 0);
#endif
	registerArgs(args);
	parsedArgs = parseArguments(argc, argv);
	if (!parsedArgs.empty() && handleVersionOrHelp())
		return 0;
	else if (parsedArgs.empty() || !getTests())
	{
		testPrintf("Fatal error: There are no tests to run given on the command line!\n");
		return 2;
	}
	workingDir.reset(getcwd(nullptr, 0));
#ifndef _MSC_VER
	isTTY = isatty(STDOUT_FILENO);
#else
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!console)
	{
		printf("Error: could not grab console!");
		return 1;
	}
	isTTY = bool(isatty(fileno(stdout)));
#endif
	try { runTests(); }
	catch (threadExit_t &val)
		{ return val; }
	return failures ? 1 : 0;
}
