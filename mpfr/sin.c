/* mpfr_sin -- sine of a floating-point number

Copyright (C) 2001 Free Software Foundation, Inc.

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
#include "mpfr.h"
#include "mpfr-impl.h"

int 
mpfr_sin (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
{
  int precy, m, ok, e, inexact;
  mpfr_t c;

  if (MPFR_IS_NAN(x) || MPFR_IS_INF(x))
    {
      MPFR_SET_NAN(y);
      return 1; /* inexact */
    }

  if (!MPFR_NOTZERO(x))
    {
      mpfr_set_ui (y, 0, GMP_RNDN);
      return 0; /* exact */
    }

  precy = MPFR_PREC(y);
  m = precy + _mpfr_ceil_log2 ((double) precy) + ABS(MPFR_EXP(x)) + 13;

  mpfr_init2 (c, m);

  do
    {
      mpfr_cos (c, x, GMP_RNDZ);
      mpfr_mul (c, c, c, GMP_RNDU);
      mpfr_ui_sub (c, 1, c, GMP_RNDN);
      e = 2 + (-MPFR_EXP(c)) / 2;
      mpfr_sqrt (c, c, GMP_RNDN);
      if (mpfr_cmp_ui (x, 0) < 0)
	mpfr_neg (c, c, GMP_RNDN);

      /* the absolute error on c is at most 2^(e-m) = 2^(EXP(c)-err) */
      e = MPFR_EXP(c) + m - e;
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

  return inexact; /* inexact */
}
