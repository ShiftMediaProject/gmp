/* mpf_pow_ui -- Compute b^e.

Copyright 1998, 1999, 2001, 2012, 2015 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

void
mpf_pow_ui (mpf_ptr r, mpf_srcptr b, unsigned long int e)
{
  mpf_t t;
  int cnt;

  if (e <= 1)
    {
      if (e == 0)
	mpf_set_ui (r, 1);
      else
	mpf_set (r, b);
      return;
    }

  count_leading_zeros (cnt, (mp_limb_t) e);
  cnt = GMP_LIMB_BITS - 1 - cnt;

  /* Use a temp for all intermediate result.  We might want to add an exponent
     derived # of bits here too, e.g.  ((cnt >> GMP_LIMB_BITS/2) != 0).  */
  mpf_init2 (t, mpf_get_prec (r) + GMP_LIMB_BITS);

  mpf_set (t, b);		/* consume most significant bit */
  while (--cnt > 0)
    {
      mpf_mul (t, t, t);
      if ((e >> cnt) & 1)
	mpf_mul (t, t, b);
    }

  if (e & 1)
    {
      mpf_mul (t, t, t);
      mpf_mul (r, t, b);
    }
  else
    {
      mpf_mul (r, t, t);
    }

  mpf_clear (t);
}
