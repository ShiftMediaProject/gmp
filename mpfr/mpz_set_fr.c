/* mpz_set_fr -- set a multiple-precision integer from a floating-point number

Copyright (C) 2000, 2001 Free Software Foundation, Inc.

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
#include "mpfr.h"
#include "mpfr-impl.h"
#include "gmp-impl.h"
#include "longlong.h"

/* puts the mantissa of f into z, and returns 'exp' such that f = z * 2^exp
 */
mp_exp_t
mpz_set_fr (mpz_ptr z, mpfr_srcptr f)
{
  int fn, sh;

  if (MPFR_IS_NAN(f) || MPFR_IS_INF(f))
    {
      fprintf (stderr, "Error in mpz_set_fr: input is NaN or Inf\n");
      exit (1);
    }

  fn = 1 + (MPFR_PREC(f) - 1) / BITS_PER_MP_LIMB;

  /* check whether allocated space for z is enough */
  if (ALLOC(z) < fn)
    MPZ_REALLOC(z, fn);

  sh = fn * BITS_PER_MP_LIMB - MPFR_PREC(f);
  if (sh)
    mpn_rshift (PTR(z), MPFR_MANT(f), fn, sh);
  else
    MPN_COPY (PTR(z), MPFR_MANT(f), fn);

  SIZ(z) = fn;

  return MPFR_EXP(f) - MPFR_PREC(f);
}
