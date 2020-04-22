// SPDX-License-Identifier: LGPL-3.0-or-later
#include <errno.h>

#include <threading/threadShim.h>
#include <crunch.h>

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

BEGIN_REGISTER_TESTS()
	TEST(testErrorMapping)
END_REGISTER_TESTS()
