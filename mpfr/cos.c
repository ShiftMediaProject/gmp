/* mpfr_cos -- cosine of a floating-point number

Copyright 2001, 2002 Free Software Foundation.

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

static int mpfr_cos2_aux       _PROTO ((mpfr_ptr, mpfr_srcptr));

int 
mpfr_cos (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
{
  int K0, K, precy, m, k, l, inexact;
  mpfr_t r, s;

  if (MPFR_IS_NAN(x) || MPFR_IS_INF(x))
    {
      MPFR_SET_NAN(y);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_ZERO(x))
    {
      mpfr_set_ui (y, 1, GMP_RNDN);
      return 0;
    }

  precy = MPFR_PREC(y);

  K0 = _mpfr_isqrt(precy / 2);
  /* we need at least K + log2(precy/K) extra bits */
  m = precy + 3 * K0 + 3;

  mpfr_init2 (r, m);
  mpfr_init2 (s, m);

  do
    {
      mpfr_mul (r, x, x, GMP_RNDU); /* err <= 1 ulp */

      /* we need that |r| < 1 for mpfr_cos2_aux, i.e. up(x^2)/2^(2K) < 1 */
      K = K0 + MAX(MPFR_EXP(r), 0);

      mpfr_div_2ui (r, r, 2 * K, GMP_RNDN); /* r = (x/2^K)^2, err <= 1 ulp */

      /* s <- 1 - r/2! + ... + (-1)^l r^l/(2l)! */
      l = mpfr_cos2_aux (s, r);

      for (k = 0; k < K; k++)
	{
	  mpfr_mul (s, s, s, GMP_RNDU); /* err <= 2*olderr */
	  mpfr_mul_2ui (s, s, 1, GMP_RNDU); /* err <= 4*olderr */
	  mpfr_sub_ui (s, s, 1, GMP_RNDN);
	}

      /* absolute error on s is bounded by (2l+1/3)*2^(2K-m) */
      for (k = 2 * K, l = 2 * l + 1; l > 1; k++, l = (l + 1) >> 1);
      /* now the error is bounded by 2^(k-m) = 2^(EXP(s)-err) */

      l = mpfr_can_round (s, MPFR_EXP(s) + m - k, GMP_RNDN, rnd_mode, precy);

      if (l == 0)
	{
	  m += BITS_PER_MP_LIMB;
	  mpfr_set_prec (r, m);
	  mpfr_set_prec (s, m);
	}
    }
  while (l == 0);

  inexact = mpfr_set (y, s, rnd_mode);

  mpfr_clear (r);
  mpfr_clear (s);

  return inexact;
}

/* s <- 1 - r/2! + r^2/4! + ... + (-1)^l r^l/(2l)! + ...
   Assumes |r| < 1.
   Returns the index l0 of the last term (-1)^l r^l/(2l)!.
   The absolute error on s is at most 2 * l0 * 2^(-m).
*/
static int
mpfr_cos2_aux (mpfr_ptr s, mpfr_srcptr r)
{
  unsigned int l, b = 2;
  long int prec_t, m = MPFR_PREC(s);
  mpfr_t t;

  MPFR_ASSERTN (MPFR_EXP(r) <= 0);
  mpfr_init2 (t, m);
  mpfr_set_ui (t, 1, GMP_RNDN);
  mpfr_set_ui(s, 1, GMP_RNDN);

  for (l = 1; MPFR_EXP(t) + m >= 0; l++)
    {
      mpfr_mul (t, t, r, GMP_RNDU); /* err <= (3l-1) ulp */
      mpfr_div_ui (t, t, (2*l-1)*(2*l), GMP_RNDU); /* err <= 3l ulp */
      if (l % 2 == 0)
	mpfr_add (s, s, t, GMP_RNDD);
      else
	mpfr_sub (s, s, t, GMP_RNDD);
      MPFR_ASSERTN (MPFR_EXP(s) == 0); /* check 1/2 <= s < 1 */
      /* err(s) <= l * 2^(-m) */
      if (3 * l > (1 << b))
	b++;
      /* now 3l <= 2^b, we want 3l*ulp(t) <= 2^(-m)
	 i.e. b+EXP(t)-PREC(t) <= -m */
      prec_t = m + MPFR_EXP(t) + b;
      if (prec_t >= MPFR_PREC_MIN)
	mpfr_round_prec (t, GMP_RNDN, prec_t);
    }

  mpfr_clear (t);

  return l;
}

