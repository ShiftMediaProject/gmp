/* mpz_root(root, u, nth) --  Set ROOT to floor(U^(1/nth)).
   Return an indication if the result is exact.

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

/* Naive implementation of nth root extraction.  It would probably be a
   better idea to use a division-free Newton iteration.  It is insane
   to use full precision from iteration 1.  The mpz_scan1 trick compensates
   to some extent.  It would be natural to avoid representing the low zero
   bits mpz_scan1 is counting, and at the same time call mpn directly.  */

#include <stdio.h>  /* for NULL */
#include <stdlib.h> /* for abort */
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

int
mpz_root (mpz_ptr r, mpz_srcptr u, unsigned long int nth)
{
  mp_ptr rootp, up;
  mp_size_t us, un, rootn;
  int exact;
  unsigned int cnt;
  unsigned long int rootnb, unb;

  up = PTR(u);
  us = SIZ(u);

  /* even roots of negatives provoke an exception */
  if (us < 0 && (nth & 1) == 0)
    SQRT_OF_NEGATIVE;

  /* root extraction interpreted as c^(1/nth) means a zeroth root should
     provoke a divide by zero, do this even if c==0 */
  if (nth == 0)
    DIVIDE_BY_ZERO;

  if (us == 0)
    {
      if (r != NULL)
	SIZ(r) = 0;
      return 1;			/* exact result */
    }

  un = ABS (us);

  count_leading_zeros (cnt, up[un - 1]);
  unb = un * GMP_NUMB_BITS - cnt + GMP_NAIL_BITS;
  rootnb = (unb + nth - 1) / nth;
  rootn = (rootnb + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS;

  if (r != NULL)
    {
      rootp = MPZ_REALLOC (r, rootn + 1);
      up = PTR(u);
    }
  else
    {
      rootp = __GMP_ALLOCATE_FUNC_LIMBS (rootn + 1);
    }

  if (nth == 1)
    {
      MPN_COPY (rootp, up, un);
      exact = 1;
    }
  else
    {
      exact = 0 == mpn_rootrem (rootp, NULL, up, un, nth);
    }

  if (r != NULL)
    SIZ(r) = us >= 0 ? rootn : -rootn;
  else
    __GMP_FREE_FUNC_LIMBS (rootp, rootn + 1);

  return exact;
}
