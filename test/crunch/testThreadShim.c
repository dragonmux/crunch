/*
 * This file is part of crunch
 * Copyright © 2020 Rachel Mant (dx-mon@users.sourceforge.net)
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
