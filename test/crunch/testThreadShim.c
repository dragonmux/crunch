// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef _WIN32
#include <errno.h>
#endif
#include <threading/threadShim.h>
#include <crunch.h>

#ifndef _WIN32
void testErrorMapping()
{
	for (int i = 0; i < 255; ++i)
	{
		const int result = thrd_err_map(i);
		if (i == 0)
			assertIntEqual(result, thrd_success);
		else if (i == ENOMEM)
			assertIntEqual(result, thrd_nomem);
		else if (i == ETIMEDOUT)
			assertIntEqual(result, thrd_timedout);
		else if (i == EBUSY)
			assertIntEqual(result, thrd_busy);
		else
			assertIntEqual(result, thrd_error);
	}
}
#else
void testErrorMapping()
{
	SetLastError(ERROR_OUTOFMEMORY);
	assertIntEqual(thrd_get_error(), thrd_nomem);
	SetLastError(WAIT_TIMEOUT);
	assertIntEqual(thrd_get_error(), thrd_timedout);
	SetLastError(ERROR_BUSY);
	assertIntEqual(thrd_get_error(), thrd_busy);
	SetLastError(ERROR_INVALID_HANDLE);
	assertIntEqual(thrd_get_error(), thrd_error);
	SetLastError(ERROR_SUCCESS);
	assertIntEqual(thrd_get_error(), thrd_success);
}
#endif

BEGIN_REGISTER_TESTS()
	TEST(testErrorMapping)
END_REGISTER_TESTS()
