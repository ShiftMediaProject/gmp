/* mpz_divexact_si -- exact division mpz by long.

Copyright 2001 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/

#include "gmp.h"
#include "gmp-impl.h"

void
mpz_divexact_si (mpz_ptr dst, mpz_srcptr src, long divisor)
{
  mp_size_t  size, abs_size;
  mp_ptr     dp;

  if (divisor == 0)
    DIVIDE_BY_ZERO;

  size = SIZ(src);
  if (size == 0)
    {
      SIZ(dst) = 0;
      return;
    }

  if (divisor < 0)
    size = -size;
  divisor = ABS (divisor);

  abs_size = ABS (size);
  MPZ_REALLOC (dst, abs_size);
  dp = PTR(dst);

  MPN_DIVREM_OR_DIVEXACT_1 (dp, PTR(src), abs_size,
                            (mp_limb_t) (unsigned long) divisor);
  abs_size -= (dp[abs_size-1] == 0);
  SIZ(dst) = (size >= 0 ? abs_size : -abs_size);
}
