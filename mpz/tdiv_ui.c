/* mpz_tdiv_ui(dividend, divisor_limb) -- Return DIVDEND mod DIVISOR_LIMB.

Copyright 1991, 1993, 1994, 1996, 1997, 1998, 2001 Free Software Foundation,
Inc.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

unsigned long int
mpz_tdiv_ui (mpz_srcptr dividend, unsigned long int divisor)
{
  mp_size_t ns, nn;
  mp_ptr np;

  if (divisor == 0)
    DIVIDE_BY_ZERO;

  ns = SIZ(dividend);
  nn = ABS(ns);
  np = PTR(dividend);

#if GMP_NAIL_BITS != 0
  if (divisor > GMP_NUMB_MAX)
    {
      mp_limb_t dp[2], rp[2];
      mp_ptr qp;
      TMP_DECL (mark);

      TMP_MARK (mark);
      dp[0] = divisor & GMP_NUMB_MASK;
      dp[1] = divisor >> GMP_NUMB_BITS;
      qp = TMP_ALLOC_LIMBS (nn - 2 + 1);
      mpn_tdiv_qr (qp, rp, (mp_size_t) 0, np, nn, dp, (mp_size_t) 2);
      TMP_FREE (mark);
      return rp[0] + (rp[1] << GMP_NUMB_BITS);
    }
#endif

  return mpn_mod_1 (np, nn, (mp_limb_t) divisor);
}
