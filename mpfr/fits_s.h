/* mpfr_fits_*_p -- test whether an mpfr fits a C signed type.

Copyright 2003 Free Software Foundation.
Contributed by the Spaces project, INRIA Lorraine.
Copied from mpf/fits_s.h.

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

int
FUNCTION (mpfr_srcptr f, mp_rnd_t rnd)
{
  mp_exp_t exp;
  mp_prec_t prec;
  TYPE s;
  mpfr_t x;
  int neg;
  int res;

  if (MPFR_IS_NAN(f) || MPFR_IS_INF(f))
    return 0; /* does not fit */

  if (MPFR_IS_ZERO(f))
    return 1; /* zero always fits */

  /* now it fits if either
     (a) MINIMUM <= f <= MAXIMUM
     (b) or MINIMUM <= round(f, prec(slong), rnd) <= MAXIMUM */

  exp = MPFR_EXP(f);
  if (exp < 1)
    return 1; /* |f| < 1: always fits */

  neg = (MPFR_SIGN(f) > 0) ? 0 : 1;

  /* let EXTREMUM be MAXIMUM if f > 0, and MINIMUM if f < 0 */

  /* first compute prec(EXTREMUM), this could be done at configure time */
  s = (neg) ? MINIMUM : MAXIMUM;
  for (prec = 0; s != 0; s /= 2, prec ++);

  /* EXTREMUM needs prec bits, i.e. 2^(prec-1) <= |EXTREMUM| < 2^prec */

   /* if exp < prec - 1, then f < 2^(prec-1) < |EXTREMUM| */
  if (exp < prec - 1)
    return 1;

  /* if exp > prec + 1, then f >= 2^prec > EXTREMUM */
  if (exp > prec + 1)
    return 0;

  /* remains cases exp = prec-1 to prec+1 */

  /* hard case: first round to prec bits, then check */
  mpfr_init2 (x, prec);
  mpfr_set (x, f, rnd);
  res = (neg) ? (mpfr_cmp_si (x, MINIMUM) >= 0)
    : (mpfr_cmp_si (x, MAXIMUM) <= 0);
  mpfr_clear (x);

  return res;
}

