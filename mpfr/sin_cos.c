/* mpfr_sin_cos -- sine and cosine of a floating-point number

Copyright 2002 Free Software Foundation, Inc.

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

/* (y, z) <- (sin(x), cos(x)), return value is 0 iff both results are exact */
int 
mpfr_sin_cos (mpfr_ptr y, mpfr_ptr z, mpfr_srcptr x, mp_rnd_t rnd_mode) 
{
  int prec, m, ok, e, inexact, neg;
  mpfr_t c, k;

  if (MPFR_IS_NAN(x) || MPFR_IS_INF(x))
    {
      MPFR_SET_NAN(y);
      MPFR_SET_NAN(z);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_ZERO(x))
    {
      MPFR_CLEAR_FLAGS(y);
      MPFR_SET_ZERO(y);
      MPFR_SET_SAME_SIGN(y, x);
      mpfr_set_ui (z, 1, GMP_RNDN);
      MPFR_RET(0);
    }

  prec = MAX(MPFR_PREC(y), MPFR_PREC(z));
  m = prec + _mpfr_ceil_log2 ((double) prec) + ABS(MPFR_EXP(x)) + 13;

  mpfr_init2 (c, m);
  mpfr_init2 (k, m);

  /* first determine sign */
  mpfr_const_pi (c, GMP_RNDN);
  mpfr_mul_2ui (c, c, 1, GMP_RNDN); /* 2*Pi */
  mpfr_div (k, x, c, GMP_RNDN);      /* x/(2*Pi) */
  mpfr_floor (k, k);                 /* floor(x/(2*Pi)) */
  mpfr_mul (c, k, c, GMP_RNDN);
  mpfr_sub (k, x, c, GMP_RNDN);      /* 0 <= k < 2*Pi */
  mpfr_const_pi (c, GMP_RNDN); /* cached */
  neg = mpfr_cmp (k, c) > 0;
  mpfr_clear (k);

  do
    {
      mpfr_cos (c, x, GMP_RNDZ);
      if ((ok = mpfr_can_round (c, m, GMP_RNDZ, rnd_mode, MPFR_PREC(z))))
        {
          inexact = mpfr_set (z, c, rnd_mode);
          mpfr_mul (c, c, c, GMP_RNDU);
          mpfr_ui_sub (c, 1, c, GMP_RNDN);
          e = 2 + (-MPFR_EXP(c)) / 2;
          mpfr_sqrt (c, c, GMP_RNDN);
          if (neg)
            mpfr_neg (c, c, GMP_RNDN);

          /* the absolute error on c is at most 2^(e-m) = 2^(EXP(c)-err) */
          e = MPFR_EXP(c) + m - e;
          ok = (e >= 0) && mpfr_can_round (c, e, GMP_RNDN, rnd_mode,
                                           MPFR_PREC(y));
        }

      if (ok == 0)
	{
	  m += _mpfr_ceil_log2 ((double) m);
	  mpfr_set_prec (c, m);
	}
    }
  while (ok == 0);

  inexact = mpfr_set (y, c, rnd_mode) || inexact;

  mpfr_clear (c);

  return inexact; /* inexact */
}
