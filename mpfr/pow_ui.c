/* mpfr_pow_ui-- compute the power of a floating-point
                                  by a machine integer

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

/* sets x to y^n, and return 0 if exact, non-zero otherwise */
int
mpfr_pow_ui (mpfr_ptr x, mpfr_srcptr y, unsigned long int n, mp_rnd_t rnd)
{
  unsigned long m;
  mpfr_t res;
  mp_prec_t prec, err;
  int inexact;
  mp_rnd_t rnd1;

  if (MPFR_IS_NAN(y))
    {
      MPFR_SET_NAN(x);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(x);

  if (n == 0) /* y^0 = 1 for any y */
    {
      /* The return mpfr_set_ui is important as 1 isn't necessarily
         in the exponent range. */
      return mpfr_set_ui (x, 1, rnd);
    }

  if (MPFR_IS_INF(y))
    {
      /* Inf^n = Inf, (-Inf)^n = Inf for n even, -Inf for n odd */
      if ((MPFR_SIGN(y) < 0) && (n % 2 == 1))
        MPFR_SET_NEG(x);
      else
        MPFR_SET_POS(x);
      MPFR_SET_INF(x);
      MPFR_RET(0);
    }

  MPFR_CLEAR_INF(x);

  if (MPFR_IS_ZERO(y)) /* 0^n = 0 for any n */
    {
      MPFR_SET_ZERO(x);
      MPFR_RET(0);
    }

  mpfr_save_emin_emax ();
  mpfr_init (res);

  prec = MPFR_PREC(x);

  rnd1 = (MPFR_SIGN(y) > 0) ? GMP_RNDU : GMP_RNDD; /* away */

  do
    {
      int i;

      prec += 3;
      for (m = n, i = 0; m; i++, m >>= 1, prec++)
        ;
      mpfr_set_prec (res, prec);
      inexact = mpfr_set (res, y, rnd1);
      err = prec <= i ? 0 : prec - i;
      MPFR_ASSERTD (i >= 1);
      /* now 2^(i-1) <= n < 2^i */
      for (i -= 2; i >= 0; i--)
        {
          if (mpfr_mul (res, res, res, GMP_RNDU))
            inexact = 1;
          if (n & (1UL << i))
            if (mpfr_mul (res, res, y, rnd1))
              inexact = 1;
        }

      /* FIXME: infinity and 0 should be checked too. */
      MPFR_ASSERTN (MPFR_IS_FP (res));
      MPFR_ASSERTN (MPFR_NOTZERO (res));
      /* check underflow */
      if (MPFR_GET_EXP (res) <= __gmpfr_emin)
        {
          mpfr_clear (res);
          mpfr_restore_emin_emax ();
          return mpfr_set_underflow (x, rnd, (n % 2) ? MPFR_SIGN(y) : 1);
        }
    }
  while (inexact && !mpfr_can_round (res, err, GMP_RNDN, GMP_RNDZ,
                                     MPFR_PREC(x) + (rnd == GMP_RNDN)));

  inexact = mpfr_set (x, res, rnd);
  mpfr_clear (res);
  mpfr_restore_emin_emax ();
  return mpfr_check_range (x, inexact, rnd);
}
