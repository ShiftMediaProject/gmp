/* mpfr_set_si -- set a MPFR number from a machine signed integer

Copyright 1999, 2000, 2001 Free Software Foundation.

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

int
mpfr_set_si (mpfr_ptr x, long i, mp_rnd_t rnd_mode)
{
  int inex;
  mp_size_t xn;
  unsigned int cnt, nbits;
  mp_limb_t ai, *xp;

  MPFR_CLEAR_FLAGS(x);
  if (i == 0)
    {
      MPFR_SET_ZERO(x);
      MPFR_SET_POS(x);
      MPFR_RET(0);
    }

  xn = (MPFR_PREC(x)-1)/BITS_PER_MP_LIMB;
  ai = SAFE_ABS(long, i);
  count_leading_zeros(cnt, ai);

  xp = MPFR_MANT(x);
  xp[xn] = ai << cnt;
  /* don't forget to put zero in lower limbs */
  MPN_ZERO(xp, xn);
  /* set sign */
  if ((i < 0) ^ (MPFR_SIGN(x) < 0))
    MPFR_CHANGE_SIGN(x);

  MPFR_EXP(x) = nbits = BITS_PER_MP_LIMB - cnt;
  inex = mpfr_check_range(x, rnd_mode);
  if (inex)
    return inex; /* underflow or overflow */

  /* round if MPFR_PREC(x) smaller than length of i */
  if (MPFR_PREC(x) < nbits)
    {
      int carry;

      carry = mpfr_round_raw(xp+xn, xp+xn, nbits, (i < 0), MPFR_PREC(x),
                             rnd_mode, &inex);
      if (carry)
        {
          mp_exp_t exp = MPFR_EXP(x);

          if (exp == __mpfr_emax)
            return mpfr_set_overflow(x, rnd_mode, (i < 0 ? -1 : 1));

          MPFR_EXP(x)++;
          xp[xn] = GMP_LIMB_HIGHBIT;
        }
    }

  MPFR_RET(inex);
}
