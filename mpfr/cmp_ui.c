/* mpfr_cmp_ui_2exp -- compare a floating-point number with an unsigned
machine integer multiplied by a power of 2

Copyright 1999, 2001, 2002 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* returns a positive value if b > i*2^f,
           a negative value if b < i*2^f,
           zero if b = i*2^f.
   b must not be NaN
*/

int 
mpfr_cmp_ui_2exp (mpfr_srcptr b, unsigned long int i, mp_exp_t f)
{
  MPFR_ASSERTN(!MPFR_IS_NAN(b));

  if (MPFR_IS_INF(b))
    return MPFR_SIGN(b);

  /* now b is neither NaN nor +/-Infinity */
  if (MPFR_IS_ZERO(b))
    return i != 0 ? -1 : 0;
  else if (MPFR_SIGN(b) < 0)
    return -1;
  /* now b > 0 */
  else if (i == 0)
    return 1;
  else /* b > 0, i > 0 */
    {
      mp_exp_t e;
      int k;
      mp_size_t bn;
      mp_limb_t c, *bp;

      /* i must be representable in a mp_limb_t */
      MPFR_ASSERTN(i == (mp_limb_t) i);

      e = MPFR_EXP(b); /* 2^(e-1) <= b < 2^e */
      if (e <= f)
        return -1;
      if (f < MPFR_EMAX_MAX - BITS_PER_MP_LIMB &&
          e > f + BITS_PER_MP_LIMB)
        return 1;

      /* now f < e <= f + BITS_PER_MP_LIMB */
      c = (mp_limb_t) i;
      count_leading_zeros(k, c);
      if ((int) (e - f) > BITS_PER_MP_LIMB - k)
        return 1;
      if ((int) (e - f) < BITS_PER_MP_LIMB - k)
        return -1;

      /* now b and i*2^f have the same exponent */
      c <<= k;
      bn = (MPFR_PREC(b) - 1) / BITS_PER_MP_LIMB;
      bp = MPFR_MANT(b);
      if (bp[bn] > c)
        return 1;
      if (bp[bn] < c)
        return -1;

      /* most significant limbs agree, check remaining limbs from b */
      while (bn > 0)
        if (bp[--bn] != 0)
          return 1;
      return 0;
    }
}
