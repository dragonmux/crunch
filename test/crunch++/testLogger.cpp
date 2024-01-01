// SPDX-License-Identifier: LGPL-3.0-or-later
#include <crunch++.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <substrate/fixed_vector>
#include <substrate/fd>
#include <substrate/pty>
#include <substrate/pipe>
#include <fcntl.h>
#include <logger.hxx>
#include <core.hxx>

using crunch::literals::operator ""_sv;
using crunch::getColumns;
using crunch::isTTY;
using crunch::logResult;
using crunch::resultType;
using crunch::RESULT_SUCCESS;
using crunch::RESULT_FAILURE;
using crunch::RESULT_SKIP;
using crunch::RESULT_ABORT;
using crunch::passes;
using crunch::failures;

using substrate::fd_t;
using substrate::pipe_t;
using substrate::readPipe_t;
using substrate::pty_t;
using substrate::fixedVector_t;

#ifndef _WIN32
constexpr static auto defaultTTY{"/dev/ptmx"_sv};
constexpr static auto devNull{"/dev/null"_sv};
#elif !defined(__MINGW32__) || !defined(__MINGW64__)
#define STDOUT_FILENO fileno(stdout)
#define STDERR_FILENO fileno(stderr)
#define O_CLOEXEC O_BINARY
#endif

std::chrono::microseconds operator ""_us(unsigned long long us) noexcept
	{ return std::chrono::microseconds{us}; }

constexpr static auto plainSuccess{" [  OK  ]\n"_sv};
constexpr static auto plainFailure{" [ FAIL ]\n"_sv};
constexpr static auto plainSkip{" [ SKIP ]\n"_sv};
constexpr static auto plainAborted{"[ **** ABORTED **** ]\n"_sv};
#ifndef _WIN32
constexpr static auto colourSuccess{
	NORMAL CURS_UP "\x1B[72G" BRACKET "[" SUCCESS "  OK  " BRACKET "]" NORMAL "\r\n"_sv
};
constexpr static auto colourFailure{
	NORMAL " \x1B[72G" BRACKET "[" FAILURE " FAIL " BRACKET "]" NORMAL "\r\n"_sv
};
constexpr static auto colourSkip{
	NORMAL " \x1B[72G" BRACKET "[" WARNING " SKIP " BRACKET "]" NORMAL "\r\n"_sv
};
constexpr static auto colourAborted{
	NORMAL "\r\n" BRACKET "[" FAILURE " **** ABORTED **** " BRACKET "]" NORMAL "\r\n"_sv
};
#else
constexpr static auto colourSuccess{"[  OK  ]\n"_sv};
constexpr static auto colourFailure{"[ FAIL ]\n"_sv};
constexpr static auto colourSkip{"[ SKIP ]\n"_sv};
constexpr static auto colourAborted{"[ **** ABORTED **** ]\n"_sv};
#endif

class loggerTests final : public testsuite
{
private:
	pty_t pty{};
	pipe_t pipe{};
	int32_t stdoutFileno{STDOUT_FILENO};
	int32_t stderrFileno{STDERR_FILENO};
	fd_t stdoutFD{dup(stdoutFileno)};
	fd_t stderrFD{dup(stderrFileno)};

	using stringView = crunch::internal::stringView;
	using cleanupFn_t = void (*)();

#ifdef _WIN32
	static int32_t dup2(int32_t fd1, int32_t fd2) noexcept
	{
		const auto result{::dup2(fd1, fd2)};
		if (!result)
			return fd2;
		return result;
	}
#endif

	void swapToPipe()
	{
		assertTrue(pipe.valid());
		assertEqual(dup2(pipe.writeFD(), stderrFileno), stderrFileno);
		assertEqual(dup2(pipe.writeFD(), stdoutFileno), stdoutFileno);
		isTTY = false;
	}

	void swapToPTY()
	{
		assertTrue(pty.valid());
		assertEqual(dup2(pty.pts(), stderrFileno), stderrFileno);
		assertEqual(dup2(pty.pts(), stdoutFileno), stdoutFileno);
		isTTY = true;
	}

	void restoreStdio()
	{
		assertEqual(dup2(stdoutFD, stdoutFileno), stdoutFileno);
		assertEqual(dup2(stderrFD, stderrFileno), stderrFileno);
		isTTY = isatty(stdoutFileno);
	}

	void assertPipeRead(const readPipe_t &fd, const stringView &expected)
	{
		fixedVector_t<char> result{expected.length()};
		assertTrue(result.valid());
		assertTrue(fd.read(result.data(), result.size()));
		assertEqual(result.data(), expected.data(), expected.length());
	}

	void assertConsoleRead(const fd_t &fd, const stringView &expected)
	{
		fixedVector_t<char> result{expected.length()};
		assertTrue(result.valid());
		std::this_thread::sleep_for(10_us);
		assertTrue(fd.read(result.data(), result.size()));
		assertEqual(result.data(), expected.data(), expected.length());
	}

#ifdef _WIN32
	WORD colourFor(const stringView &expected) noexcept
	{
		if (expected == colourSuccess)
			return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		else if (expected == colourFailure)
			return FOREGROUND_RED | FOREGROUND_INTENSITY;
		else if (expected == colourSkip)
			return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		else
			return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	}

	void assertConsoleColouring(const HANDLE console, const WORD colour)
	{
		WORD attribute{};
		DWORD attrCount{};
		CONSOLE_SCREEN_BUFFER_INFO cursor{};

		auto win32APICall = [this](const bool result)
		{
			if (!result)
			{
				const auto error{GetLastError()};
				printf("ReadConsoleOutputAttribute() failed with result %08lx (%ld)\n", error, error);
			}
			assertTrue(result);
		};

		assertNotEqual(console, INVALID_HANDLE_VALUE);
		win32APICall(GetConsoleScreenBufferInfo(console, &cursor));
		assertEqual(cursor.dwCursorPosition.X, 0);
		assertNotEqual(cursor.dwCursorPosition.Y, 0);

		cursor.dwCursorPosition.X = getColumns();
		--cursor.dwCursorPosition.Y;
		win32APICall(ReadConsoleOutputAttribute(console, &attribute, 1, cursor.dwCursorPosition, &attrCount));
		assertEqual(uint32_t{attrCount}, 1U);
		assertEqual(attribute, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

		++cursor.dwCursorPosition.X;
	}
#endif

	void testLogResult(const resultType type, const cleanupFn_t cleanupFn,
		const stringView &plainExpected, const stringView &colourExpected)
	{
		swapToPipe();
		logResult(type, "");
		cleanupFn();
		restoreStdio();
		assertPipeRead(pipe.readFD(), plainExpected);

		swapToPTY();
		logResult(type, "");
		cleanupFn();
		restoreStdio();
		assertConsoleRead(pty.ptmx(), colourExpected);
	}

	void testColumns()
	{
		swapToPTY();
		assertEqual(getColumns(), 72);
		restoreStdio();
	}

	void testSuccess() { testLogResult(RESULT_SUCCESS, []() { --passes; }, plainSuccess, colourSuccess); }
	void testFailure() { testLogResult(RESULT_FAILURE, []() { --failures; }, plainFailure, colourFailure); }
	void testSkip() { testLogResult(RESULT_SKIP, []() { --passes; }, plainSkip, colourSkip); }

	void tryLogAbort()
	{
		try
			{ logResult(RESULT_ABORT, ""); }
		catch (threadExit_t &)
			{ return; }
		restoreStdio();
		fail("logResult() failed to throw exception");
	}

	void testAbort()
	{
		swapToPipe();
		tryLogAbort();
		restoreStdio();
		assertPipeRead(pipe.readFD(), plainAborted);

		swapToPTY();
		tryLogAbort();
		restoreStdio();
		assertConsoleRead(pty.ptmx(), colourAborted);
	}

public:
	CRUNCHpp_MAYBE_NOEXCEPT(loggerTests()) = default;
	loggerTests(const loggerTests &) = delete;
	loggerTests(loggerTests &&) = delete;
	loggerTests &operator =(const loggerTests &) = delete;
	loggerTests &operator =(loggerTests &&) = delete;
	~loggerTests() noexcept final = default;

	void registerTests() final
	{
		if (!stdoutFD.valid() || !stderrFD.valid())
			skip("Unable to dup() stdio file descriptors for tests");
		CRUNCHpp_TEST(testColumns)
		CRUNCHpp_TEST(testSuccess)
		CRUNCHpp_TEST(testFailure)
		CRUNCHpp_TEST(testSkip)
		CRUNCHpp_TEST(testAbort)
	}
};

CRUNCHpp_TESTS(loggerTests)
