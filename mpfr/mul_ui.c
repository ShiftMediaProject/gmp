/* mpfr_mul_ui -- multiply a floating-point number by a machine integer

Copyright (C) 1999, 2001, 2002 Free Software Foundation, Inc.

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

int
mpfr_mul_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int u, mp_rnd_t rnd_mode)
{
  mp_limb_t *my, *old_my;
  unsigned long xn, yn, cnt, c;
  int inexact;
  TMP_DECL(marker);

  if (MPFR_IS_NAN(x))
    {
      MPFR_CLEAR_FLAGS(y);
      MPFR_SET_NAN(y);
      return 1; /* a NaN is always inexact */
    }

  if (MPFR_IS_INF(x)) 
    {
      MPFR_CLEAR_FLAGS(y);
      if (u) 
	{ 
	  MPFR_SET_INF(y); 
	  if (MPFR_SIGN(y) != MPFR_SIGN(x))
	    MPFR_CHANGE_SIGN(y);
	  return 0; /* infinity is exact */
	}
      else /* 0 * infinity */
	{
	  MPFR_SET_NAN(y);
	  return 1; /* NaN is inexact */
	}
    }

  if (MPFR_IS_ZERO(x) || !u) 
    {
      MPFR_CLEAR_FLAGS(y);
      MPFR_SET_ZERO(y);
      return 0; /* zero is exact */
    }

  MPFR_CLEAR_FLAGS(y);

  if (u == 1)
    return mpfr_set (y, x, rnd_mode);

  TMP_MARK(marker);
  my = MPFR_MANT(y);
  yn = (MPFR_PREC(y) - 1) / BITS_PER_MP_LIMB + 1;
  xn = (MPFR_PREC(x) - 1) / BITS_PER_MP_LIMB + 1;

  old_my = my;

  if (yn < xn + 1)
    my = (mp_ptr) TMP_ALLOC ((xn + 1) * BYTES_PER_MP_LIMB);

  my[xn] = mpn_mul_1 (my, MPFR_MANT(x), xn, u);
  
  /* x * u is stored in my[xn], ..., my[0] */

  /* since the case u=1 was treated above, we have u >= 2, thus
     my[xn] >= 1 since x was msb-normalized */
  MPFR_ASSERTN(my[xn] != 0);
  cnt = 0;
  if ((my[xn] & MP_LIMB_T_HIGHBIT) == 0)
    {
      count_leading_zeros(cnt, my[xn]);
      mpn_lshift (my, my, xn + 1, cnt);
    }

  /* now my[xn], ..., my[0] is msb-normalized too, and has at most
     PREC(x) + (BITS_PER_MP_LIMB - cnt) non-zero bits */

  c = mpfr_round_raw (old_my, my, (xn + 1) * BITS_PER_MP_LIMB,
		      (MPFR_SIGN(x) < 0), MPFR_PREC(y), rnd_mode, &inexact);

  MPFR_EXP(y) = MPFR_EXP(x) + BITS_PER_MP_LIMB - cnt; 

  if (c) /* rounded result is 1.0000000000000000... */
    {
      old_my[yn-1] = MP_LIMB_T_HIGHBIT;
      MPFR_EXP(y) ++;
    }

  /* set sign */
  if (MPFR_SIGN(y) * MPFR_SIGN(x) < 0)
    MPFR_CHANGE_SIGN(y);

  TMP_FREE(marker);

  return inexact;
}
