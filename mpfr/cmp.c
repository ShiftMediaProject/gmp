/* mpfr_cmp -- compare two floating-point numbers

Copyright 1999, 2001 Free Software Foundation.

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
#include "mpfr.h"
#include "mpfr-impl.h"

/* returns 0 iff b = sign(s) * c
           a positive value iff b > sign(s) * c
           a negative value iff b < sign(s) * c
*/

int
mpfr_cmp3 (mpfr_srcptr b, mpfr_srcptr c, int s)
{
  mp_exp_t be, ce;
  mp_size_t bn, cn;
  mp_limb_t *bp, *cp;

  MPFR_ASSERTN(!MPFR_IS_NAN(b));
  MPFR_ASSERTN(!MPFR_IS_NAN(c));
  s *= MPFR_SIGN(c);

  if (MPFR_IS_INF(b))
    return (MPFR_IS_INF(c) && s * MPFR_SIGN(b) > 0) ? 0 : MPFR_SIGN(b);

  if (MPFR_IS_INF(c))
    return -s;

  /* b and c are real numbers */

  if (MPFR_IS_ZERO(b))
    return MPFR_IS_ZERO(c) ? 0 : -s;

  if (MPFR_IS_ZERO(c))
    return MPFR_SIGN(b);

  if (s * MPFR_SIGN(b) < 0)
    return MPFR_SIGN(b);

  /* now signs are equal */

  be = MPFR_EXP(b);
  ce = MPFR_EXP(c);
  if (be > ce)
    return s;
  if (be < ce)
    return -s;

  /* both signs and exponents are equal */

  bn = (MPFR_PREC(b)-1)/BITS_PER_MP_LIMB;
  cn = (MPFR_PREC(c)-1)/BITS_PER_MP_LIMB;

  bp = MPFR_MANT(b);
  cp = MPFR_MANT(c);

  for ( ; bn >= 0 && cn >= 0; bn--, cn--)
    {
      if (bp[bn] > cp[cn])
        return s;
      if (bp[bn] < cp[cn])
        return -s;
    }

  for ( ; bn >= 0; bn--)
    if (bp[bn])
      return s;

  for ( ; cn >= 0; cn--)
    if (cp[cn])
      return -s;

   return 0;
}
