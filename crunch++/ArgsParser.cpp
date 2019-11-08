/*
 * This file is part of crunch
 * Copyright © 2013-2017 Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * crunch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * crunch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "crunch++.h"
#include "ArgsParser.h"
#include "StringFuncs.h"
#include "Logger.h"
#include <stdlib.h>
#include <string.h>
#include <new>

const arg_t *args = nullptr;

bool arg_t::matches(const char *const str) const noexcept
{
	if (flags & ARG_INCOMPLETE)
		return !strncmp(value.data(), str, value.length());
	return !strcmp(value.data(), str);
}

parsedArg_t::parsedArg_t() : value{}, paramsFound{0}, params{}, flags{0}, minLength{0} { }

bool parsedArg_t::matches(const char *const str) const noexcept
	{ return matches(str, strlen(str)); }

bool parsedArg_t::matches(const char *const str, const size_t length) const noexcept
{
	if (flags & ARG_INCOMPLETE)
		return length >= minLength &&
			!strncmp(value.data(), str, length);
	return !strcmp(value.data(), str);
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
	for (auto &entry : parsedArgs)
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
	const uint32_t min = argument.numMinParams, max = argument.numMaxParams;
	bool eoa = false;
	for (uint32_t i = argPos; i < argc && n < max && !eoa; ++i)
	{
		const arg_t *currArg = args_;
		while (!currArg->value.empty() && !eoa)
		{
			if (currArg->matches(argv[i]))
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
	if (argc < 1 || (argc >> 31) == 1 || !argv || !args)
		return {};

	parsedArgs_t result{};
	result.reserve(argc);
	for (uint32_t i = 1; i < argc; ++i)
	{
		bool found = false, skip = false;
		auto argument = args;
		parsedArg_t entry;
		while (!argument->value.empty())
		{
			found = argument->matches(argv[i]);
			if (found)
			{
				entry.value = argv[i];
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
					printf("Not enough parameters given for argument %s\n", argv[i]);
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
				strncmp(argument->value.data(), argv[i], argument->value.length()) == 0)
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
			entry.value = argv[i];
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

const parsedArg_t *findArg(const parsedArgs_t &args_, const char *const value, const parsedArg_t *defaultValue)
{
	if (args_.empty() || !value)
		return defaultValue;
	for (const auto &arg : args_)
	{
		if (arg.matches(value))
			return &arg;
	}
	return defaultValue;
}

const arg_t *findArgInArgs(const char *const value)
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
