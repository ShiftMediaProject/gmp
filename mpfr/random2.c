/* mpf_random2 -- Generate a positive random mpf_t of specified size, with
   long runs of consecutive ones and zeros in the binary representation.
   Intended for testing of other MP routines.

Copyright (C) 1995, 1996 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void
#if __STDC__
mpfr_random2 (mpfr_ptr x, mp_size_t size, mp_exp_t exp)
#else
mpfr_random2 (x, size, exp)
     mpfr_ptr x;
     mp_size_t size;
     mp_exp_t exp;
#endif
{
  mp_size_t xn;
  unsigned long cnt;
  mp_ptr xp = MPFR_MANT(x), yp[1];
  mp_size_t prec = (MPFR_PREC(x) - 1)/BITS_PER_MP_LIMB; 

  MPFR_CLEAR_FLAGS(x);
  xn = ABS (size);
  if (xn != 0)
    {
      if (xn > prec + 1)
	xn = prec + 1;

      mpn_random2 (xp, xn);
    }

  if (exp != 0) {
    /* use mpn_random instead of random since that function is not
       available on all platforms (for example HPUX, DEC OSF, ...) */
    mpn_random ((mp_limb_t*) yp, 1);
    exp = (mp_exp_t) yp[0] % (2 * exp) - exp;
  }

  count_leading_zeros(cnt, xp[xn - 1]); 
  if (cnt) mpn_lshift(xp, xp, xn, cnt); 
  MPFR_EXP(x) = exp-cnt; 
  cnt = xn*BITS_PER_MP_LIMB - prec; 
  /* cnt is the number of non significant bits in the low limb */
  xp[0] &= ~((((mp_limb_t)1)<<cnt) - 1);
}
