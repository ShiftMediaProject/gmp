/* mpfr_tan -- tangent of a floating-point number

Copyright (C) 2001-2002 Free Software Foundation, Inc.

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

/* computes tan(x) = sign(x)*sqrt(1/cos(x)^2-1) */
int 
mpfr_tan (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  int precy, m, ok, e, inexact;
  mpfr_t c;

  if (MPFR_IS_NAN(x) || MPFR_IS_INF(x))
    {
      MPFR_SET_NAN(y);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_ZERO(x))
    {
      MPFR_CLEAR_FLAGS(y);
      MPFR_SET_ZERO(y);
      MPFR_SET_SAME_SIGN(y, x);
      MPFR_RET(0);
    }

  precy = MPFR_PREC(y);
  m = precy + _mpfr_ceil_log2 ((double) precy) + ABS(MPFR_EXP(x)) + 13;

  mpfr_init2 (c, m);

  do
    {
      mpfr_cos (c, x, GMP_RNDZ);
      mpfr_mul (c, c, c, GMP_RNDZ);
      mpfr_ui_div (c, 1, c, GMP_RNDU);
      e = MPFR_EXP(c);
      mpfr_sub_ui (c, c, 1, GMP_RNDN);
      e = e - MPFR_EXP(c);
      mpfr_sqrt (c, c, GMP_RNDU);
      if (mpfr_cmp_ui (x, 0) < 0)
	mpfr_neg (c, c, GMP_RNDN);

      /* the error on c is at most (2+11*2^e)*ulp(c) <= 2^(e+4)*ulp(c)
	 = 2^(e+4+EXP(c)-m) = 2^(EXP(c)-err) */
      e = m - e - 4;
      ok = (e >= 0) && mpfr_can_round (c, e, GMP_RNDN, rnd_mode, precy);

      if (ok == 0)
	{
	  m += BITS_PER_MP_LIMB;
	  mpfr_set_prec (c, m);
	}
    }
  while (!ok);

  inexact = mpfr_set (y, c, rnd_mode);

  mpfr_clear (c);

  return inexact;
}
