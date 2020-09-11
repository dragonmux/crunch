// SPDX-License-Identifier: LGPL-3.0-or-later
#include <crunch++.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <substrate/fixed_vector>
#ifndef _WINDOWS
#include <substrate/pty>
#else
#include <io.h>
#endif
#include <substrate/pipe>
#include <fcntl.h>
#include <logger.hxx>
#include <core.hxx>

using crunch::literals::operator ""_sv;
using crunch::logger;
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

#ifndef _WINDOWS
using substrate::pty_t;

constexpr static auto defaultTTY{"/dev/ptmx"_sv};
constexpr static auto devNull{"/dev/null"_sv};
#else
constexpr static auto devNull{"NUL"_sv};
#define STDOUT_FILENO fileno(stdout)
#define O_CLOEXEC O_BINARY
#endif
using substrate::fd_t;
using substrate::pipe_t;
using substrate::readPipe_t;
using substrate::fixedVector_t;

std::chrono::microseconds operator ""_us(unsigned long long us) noexcept
	{ return std::chrono::microseconds{us}; }

constexpr static auto plainSuccess{" [  OK  ]\n"_sv};
constexpr static auto plainFailure{" [ FAIL ]\n"_sv};
constexpr static auto plainSkip{" [ SKIP ]\n"_sv};
constexpr static auto colourSuccess{
	NORMAL CURS_UP "\x1B[72G" BRACKET "[" SUCCESS "  OK  " BRACKET "]" NORMAL "\r\n"_sv
};
constexpr static auto colourFailure{
	NORMAL " \x1B[72G" BRACKET "[" FAILURE " FAIL " BRACKET "]" NORMAL "\r\n"_sv
};
constexpr static auto colourSkip{
	NORMAL " \x1B[72G" BRACKET "[" WARNING " SKIP " BRACKET "]" NORMAL "\r\n"_sv
};

class loggerTests final : public testsuite
{
private:
#ifndef _WINDOWS
	pty_t pty{};
#endif
	pipe_t pipe{};
	int32_t stdoutFD{-1};
	int32_t stderrFD{-1};
	crunch::testLog ourLogger;

	using stringView = crunch::internal::stringView;
	using cleanupFn_t = void (*)();

	void swapToPipe()
	{
		assertTrue(pipe.valid());
		assertEqual(dup2(pipe.writeFD(), STDERR_FILENO), STDERR_FILENO);
		assertEqual(dup2(pipe.writeFD(), STDOUT_FILENO), STDOUT_FILENO);
		isTTY = false;
	}

#ifndef _WINDOWS
	void swapToPTY()
	{
		assertTrue(pty.valid());
		assertEqual(dup2(pty.pts(), STDERR_FILENO), STDERR_FILENO);
		assertEqual(dup2(pty.pts(), STDOUT_FILENO), STDOUT_FILENO);
		isTTY = true;
	}
#endif

	void restoreStdio()
	{
		assertEqual(dup2(stdoutFD, STDOUT_FILENO), STDOUT_FILENO);
		assertEqual(dup2(stderrFD, STDERR_FILENO), STDERR_FILENO);
		isTTY = isatty(STDOUT_FILENO);
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

	void testLogResult(const resultType type, const cleanupFn_t cleanupFn,
		const stringView &plainExpected, const stringView &colourExpected)
	{
		swapToPipe();
		logResult(type, "");
		cleanupFn();
		restoreStdio();
		assertPipeRead(pipe.readFD(), plainExpected);

#ifndef _WINDOWS
		swapToPTY();
		logResult(type, "");
		cleanupFn();
		restoreStdio();
		assertConsoleRead(pty.ptmx(), colourExpected);
#endif
	}

	void testColumns()
	{
#ifndef _WINDOWS
		swapToPTY();
		assertEqual(getColumns(), 72);
		restoreStdio();
#endif
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
		isTTY = isatty(STDOUT_FILENO);
		fail("logResult() failed to throw exception");
	}

	void testAbort()
	{
		//logger = &pipeLogger;
		isTTY = false;
		tryLogAbort();
		isTTY = true;
		tryLogAbort();
		isTTY = isatty(STDOUT_FILENO);
		//logger = nullptr;
	}

public:
	loggerTests() noexcept : stdoutFD{dup(STDOUT_FILENO)}, stderrFD{dup(STDERR_FILENO)},
		ourLogger{nullptr, stdoutFD != -1 ? fdopen(stdoutFD, "w") : nullptr, nullptr, 0} { }
	loggerTests(const loggerTests &) = delete;
	loggerTests(loggerTests &&) = delete;
	loggerTests &operator =(const loggerTests &) = delete;
	loggerTests &operator =(loggerTests &&) = delete;

	~loggerTests() noexcept final
	{
		close(stdoutFD);
		close(stderrFD);
		if (ourLogger.stdout_)
			fclose(ourLogger.stdout_); // NOLINT(cppcoreguidelines-owning-memory)
	}

	void registerTests() final
	{
		if (stdoutFD == -1 || stderrFD == -1)
			skip("Unable to dup() stdio file descriptors for tests");
		CRUNCHpp_TEST(testColumns)
		CRUNCHpp_TEST(testSuccess)
		CRUNCHpp_TEST(testFailure)
		CRUNCHpp_TEST(testSkip)
#ifndef _WINDOWS
		CRUNCHpp_TEST(testAbort)
#endif
	}
};

CRUNCHpp_TESTS(loggerTests)
