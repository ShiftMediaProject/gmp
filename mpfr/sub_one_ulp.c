/* mpfr_sub_one_ulp -- subtract one unit in last place

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

/* sets x to x-sign(x)*ulp(x) */
int
mpfr_sub_one_ulp(mpfr_ptr x, mp_rnd_t rnd_mode)
{
  mp_size_t xn;
  int sh;
  mp_limb_t *xp;

  if (MPFR_IS_NAN(x))
    MPFR_RET_NAN;

  if (MPFR_IS_INF(x) || MPFR_IS_ZERO(x))
    return 0;

  MPFR_ASSERTN(MPFR_PREC_MIN > 1);

  xn = 1 + (MPFR_PREC(x) - 1) / BITS_PER_MP_LIMB;
  sh = xn * BITS_PER_MP_LIMB - MPFR_PREC(x);
  xp = MPFR_MANT(x);
  mpn_sub_1 (xp, xp, xn, MP_LIMB_T_ONE << sh);
  if (xp[xn-1] >> (BITS_PER_MP_LIMB - 1) == 0)
    { /* was an exact power of two: not normalized any more */
      mp_exp_t exp = MPFR_EXP(x);
      if (exp == __mpfr_emin)
        return mpfr_set_underflow(x, rnd_mode, MPFR_SIGN(x));
      else
        {
          int i;
          MPFR_EXP(x)--;
          xp[0] = (sh + 1 == BITS_PER_MP_LIMB) ? 0 : MP_LIMB_T_MAX << (sh + 1);
          for (i = 1; i < xn; i++)
            xp[i] = MP_LIMB_T_MAX;
        }
    }
  return 0;
}
