/* mpfr_sin -- sine of a floating-point number

Copyright 2001, 2002 Free Software Foundation, Inc.

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

int mpfr_sin_sign _PROTO((mpfr_srcptr));

/* determine the sign of sin(x) using argument reduction.
   Assumes x is not an exact multiple of Pi (this excludes x=0). */
int
mpfr_sin_sign (mpfr_srcptr x)
{
  mpfr_t c, k;
  mp_exp_t K;
  int sign = 0;
  mp_prec_t m;
  mpfr_srcptr y;

  m = MPFR_PREC(x);

  mpfr_init2 (c, 2);
  mpfr_init2 (k, 2);

  do
    {
      m += BITS_PER_MP_LIMB;

      mpfr_set_prec (c, m);
      mpfr_set_prec (k, m);

      /* first determine round(x/(2*Pi)): does not have to be exact since
         the result is an integer */
      mpfr_const_pi (c, GMP_RNDN); /* err <= ulp(c) = 2^(2-m) */
      mpfr_div (k, x, c, GMP_RNDN);
      MPFR_EXP(k) --; /* x/(2Pi) = 1/2*(x/Pi) */
      mpfr_rint (k, k, GMP_RNDN);

      if (MPFR_NOTZERO(k))
        {
          K = MPFR_EXP(k); /* k is an integer, thus K >= 1 */
          mpfr_mul (k, k, c, GMP_RNDN); /* err <= 2^(K+3-m) */
          MPFR_EXP(k) ++;
          mpfr_sub (k, x, k, GMP_RNDN); /* err<=2^(4-m)+2^(K+3-m)<=2^(K+4-m) */
          y = k;
        }
      else
        {
          K = 1;
          y = x;
        }
      if (mpfr_cmp (y, c) >= 0)
        {
          mpfr_sub (k, y, c, GMP_RNDN); /* err <= 2^(2-m)+2^(K+4-m)+2^(2-m)
                                           = 2^(3-m) + 2^(K+4-m) */
          y = k;
        }
    }
  while (MPFR_IS_ZERO(y) || (MPFR_EXP(y) < K + 5 - (mp_exp_t) m));

  sign = MPFR_SIGN(y);

  mpfr_clear (k);
  mpfr_clear (c);
  
  return sign;
}

int 
mpfr_sin (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
{
  int precy, m, ok, e, inexact, sign;
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
  m = precy + __gmpfr_ceil_log2 ((double) precy) + MAX(0,MPFR_EXP(x)) + 13;
  
  sign = mpfr_sin_sign (x);

  mpfr_init2 (c, m);

  do
    {
      mpfr_cos (c, x, GMP_RNDZ);
      mpfr_mul (c, c, c, GMP_RNDU);
      mpfr_ui_sub (c, 1, c, GMP_RNDN);
      e = 2 + (-MPFR_EXP(c)) / 2;
      mpfr_sqrt (c, c, GMP_RNDN);
      if (sign < 0)
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
