/* mpfr_add_one_ulp, mpfr_sub_one_ulp -- add/subtract one unit in last place

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"

/* sets x to x+sign(x)*2^(EXP(x)-PREC(x)) */
int 
#if __STDC__
mpfr_add_one_ulp(mpfr_ptr x)
#else
mpfr_add_one_ulp(x)
     mpfr_ptr x; 
#endif
{
  int xn, sh; mp_limb_t *xp;

  xn = 1 + (PREC(x)-1)/BITS_PER_MP_LIMB;
  sh = xn*BITS_PER_MP_LIMB - PREC(x);
  xp = MANT(x);
  if (mpn_add_1(xp, xp, xn, (mp_limb_t)1<<sh)) {
    EXP(x)++;
    mpn_rshift(xp, xp, xn, 1);
    xp[xn-1] += (mp_limb_t)1<<(BITS_PER_MP_LIMB-1);
  }
  return 0;
}

/* sets x to x-sign(x)*ulp(x) */
int mpfr_sub_one_ulp(mpfr_ptr x)
{
  int xn, sh; mp_limb_t *xp;

  xn = 1 + (PREC(x)-1)/BITS_PER_MP_LIMB;
  sh = xn*BITS_PER_MP_LIMB-PREC(x);
  xp = MANT(x);
  mpn_sub_1(xp, xp, xn, (mp_limb_t)1<<sh);
  if (xp[xn-1] >> (BITS_PER_MP_LIMB-1) == 0) {
    /* was an exact power of two: not normalized any more */
    EXP(x)--;
    mpn_lshift(xp, xp, xn, 1);
    *xp |= ((mp_limb_t)1 << sh);
  }
  return 0;
}
