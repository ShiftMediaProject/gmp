/* mpfr_exp -- exponential of a floating-point number

Copyright 1999, 2000, 2001, 2002 Free Software Foundation.
Contributed by the Spaces project.

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

/* #define DEBUG */

/* use Brent's formula exp(x) = (1+r+r^2/2!+r^3/3!+...)^(2^K)*2^n
   where x = n*log(2)+(2^K)*r
   number of operations = O(K+prec(r)/K)
*/
int 
mpfr_exp (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
{
  int expx, precy;
  double d;

  if (MPFR_IS_NAN(x))
    {
      MPFR_SET_NAN(y);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(y);

  if (MPFR_IS_INF(x))
    {
      if (MPFR_SIGN(x) > 0)
	{
	  MPFR_SET_INF(y);
	}
      else
	{
	  MPFR_CLEAR_INF(y);
	  MPFR_SET_ZERO(y);
	}
      MPFR_SET_POS(y);
      MPFR_RET(0);
    }

  MPFR_CLEAR_INF(y);

  if (MPFR_IS_ZERO(x))
    return mpfr_set_ui (y, 1, GMP_RNDN);

  expx = MPFR_EXP(x);
  precy = MPFR_PREC(y);

  /* result is +Inf when exp(x) >= 2^(__mpfr_emax), i.e.
     x >= __mpfr_emax * log(2) */
  d = mpfr_get_d1 (x);
  if (d >= (double) __mpfr_emax * LOG2)
    return mpfr_set_overflow(y, rnd_mode, 1);

  /* result is 0 when exp(x) < 1/2*2^(__mpfr_emin), i.e.
     x < (__mpfr_emin-1) * LOG2 */
  if (d < ((double) __mpfr_emin - 1.0) * LOG2)
    return mpfr_set_underflow(y, rnd_mode, 1);

  /* if x < 2^(-precy), then exp(x) i.e. gives 1 +/- 1 ulp(1) */
  if (expx < -precy)
    {
      int signx = MPFR_SIGN(x);

      mpfr_set_ui (y, 1, rnd_mode);
      if (signx > 0 && rnd_mode == GMP_RNDU)
	{
	  mpfr_add_one_ulp (y, rnd_mode);
	  return 1;
	}
      else if (signx < 0 && (rnd_mode == GMP_RNDD || rnd_mode == GMP_RNDZ))
	{
	  mpfr_sub_one_ulp (y, rnd_mode);
	  return -1;
	}
      return -signx;
    }

  if (precy > 13000)
    return mpfr_exp3 (y, x, rnd_mode); /* O(M(n) log(n)^2) */
  else
    return mpfr_exp_2 (y, x, rnd_mode); /* O(n^(1/3) M(n)) */
}
