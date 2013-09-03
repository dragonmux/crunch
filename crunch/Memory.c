/*
 * This file is part of crunch
 * Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
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

#include "Memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define die(...) \
{ \
	printf(__VA_ARGS__); \
	exit(1); \
}

void *testMalloc(size_t size)
{
	void *ret = malloc(size);
	if (ret == NULL)
		die("**** crunch Fatal ****\nCould not allocate enough memory!\n**** crunch Fatal ****");
	memset(ret, 0, size);
	return ret;
}

void *testRealloc(void *ptr, size_t size)
{
	void *ret = realloc(ptr, size);
	if (ret == NULL)
		die("**** crunch Fatal ****\nCould not reallocate memory - out of memory!\n**** crunch Fatal ****");
	return ret;
}
