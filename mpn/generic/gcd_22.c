/* mpn_gcd_22 -- double limb greatest common divisor.

Copyright 1994, 1996, 2000, 2001, 2009, 2012, 2019 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#include "gmp-impl.h"
#include "longlong.h"

#if GMP_NAIL_BITS > 0
#error Nails not supported.
#endif

mp_double_limb_t
mpn_gcd_22 (mp_limb_t u1, mp_limb_t u0, mp_limb_t v1, mp_limb_t v0)
{
  mp_double_limb_t g;
  ASSERT (u0 & 1);
  ASSERT (v0 & 1);

  /* Check for u0 != v0 needed to ensure that argument to
   * count_trailing_zeros is non-zero. */
  while (u1 != v1 && u0 != v0)
    {
      unsigned long int r;
      if (u1 > v1)
	{
	  sub_ddmmss (u1, u0, u1, u0, v1, v0);
	  count_trailing_zeros (r, u0);
	  u0 = ((u1 << (GMP_NUMB_BITS - r)) & GMP_NUMB_MASK) | (u0 >> r);
	  u1 >>= r;
	}
      else  /* u1 < v1.  */
	{
	  sub_ddmmss (v1, v0, v1, v0, u1, u0);
	  count_trailing_zeros (r, v0);
	  v0 = ((v1 << (GMP_NUMB_BITS - r)) & GMP_NUMB_MASK) | (v0 >> r);
	  v1 >>= r;
	}
    }

  /* If U == V == GCD, done.  Otherwise, compute GCD (V, |U - V|).  */
  if (u1 == v1 && u0 == v0)
    {
      g.d0 = u0; g.d1 = u1;
      return g;
    }
  else {
    mp_limb_t t[2];
    t[0] = u0; t[1] = u1;

    v0 = (u0 == v0) ? ((u1 > v1) ? u1-v1 : v1-u1) : ((u0 > v0) ? u0-v0 : v0-u0);
    g.d0 = mpn_gcd_1 (t, 1 + (u1 != 0), v0);
    g.d1 = 0;
    return g;
  }
}
