// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstring>
#include <cinttypes>
#include "crunch++.h"
#include "core.hxx"
#include "logger.hxx"
#include "stringFuncs.hxx"

constexpr static double doubleDelta = 0.0000001;

namespace crunch
{
	uint32_t passes = 0, failures = 0;

	template<typename T>
	void assertionFailure(const char *what, T result, T expected)
	{
		auto mesg = formatString("Assertion failure: %s", what);
		logResult(RESULT_FAILURE, mesg.get(), result, expected);
	}

	template<typename T>
	void assertionError(const char *params, T result, T expected)
	{
		auto what = formatString("expected %s, got %s", params, params);
		assertionFailure(what.get(), expected, result);
	}

	template<typename T>
	void assertionError(const char *params, T result, std::nullptr_t)
		{ return assertionError(params, result, T(nullptr)); }

	template<typename T>
	void assertionError(const char *params, T result)
	{
		auto what = formatString("did not expect %s", params);
		assertionFailure(what.get(), result, result);
	}
} // namespace crunch

using crunch::assertionFailure;
using crunch::assertionError;
using crunch::logResult;
using crunch::RESULT_FAILURE;
using crunch::RESULT_SKIP;

testsuite::testsuite() noexcept = default;
testsuite::~testsuite() noexcept = default;

void testsuite::fail(const char *const reason)
{
	logResult(RESULT_FAILURE, "Failure: %s", reason);
	throw threadExit_t{1};
}

void testsuite::skip(const char *const reason)
{
	logResult(RESULT_SKIP, "Skipping: %s", reason);
	throw threadExit_t{1};
}

void testsuite::assertTrue(const bool value)
{
	if (!value)
	{
		assertionError("%s", boolToString(value), "true");
		throw threadExit_t{1};
	}
}

void testsuite::assertFalse(const bool value)
{
	if (value)
	{
		assertionError("%s", boolToString(value), "false");
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const int8_t result, const int8_t expected)
{
	if (result != expected)
	{
		assertionError("%d", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const int8_t result, const int8_t expected)
{
	if (result == expected)
	{
		assertionError("%d", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const uint8_t result, const uint8_t expected)
{
	if (result != expected)
	{
		assertionError("%u", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const uint8_t result, const uint8_t expected)
{
	if (result == expected)
	{
		assertionError("%u", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const int16_t result, const int16_t expected)
{
	if (result != expected)
	{
		assertionError("%d", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const int16_t result, const int16_t expected)
{
	if (result == expected)
	{
		assertionError("%d", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const uint16_t result, const uint16_t expected)
{
	if (result != expected)
	{
		assertionError("%u", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const uint16_t result, const uint16_t expected)
{
	if (result == expected)
	{
		assertionError("%u", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const int32_t result, const int32_t expected)
{
	if (result != expected)
	{
		assertionError("%d", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const int32_t result, const int32_t expected)
{
	if (result == expected)
	{
		assertionError("%d", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const uint32_t result, const uint32_t expected)
{
	if (result != expected)
	{
		assertionError("%u", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const uint32_t result, const uint32_t expected)
{
	if (result == expected)
	{
		assertionError("%u", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const int64_t result, const int64_t expected)
{
	if (result != expected)
	{
		assertionError("%" PRId64, result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const int64_t result, const int64_t expected)
{
	if (result == expected)
	{
		assertionError("%" PRId64, result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const uint64_t result, const uint64_t expected)
{
	if (result != expected)
	{
		assertionError("%" PRIu64, result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const uint64_t result, const uint64_t expected)
{
	if (result == expected)
	{
		assertionError("%" PRIu64, result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(void *result, void *expected)
{
	if (result != expected)
	{
		assertionError("%p", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(void *result, void *expected)
{
	if (result == expected)
	{
		assertionError("%p", result);
		throw threadExit_t{1};
	}
}

inline bool delta(const double result, const double expected)
{
	return (result >= (expected - doubleDelta) && result <= (expected + doubleDelta));
}

void testsuite::assertEqual(const double result, const double expected)
{
	if (!delta(result, expected))
	{
		assertionError("%f", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const double result, const double expected)
{
	if (delta(result, expected))
	{
		assertionError("%f", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const char *const result, const char *const expected)
{
	if (std::strcmp(result, expected) != 0)
	{
		assertionError("%s", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const char *const result, const char *const expected)
{
	if (std::strcmp(result, expected) == 0)
	{
		assertionError("%s", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const stringView result, const stringView expected)
{
	if (result.length() != expected.length() || std::memcmp(result.data(), expected.data(), expected.length()) != 0)
	{
		assertionError("%s", result.data(), expected.data());
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const stringView result, const stringView expected)
{
	if (result.length() == expected.length() && std::memcmp(result.data(), expected.data(), expected.length()) == 0)
	{
		assertionError("%s", result.data());
		throw threadExit_t{1};
	}
}

void testsuite::assertEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (std::memcmp(result, expected, expectedLength) != 0)
	{
		assertionFailure("buffers %p and %p do not match", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotEqual(const void *result, const void *expected, const size_t expectedLength)
{
	if (std::memcmp(result, expected, expectedLength) == 0)
	{
		assertionFailure("buffers %p and %p match", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertNull(const void *const result)
{
	if (result != nullptr)
	{
		assertionError("%p", result, nullptr);
		throw threadExit_t{1};
	}
}

void testsuite::assertNotNull(const void *const result)
{
	if (result == nullptr)
	{
		assertionError("%p", result);
		throw threadExit_t{1};
	}
}

void testsuite::assertGreaterThan(const long result, const long expected)
{
	if (result <= expected)
	{
		assertionFailure("%ld was not greater than %ld", result, expected);
		throw threadExit_t{1};
	}
}

void testsuite::assertLessThan(const long result, const long expected)
{
	if (result >= expected)
	{
		assertionFailure("%ld was not less than %ld", result, expected);
		throw threadExit_t{1};
	}
}
