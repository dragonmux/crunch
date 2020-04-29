// SPDX-License-Identifier: LGPL-3.0-or-later
#include <cstdlib>
#include <cstring>
#include <new>
#include "crunch++.h"
#include "argsParser.hxx"
#include "stringFuncs.hxx"
#include "logger.hxx"

namespace crunch
{
	static const arg_t *args = nullptr;

	bool arg_t::matches(const internal::stringView &str) const noexcept
	{
		if (flags & ARG_INCOMPLETE)
			return !str.compare(0, value.length(), value);
		return value == str;
	}

	bool parsedArg_t::matches(const internal::stringView &str) const noexcept
	{
		if (flags & ARG_INCOMPLETE)
			return str.length() >= minLength &&
				!value.compare(0, str.length(), str);
		return value == str;
	}

	void parsedArg_t::swap(parsedArg_t &arg) noexcept
	{
		value.swap(arg.value);
		std::swap(paramsFound, arg.paramsFound);
		params.swap(arg.params);
		std::swap(flags, arg.flags);
		std::swap(minLength, arg.minLength);
	}

	void registerArgs(const arg_t *allowedArgs) noexcept
		{ args = allowedArgs; }

	bool checkAlreadyFound(const parsedArgs_t &parsedArgs, const parsedArg_t &toCheck) noexcept
	{
		for (const auto &entry : parsedArgs)
		{
			if (entry.value == toCheck.value)
				return true;
		}
		return false;
	}

	uint32_t checkParams(const uint32_t argc, const char *const *const argv, const uint32_t argPos,
		const arg_t &argument, const arg_t *const args_) noexcept
	{
		uint32_t n = 0;
		const auto min{argument.numMinParams};
		const auto max{argument.numMaxParams};
		bool eoa = false;
		for (uint32_t i = argPos; i < argc && n < max && !eoa; ++i)
		{
			const arg_t *currArg = args_;
			const internal::stringView arg{argv[i], std::strlen(argv[i])};
			while (!currArg->value.empty() && !eoa)
			{
				if (currArg->matches(arg))
					eoa = true;
				++currArg;
			}
			if (eoa || ++n == max)
				break;
		}
		if (n < min)
			return UINT32_MAX;
		return n;
	}

	parsedArgs_t parseArguments(const uint32_t argc, const char *const *const argv) noexcept try
	{
		if (argc < 1 || (argc >> 31U) == 1 || !argv || !args)
			return {};

		parsedArgs_t result{};
		result.reserve(argc);
		for (uint32_t i = 1; i < argc; ++i)
		{
			bool found{};
			bool skip{};
			const auto *argument{args};
			parsedArg_t entry{};
			const internal::stringView arg{argv[i], std::strlen(argv[i])};
			while (!argument->value.empty())
			{
				found = argument->matches(arg);
				if (found)
				{
					entry.value = arg;
					entry.minLength = argument->value.length();
					if (!(argument->flags & ARG_REPEATABLE) && checkAlreadyFound(result, entry))
					{
						printf("Duplicate argument found: %s\n", entry.value.data());
						skip = true;
						break;
					}
					entry.paramsFound = checkParams(argc, argv, i + 1, *argument, args);
					if (entry.paramsFound == UINT32_MAX)
					{
						printf("Not enough parameters given for argument %s\n", arg.data());
						return {};
					}
					// Only allocate for the params if there are any found, otherwise the list dwell as empty.
					if (entry.paramsFound)
					{
						entry.params.reserve(entry.paramsFound);
						for (uint32_t j = 0; j < entry.paramsFound; ++j)
							entry.params.emplace_back(argv[i + j + 1]);
					}
					entry.params.shrink_to_fit();
					i += entry.paramsFound;
					entry.flags = argument->flags;
					break;
				}
				else if ((argument->flags & ARG_INCOMPLETE) &&
					!argument->value.compare(0, arg.length(), arg))
				{
					printf("Badly formatted argument (%s)\n", argv[i]);
					return {};
				}
				++argument;
			}
			if (skip)
				continue;
			else if (!found)
			{
				entry.value = arg;
				entry.paramsFound = 0;
				entry.flags = argument->flags;
				entry.minLength = 0;
			}
			result.emplace_back(std::move(entry));
		}
		/* Shrink as appropriate */
		result.shrink_to_fit();
		return result;
	}
	catch (std::bad_alloc &)
		{ return {}; }

	const parsedArg_t *findArg(const parsedArgs_t &args_, const internal::stringView &value,
		const parsedArg_t *defaultValue)
	{
		if (args_.empty() || value.empty())
			return defaultValue;
		for (const auto &arg : args_)
		{
			if (arg.matches(value))
				return &arg;
		}
		return defaultValue;
	}

	const arg_t *findArgInArgs(const internal::stringView &value)
	{
		const arg_t *curr = args;
		while (!curr->value.empty())
		{
			if (curr->matches(value))
				return curr;
			++curr;
		}
		return nullptr;
	}
} // namespace crunch
