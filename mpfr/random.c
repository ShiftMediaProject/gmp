/* mpfr_random -- generate a random floating-point number

Copyright 1999, 2001 Free Software Foundation, Inc.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* Computes a random mpfr in [0, 1[ with precision MPFR_PREC */

void
mpfr_random (mpfr_ptr x)
{
  mp_limb_t *xp; unsigned long xn, cnt, prec = MPFR_PREC(x); 

  MPFR_CLEAR_FLAGS(x);
  xp = MPFR_MANT(x);
  xn = (prec-1)/BITS_PER_MP_LIMB + 1;

  mpn_random (xp, xn);

  if (xp[xn - 1] == 0)
    xp[xn - 1] = 1;
  /* since count_leading_zeros doesn't like zeroes */

  count_leading_zeros (cnt, xp[xn - 1]); 
  if (cnt)
    mpn_lshift (xp, xp, xn, cnt); 
  MPFR_EXP(x) = -cnt; 
  if (MPFR_SIGN(x) < 0)
    MPFR_CHANGE_SIGN(x);

  cnt = xn * BITS_PER_MP_LIMB - prec; 
  /* cnt is the number of non significant bits in the low limb */
  xp[0] &= ~((MP_LIMB_T_ONE << cnt) - MP_LIMB_T_ONE);
}
