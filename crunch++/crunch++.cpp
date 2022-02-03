// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstring>
#include <cinttypes>
#ifndef _WIN32
#include <dlfcn.h>
#include <unistd.h>
#include <csignal>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#if defined(__MINGW32__) || defined(__MINGW64__)
#include <unistd.h>
#else
#include <io.h>
constexpr static const auto R_OK{0x04};
#endif
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
#include <utility>
#include <substrate/utility>
#include "core.hxx"
#include "logger.hxx"
#include "argsParser.hxx"
#include "stringFuncs.hxx"
#include "crunch++.h"
#include <version.hxx>

using namespace std;

namespace crunch
{
	constexpr auto args{substrate::make_array<arg_t>(
	{
		{"--log"_sv, 1, 1, 0},
		{"--help"_sv, 0, 0, 0},
		{"-h"_sv, 0, 0, 0},
		{"--version"_sv, 0, 0, 0},
		{"-v"_sv, 0, 0, 0},
		{{}, 0, 0, 0}
	})};

#ifdef _WIN32
	static const auto libExt{substrate::make_array<internal::stringView>(
	{
		"dll"_sv, "so"_sv, "tlib"_sv
	})};
	static const std::size_t libExtMaxLength{4U};
#else
	static const auto libExt{substrate::make_array<internal::stringView>({"so"_sv})};
	static const std::size_t libExtMaxLength{2U};
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

	using registerFn = void (*)();

	void red()
	{
		if (isTTY)
#ifndef _WIN32
			testPrintf(FAILURE);
#else
			SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif
	}

	void magenta()
	{
		if (isTTY)
#ifndef _WIN32
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
			if (!findArgInArgs(parsedArg.value))
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
	catch (const std::bad_alloc &)
	{
		cxxTests.clear();
		red();
		testPrintf("Failed to allocate memory while performing test registration");
		newline();
		return false;
	}

	std::unique_ptr<char[]> extForLibrary(const internal::stringView &test)
	{
		const auto lengthWorkingDir{std::char_traits<char>::length(workingDir.get())};
		// + 3 on the end to account for one '/', '.' and NUL termination
		auto library{substrate::make_unique_nothrow<char[]>(lengthWorkingDir + test.length() + libExtMaxLength + 3)};
		memcpy(library.get(), workingDir.get(), lengthWorkingDir);
		auto offset{lengthWorkingDir};
		library[offset++] = '/';
		memcpy(library.get() + offset, test.data(), test.length());
		offset += test.length();
		library[offset++] = '.';
		for (const auto ext : libExt)
		{
			memcpy(library.get() + offset, ext.data(), ext.length() + 1);
			if (!access(library.get(), R_OK))
				return library;
		}
		return {};
	}

	void runTests()
	{
		testLog *logFile{};
		const auto *const logging{findArg(parsedArgs, "--log"_sv, nullptr)};
		if (logging)
		{
			logFile = startLogging(logging->params[0].data());
			loggingTests = true;
		}

		for (size_t i{0}; i < numTests; i++)
		{
			auto testLib{extForLibrary(namedTests[i]->value)};
			if (!testLib)
			{
				red();
				testPrintf("Test library %s does not exist, skipping", namedTests[i]->value.data());
				newline();
				continue;
			}
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
				testPrintf("Running tests in class %s...", test.name());
				newline();

				try { test.suite()->registerTests(); }
				catch (threadExit_t &) { continue; }
				catch (std::bad_alloc &)
				{
					red();
					testPrintf("Failed to allocate memory while registering suite");
					newline();
					continue;
				}

				try
					{ test.suite()->test(); }
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
#else
	// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
	const char *strsicode(const int32_t sig, const int32_t code) noexcept
	{
		switch(sig)
		{
			case SIGILL:
				/* SIGILL codes */
				switch (code)
				{
					case ILL_ILLOPC:
						return "Illegal opcode.";
					case ILL_ILLOPN:
						return "Illegal operand.";
					case ILL_ILLADR:
						return "Illegal addressing mode.";
					case ILL_ILLTRP:
						return "Illegal trap.";
					case ILL_PRVOPC:
						return "Privileged opcode.";
					case ILL_PRVREG:
						return "Privileged register.";
					case ILL_COPROC:
						return "Coprocessor error.";
					case ILL_BADSTK:
						return "Internal stack error.";
					default:
						return "Unknown SIGILL Code";
				}
				break;
			case SIGFPE:
				/* SIGSFPE codes */
				switch (code)
				{
					case FPE_INTDIV:
						return "Integer divide by zero.";
					case FPE_INTOVF:
						return "Integer overflow.";
					case FPE_FLTDIV:
						return "Floating-point divide by zero.";
					case FPE_FLTOVF:
						return "Floating-point overflow.";
					case FPE_FLTUND:
						return "Floating-point underflow.";
					case FPE_FLTRES:
						return "Floating-point inexact result.";
					case FPE_FLTINV:
						return "Floating-point invalid operation.";
					case FPE_FLTSUB:
						return "Subscript out of range.";
					default:
						return "Unknown SIGFPE Code";
				}
				break;
			case SIGSEGV:
				/* SIGSEGV codes */
				switch (code)
				{
					case SEGV_MAPERR:
						return "Address not mapped to object.";
					case SEGV_ACCERR:
						return "Invalid permissions for mapped object.";
					case SEGV_BNDERR:
						return "Failed address bound checks.";
					case SEGV_PKUERR:
						return "Access was denied by memory protection keys.";
					default:
						return "Unknown SIGSEGV Code";
				}
				break;
			case SIGTRAP:
				// /* SIGTRAP codes */
				// switch(code) {
				//      case TRAP_BRKPT: return "Process breakpoint.";
				//      case TRAP_TRACE: return "Process trace trap.";
				//      case TRAP_BRANCH: return "Process taken branch trap.";
				//      case TRAP_HWBKPT: return "Hardware breakpoint/watchpoint."
				//      default: return "Unknown SIGTRAP Code";
				// }
				// break;
				return "Unknown SIGTRAP Code";
			default:
				return "Unknown Signal";
		}
	}

	void trapHandler(const int, siginfo_t *const info, void *const context)
	{
		const auto *const ctx{static_cast<ucontext_t *>(context)};
		const auto &mctx{ctx->uc_mcontext};

		fprintf(stderr, R"(
****TRAP HANDLER****
	SIG #: %d
	SCODE: %d
	 NAME: %s
	VADDR: %p
	VASRT: %p
	VAEND: %p

REASON FOR SIGNAL:
	%s

REGISTERS:
	RIP: %016llX	RSP: %016llX	RBP: %016llX
	RSI: %016llX	RDI: %016llX	EFL: %016llX
	SEG: %016llX	ERR: %016llX	CR2: %016llX
	RAX: %016llX	RBX: %016llX	RCX: %016llX
	RDX: %016llX	 R8: %016llX	 R9: %016llX
	R10: %016llX	R11: %016llX	R12: %016llX
	R13: %016llX	R14: %016llX	R15: %016llX
)",
			// NOLINTNEXTLINE(concurrency-mt-unsafe)
			info->si_signo, info->si_code, strsignal(info->si_signo),
			// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
			info->si_addr, info->si_lower, info->si_upper,
			strsicode(info->si_signo, info->si_code),
			mctx.gregs[REG_RIP], mctx.gregs[REG_RSP], mctx.gregs[REG_RBP],
			mctx.gregs[REG_RSI], mctx.gregs[REG_RDI], mctx.gregs[REG_EFL],
			mctx.gregs[REG_CSGSFS], mctx.gregs[REG_ERR], mctx.gregs[REG_CR2],
			mctx.gregs[REG_RAX], mctx.gregs[REG_RBX], mctx.gregs[REG_RCX],
			mctx.gregs[REG_RDX], mctx.gregs[REG_R8], mctx.gregs[REG_R9],
			mctx.gregs[REG_R10], mctx.gregs[REG_R11], mctx.gregs[REG_R12],
			mctx.gregs[REG_R13], mctx.gregs[REG_R14], mctx.gregs[REG_R15]
		);
	}

	static struct sigaction trapSignal
	{
		(void (*)(int))trapHandler,
		{},
		SA_SIGINFO | SA_NODEFER,
		nullptr
	};
#endif

	bool handleVersionOrHelp()
	{
		constParsedArg_t version{findArg(parsedArgs, "--version"_sv, nullptr)};
		constParsedArg_t versionShort{findArg(parsedArgs, "-v"_sv, nullptr)};
		constParsedArg_t help{findArg(parsedArgs, "--help"_sv, nullptr)};
		constParsedArg_t helpShort{findArg(parsedArgs, "-h"_sv, nullptr)};

		if (help || helpShort)
			puts(crunchpp::help.data());
		else if (version || versionShort)
			testPrintf("crunch++ %s (%s %s %s-%s)\n", crunchpp::version.data(), crunchpp::compiler.data(),
				crunchpp::compilerVersion.data(), crunchpp::system.data(), crunchpp::arch.data());
		else
			return false;
		return true;
	}

	int32_t main(const int32_t argc, const char *const *const argv) try
	{
#if _WINDOWS
		_set_invalid_parameter_handler(invalidHandler);
		_CrtSetReportMode(_CRT_ASSERT, 0);
		_CrtSetReportMode(_CRT_ERROR, 0);
#endif
		registerArgs(args.data());
		parsedArgs = parseArguments(argc, argv);
		if (!parsedArgs.empty() && handleVersionOrHelp())
			return 0;
		else if (parsedArgs.empty() || !getTests())
		{
			testPrintf("Fatal error: There are no tests to run given on the command line!\n");
			return 2;
		}
		workingDir.reset(getcwd(nullptr, 0));
#ifndef _WIN32
		isTTY = isatty(STDOUT_FILENO);
		sigaction(SIGABRT, &trapSignal, nullptr);
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
	catch (const std::out_of_range &error)
	{
		testPrintf("Error during string splicing operations: %s\n", error.what());
		return 2;
	}
} // namespace crunch

int main(int argc, char **argv) { return crunch::main(argc, argv); }
