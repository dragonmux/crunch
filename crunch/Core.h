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

#ifndef __CORE_H__
#define __CORE_H__

#ifdef _MSC_VER
#define NORETURN __declspec(noreturn)
#else
#define NORETURN __attribute((noreturn))
#endif

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include "crunch.h"

extern void libDebugExit(int num) NORETURN;

extern uint32_t passes, failures;

#define pthreadExit(val) \
	pthread_exit((void *)val); \
	exit(*val)

typedef struct _unitTest
{
	pthread_t *testThread;
	test *theTest;
} unitTest;

#endif /* __CORE_H__ */
