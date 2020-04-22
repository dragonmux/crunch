// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef ARGS_PARSER_H
#define ARGS_PARSER_H

#include <stdint.h>
#include <memory>
#include <string>
#include <vector>
#include "crunch++.h"

struct arg_t final
{
	const std::string value;
	const uint32_t numMinParams;
	const uint32_t numMaxParams;
	const uint8_t flags;

	CRUNCH_VIS bool matches(const char *const str) const noexcept;
};

struct parsedArg_t final
{
	//using strPtr_t = std::unique_ptr<const char []>;

	std::string value;
	uint32_t paramsFound;
	std::vector<std::string> params;
	uint8_t flags;
	size_t minLength;

	CRUNCH_VIS parsedArg_t();
	parsedArg_t(parsedArg_t &&arg) : parsedArg_t{} { swap(arg); }
	void operator =(parsedArg_t &&arg) noexcept { swap(arg); }

	bool matches(const char *const str, const size_t length) const noexcept;
	CRUNCH_VIS bool matches(const char *const str) const noexcept;
	CRUNCH_VIS void swap(parsedArg_t &arg) noexcept;

	parsedArg_t(const parsedArg_t &) = delete;
	parsedArg_t &operator =(const parsedArg_t &) = delete;
};

using constParsedArg_t = const parsedArg_t *;
using parsedArgs_t = std::vector<parsedArg_t>;
using parsedRefArgs_t = std::vector<const parsedArg_t *>;

#define ARG_REPEATABLE	1
#define ARG_INCOMPLETE	2

CRUNCHpp_API void registerArgs(const arg_t *allowedArgs) noexcept;
CRUNCHpp_API parsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv) noexcept;
CRUNCHpp_API const parsedArg_t *findArg(const parsedArgs_t &args, const char *const value,
	const parsedArg_t *defaultVal);
CRUNCHpp_API const arg_t *findArgInArgs(const char *const value);
inline const arg_t *findArgInArgs(const std::unique_ptr<const char []> &value) { return findArgInArgs(value.get()); }
inline const arg_t *findArgInArgs(const std::string &value) { return findArgInArgs(value.data()); }

CRUNCH_VIS bool checkAlreadyFound(const parsedArgs_t &parsedArgs, const parsedArg_t &toCheck) noexcept;
CRUNCH_VIS uint32_t checkParams(const uint32_t argc, const char *const *const argv,
	const uint32_t argPos, const arg_t &argument, const arg_t *const args) noexcept;

#endif /* ARGS_PARSER_H */
