// SPDX-License-Identifier: LGPL-3.0-or-later
#include <crunch++.h>
#ifndef _WINDOWS
#include <unistd.h>
#else
#include <io.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include "logger.hxx"
#include "core.hxx"

#ifndef _WINDOWS
#define DEV_NULL "/dev/null"
#else
#define DEV_NULL "NUL"
#define STDOUT_FILENO fileno(stdout)
#define O_CLOEXEC O_BINARY
#endif

class loggerTests final : public testsuite
{
private:
	int nullFD = -1;
	int stdoutFD = -1;
	testLog ourLogger;

	void testColumns()
	{
		logger = &ourLogger;
		assertEqual(dup2(nullFD, STDOUT_FILENO), STDOUT_FILENO);
		assertEqual(getColumns(), 72);
		assertEqual(dup2(stdoutFD, STDOUT_FILENO), STDOUT_FILENO);
		logger = nullptr;
	}

	void testSuccess()
	{
		//logger = &pipeLogger;
		isTTY = false;
		logResult(RESULT_SUCCESS, "");
		--passes;
		isTTY = true;
		logResult(RESULT_SUCCESS, "");
		--passes;
		isTTY = isatty(STDOUT_FILENO);
		//logger = nullptr;
	}

	void testFailure()
	{
		//logger = &pipeLogger;
		isTTY = false;
		logResult(RESULT_FAILURE, "");
		--failures;
		isTTY = true;
		logResult(RESULT_FAILURE, "");
		--failures;
		isTTY = isatty(STDOUT_FILENO);
		//logger = nullptr;
	}

	void testSkip()
	{
		//logger = &pipeLogger;
		isTTY = false;
		logResult(RESULT_SKIP, "");
		--passes;
		isTTY = true;
		logResult(RESULT_SKIP, "");
		--passes;
		isTTY = isatty(STDOUT_FILENO);
		//logger = nullptr;
	}

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
	loggerTests() noexcept : nullFD{open(DEV_NULL, O_RDONLY | O_CLOEXEC)}, stdoutFD{dup(STDOUT_FILENO)},
		ourLogger{nullptr, stdoutFD != -1 ? fdopen(stdoutFD, "w") : nullptr, nullptr, 0} { }

	~loggerTests() noexcept
	{
		close(nullFD);
		close(stdoutFD);
		if (ourLogger.stdout_)
			fclose(ourLogger.stdout_);
	}

	void registerTests() final
	{
#ifdef _WINDOWS
		skip("This suite does not work on windows");
#endif
		if (nullFD == -1 || stdoutFD == -1)
			skip("Unable to open null device for tests");
		CXX_TEST(testColumns)
		CXX_TEST(testSuccess)
		CXX_TEST(testFailure)
		CXX_TEST(testSkip)
		CXX_TEST(testAbort)
	}
};

CRUNCHpp_TEST void registerCXXTests();
void registerCXXTests()
{
	registerTestClasses<loggerTests>();
}
