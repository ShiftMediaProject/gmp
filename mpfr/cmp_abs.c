/* mpfr_cmp_abs -- compare the absolute values of two nonzero FP numbers

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

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* returns sign(abs(b) - abs(c))
   b and c must be nonzero real numbers */

int
mpfr_cmp_abs (mpfr_srcptr b, mpfr_srcptr c)
{
  mp_exp_t be, ce;
  mp_size_t bn, cn;
  mp_limb_t *bp, *cp;

  MPFR_ASSERTN(MPFR_IS_FP(b) && MPFR_NOTZERO(b));
  MPFR_ASSERTN(MPFR_IS_FP(c) && MPFR_NOTZERO(c));

  be = MPFR_EXP(b);
  ce = MPFR_EXP(c);
  if (be > ce)
    return 1;
  if (be < ce)
    return -1;

  /* exponents are equal */

  bn = (MPFR_PREC(b)-1)/BITS_PER_MP_LIMB;
  cn = (MPFR_PREC(c)-1)/BITS_PER_MP_LIMB;

  bp = MPFR_MANT(b);
  cp = MPFR_MANT(c);

  for ( ; bn >= 0 && cn >= 0; bn--, cn--)
    {
      if (bp[bn] > cp[cn])
        return 1;
      if (bp[bn] < cp[cn])
        return -1;
    }

  for ( ; bn >= 0; bn--)
    if (bp[bn])
      return 1;

  for ( ; cn >= 0; cn--)
    if (cp[cn])
      return -1;

   return 0;
}
