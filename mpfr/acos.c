/* mpfr_acos -- arc-cosinus of a floating-point number

Copyright 2001 Free Software Foundation.

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

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_acos (mpfr_ptr acos, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  mpfr_t xp;
  mpfr_t arcc;

  int signe, suplement;

  mpfr_t tmp;
  int Prec;
  int prec_acos;
  int good = 0;
  int realprec;
  int compared;
  int inexact = 0;

  /* Trivial cases */
  if (MPFR_IS_NAN(x) || MPFR_IS_INF(x))
    {
      MPFR_SET_NAN(acos);
      MPFR_RET_NAN;
    }

  /* Set x_p=|x| */
  signe = MPFR_SIGN(x);
  mpfr_init2 (xp, MPFR_PREC(x));
  mpfr_abs (xp, x, rnd_mode);

  compared = mpfr_cmp_ui (xp, 1);

  if (compared > 0) /* acos(x) = NaN for x > 1 */
    {
      mpfr_clear (xp);
      MPFR_SET_NAN(acos);
      MPFR_RET_NAN;
    }

  if (compared == 0)
    {
      mpfr_clear (xp);
      if (signe > 0) /* acos(+1) = 0 */
	return mpfr_set_ui (acos, 0, rnd_mode);
      else /* acos(-1) = Pi */
        {
          mpfr_const_pi (acos, rnd_mode);
          return 1; /* inexact */
        }
    }

  if (MPFR_IS_ZERO(x)) /* acos(0)=Pi/2 */
    {
      mpfr_clear (xp);
      mpfr_const_pi (acos, rnd_mode);
      MPFR_EXP(acos)--;
      return 1; /* inexact */
    }

  prec_acos = MPFR_PREC(acos);
  mpfr_ui_sub (xp, 1, xp, GMP_RNDD);
  
  if (signe > 0)
      suplement = 2 - 2 * MPFR_EXP(xp);
  else
      suplement = 2 - MPFR_EXP(xp);

  realprec = prec_acos + 10;

  while (!good)
    {
      Prec = realprec+suplement;

      /* Initialisation    */
      mpfr_init2 (tmp, Prec);
      mpfr_init2 (arcc, Prec);

      mpfr_mul (tmp, x, x, GMP_RNDN);
      mpfr_ui_sub (tmp, 1, tmp, GMP_RNDN);
      mpfr_sqrt (tmp, tmp, GMP_RNDN);
      mpfr_div (tmp, x, tmp, GMP_RNDN);
      mpfr_atan (arcc, tmp, GMP_RNDN);
      mpfr_const_pi (tmp, GMP_RNDN);
      mpfr_div_2ui (tmp, tmp, 1, GMP_RNDN);
      mpfr_sub (arcc, tmp, arcc, GMP_RNDN);

      if (mpfr_can_round (arcc, realprec, GMP_RNDN, rnd_mode, MPFR_PREC(acos)))
        {
          inexact = mpfr_set (acos, arcc, rnd_mode);
          good = 1;
        }
      else
        realprec += _mpfr_ceil_log2 ((double) realprec);

    mpfr_clear (tmp);
    mpfr_clear (arcc);
  }

  mpfr_clear (xp);
  return inexact;
}
