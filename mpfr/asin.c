/* mpfr_asin -- arc-sinus of a floating-point number

Copyright 2001, 2002, 2003 Free Software Foundation.

This file is part of the MPFR Library, and was contributed by Mathieu Dutour.

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

int
mpfr_asin (mpfr_ptr asin, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  mpfr_t xp;
  mpfr_t arcs;

  int signe, supplement;

  mpfr_t tmp;
  int Prec;
  int prec_asin;
  int good = 0;
  int realprec;
  int estimated_delta;
  int compared;
  int inexact;

  /* Trivial cases */
  if (MPFR_IS_NAN(x) || MPFR_IS_INF(x))
    {
      MPFR_SET_NAN(asin);
      MPFR_RET_NAN;
    }

  /* Set x_p=|x| */
  signe = MPFR_SIGN(x);
  mpfr_init2 (xp, MPFR_PREC(x));
  mpfr_set (xp, x, rnd_mode);
  if (signe == -1)
    MPFR_CHANGE_SIGN(xp);

  compared = mpfr_cmp_ui (xp, 1);

  if (compared > 0) /* asin(x) = NaN for |x| > 1 */
    {
      MPFR_SET_NAN(asin);
      mpfr_clear (xp);
      MPFR_RET_NAN;
    }

  if (compared == 0) /* x = 1 or x = -1 */
    {
      if (signe > 0) /* asin(+1) = Pi/2 */
        inexact = mpfr_const_pi (asin, rnd_mode);
      else /* asin(-1) = -Pi/2 */
        {
          if (rnd_mode == GMP_RNDU)
            rnd_mode = GMP_RNDD;
          else if (rnd_mode == GMP_RNDD)
            rnd_mode = GMP_RNDU;
          inexact = -mpfr_const_pi (asin, rnd_mode);
          mpfr_neg (asin, asin, rnd_mode);
        }
      MPFR_SET_EXP (asin, MPFR_GET_EXP (asin) - 1);
      mpfr_clear (xp);
      return inexact;
    }

  if (MPFR_IS_ZERO(x)) /* x = 0 */
    {
      mpfr_set_ui (asin, 0, GMP_RNDN);
      mpfr_clear (xp);
      return 0; /* exact result */
    }

  prec_asin = MPFR_PREC(asin);
  mpfr_ui_sub (xp, 1, xp, GMP_RNDD);

  supplement = 2 - MPFR_GET_EXP (xp);
  realprec = prec_asin + 10;

  while (!good)
    {
      estimated_delta = 1 + supplement;
      Prec = realprec+estimated_delta;

      /* Initialisation    */
      mpfr_init2 (tmp, Prec);
      mpfr_init2 (arcs, Prec);
      mpfr_mul (tmp, x, x, GMP_RNDN);
      mpfr_ui_sub (tmp, 1, tmp, GMP_RNDN);
      mpfr_sqrt (tmp, tmp, GMP_RNDN);
      mpfr_div (tmp, x, tmp, GMP_RNDN);
      mpfr_atan (arcs, tmp, GMP_RNDN);
      if (mpfr_can_round (arcs, realprec, GMP_RNDN, GMP_RNDZ,
                          MPFR_PREC(asin) + (rnd_mode == GMP_RNDN)))
	{
	  inexact = mpfr_set (asin, arcs, rnd_mode);
	  good = 1;
	}
      else
	{
	  realprec += __gmpfr_ceil_log2 ((double) realprec);
	}
      mpfr_clear (tmp);
      mpfr_clear (arcs);
    }

  mpfr_clear (xp);

  return inexact;
}
