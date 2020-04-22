// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef __CORE_H__
#define __CORE_H__

#include "threading/threadShim.h"
#include <stdint.h>
#include <stdlib.h>
#include "crunch.h"

CRUNCH_API uint32_t passes, failures;

enum
{
	THREAD_SUCCESS = 0,
	THREAD_ERROR = 1,
	THREAD_ABORT = 2
};

#endif /* __CORE_H__ */
