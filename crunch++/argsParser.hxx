// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef ARGS_PARSER__HXX
#define ARGS_PARSER__HXX

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <cstring>
#include "crunch++.h"

namespace crunch
{
	struct arg_t final
	{
		internal::stringView value{};
		const uint32_t numMinParams{};
		const uint32_t numMaxParams{};
		const uint8_t flags{};

		// clang 5 has a bad time with this if we don't define it this way.
		constexpr arg_t() noexcept { } // NOLINT(modernize-use-equals-default)
		constexpr arg_t(const internal::stringView &value_, const uint32_t minParams, const uint32_t maxParams,
			const uint8_t flags_) : value{value_}, numMinParams{minParams}, numMaxParams{maxParams}, flags{flags_} { }
		~arg_t() noexcept = default;
		CRUNCH_VIS bool matches(const internal::stringView &str) const noexcept;
	};

	struct parsedArg_t final
	{
		//using strPtr_t = std::unique_ptr<const char []>;

		internal::stringView value{};
		uint32_t paramsFound{0};
		std::vector<std::string> params{};
		uint8_t flags{0};
		size_t minLength{0};

		// clang 5 has a bad time with this if we don't define it this way.
		parsedArg_t() noexcept { } // NOLINT(modernize-use-equals-default)
		parsedArg_t(parsedArg_t &&arg) noexcept : parsedArg_t{} { swap(arg); }
		~parsedArg_t() noexcept = default;
		void operator =(parsedArg_t &&arg) noexcept { swap(arg); }

		CRUNCH_VIS bool matches(const internal::stringView &str) const noexcept;
		CRUNCH_VIS void swap(parsedArg_t &arg) noexcept;

		parsedArg_t(const parsedArg_t &) = delete;
		parsedArg_t &operator =(const parsedArg_t &) = delete;
	};

	using constParsedArg_t = const parsedArg_t *;
	using parsedArgs_t = std::vector<parsedArg_t>;
	using parsedRefArgs_t = std::vector<const parsedArg_t *>;

	constexpr uint8_t ARG_REPEATABLE = 1U;
	constexpr uint8_t ARG_INCOMPLETE = 2U;

	CRUNCHpp_API void registerArgs(const arg_t *allowedArgs) noexcept;
	CRUNCHpp_API parsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv) noexcept;
	CRUNCHpp_API const parsedArg_t *findArg(const parsedArgs_t &args_, const internal::stringView &value,
		const parsedArg_t *defaultValue);
	CRUNCHpp_API const arg_t *findArgInArgs(const internal::stringView &value);

	CRUNCHpp_API bool checkAlreadyFound(const parsedArgs_t &parsedArgs, const parsedArg_t &toCheck) noexcept;
	CRUNCHpp_API uint32_t checkParams(const uint32_t argc, const char *const *const argv,
		const uint32_t argPos, const arg_t &argument, const arg_t *const args) noexcept;
} // namespace crunch

#endif /*ARGS_PARSER__HXX*/
