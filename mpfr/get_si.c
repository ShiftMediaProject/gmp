/* mpfr_get_si -- convert a floating-point number to a signed long.

Copyright 2003 Free Software Foundation, Inc.

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

long
mpfr_get_si (mpfr_srcptr f, mp_rnd_t rnd)
{
  mp_prec_t prec;
  long s;
  mpfr_t x;
  mp_size_t n;
  mp_exp_t exp;

  if (!mpfr_fits_slong_p (f, rnd) || MPFR_IS_ZERO(f))
    return (long) 0;

  /* determine prec of long */
  for (s = LONG_MIN, prec = 0; s != 0; s /= 2, prec ++);

  /* first round to prec bits */
  mpfr_init2 (x, prec);
  mpfr_set (x, f, rnd);

  ASSERT(GMP_NAIL_BITS == 0); /* otherwise we may have to consider two or
				 more limbs */

  /* now the result is in the most significant limb of x */
  exp = MPFR_GET_EXP (x); /* since |x| >= 1, exp >= 1 */
  n = MPFR_ABSSIZE(x);
  s = MPFR_MANT(x)[n - 1] >> (GMP_NUMB_BITS - exp);

  mpfr_clear (x);

  return MPFR_SIGN(f) * s;
}
