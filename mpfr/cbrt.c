/* mpfr_cbrt -- cube root function.

Copyright 2002, 2003 Free Software Foundation.
Contributed by the Spaces project, INRIA Lorraine.

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
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

 /* The computation of y = x^(1/3) is done as follows:

    Let x = sign * m * 2^(3*e) where m is an integer

    with 2^(3n-3) <= m < 2^(3n) where n = PREC(y)

    and m = s^3 + r where 0 <= r and m < (s+1)^3

    we want that s has n bits i.e. s >= 2^(n-1), or m >= 2^(3n-3)
    i.e. m must have at least 3n-2 bits

    then x^(1/3) = s * 2^e if r=0
         x^(1/3) = (s+1) * 2^e if round up
         x^(1/3) = (s-1) * 2^e if round down
         x^(1/3) = s * 2^e if nearest and r < 3/2*s^2+3/4*s+1/8
                   (s+1) * 2^e otherwise
 */

int
mpfr_cbrt (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
{

  mpz_t m;
  mp_exp_t e, r, sh;
  mp_prec_t n, size_m;
  int inexact, sign_x;

  /* special values */
  if (MPFR_IS_NAN(x))
    {
      MPFR_SET_NAN(y);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(y);

  if (MPFR_IS_INF(x))
    {
      MPFR_SET_INF(y);
      MPFR_SET_SAME_SIGN (y, x);
      return 0;
    }

  MPFR_CLEAR_INF(y);

  /* case 0: cbrt(+/- 0) = +/- 0 */
  if (MPFR_IS_ZERO(x))
    {
      MPFR_SET_ZERO(y);
      MPFR_SET_SAME_SIGN (y, x);
      return 0;
    }

  sign_x = MPFR_SIGN(x);

  mpz_init (m);

  e = mpfr_get_z_exp (m, x); /* x = m * 2^e */
  if (sign_x < 0)
    mpz_neg (m, m);
  r = e % 3;
  if (r < 0)
    r += 3;
  /* x = (m*2^r) * 2^(e-r) = (m*2^r) * 2^(3*q) */

  size_m = mpz_sizeinbase (m, 2);
  n = MPFR_PREC(y);
  if (rnd_mode == GMP_RNDN)
    n ++;
  
  /* we want 3*n-2 <= size_m + 3*sh + r <= 3*n
     i.e. 3*sh + size_m + r <= 3*n */
  sh = (3 * n - size_m - r) / 3;
  sh = 3 * sh + r;
  if (sh >= 0)
    {
      mpz_mul_2exp (m, m, sh);
      e = e - sh;
    }

  /* invariant: x = m*2^e */

  /* we reuse the variable m to store the cube root, since it is not needed
     any more: we just need to know if the root is exact */
  inexact = mpz_root (m, m, 3) == 0;

  sh = mpz_sizeinbase (m, 2) - n;
  if (sh > 0) /* we have to flush to 0 the last sh bits from m */
    {
      inexact = inexact || (mpz_scan1 (m, 0) < sh);
      mpz_div_2exp (m, m, sh);
      e += 3 * sh;
    }

  if (inexact)
    {
      if ((rnd_mode == GMP_RNDU) ||
          ((rnd_mode == GMP_RNDN) && mpz_tstbit (m, 0)))
        mpz_add_ui (m, m, inexact = 1);
      else
        inexact = -1;
    }

  /* either inexact is not zero, and the conversion is exact, i.e. inexact
     is not changed; or inexact=0, and inexact is set only when
     rnd_mode=GMP_RNDN and bit (n+1) from m is 1 */
  inexact += mpfr_set_z (y, m, GMP_RNDN);
  MPFR_SET_EXP (y, MPFR_GET_EXP (y) + e / 3);

  if (sign_x < 0)
    {
      mpfr_neg (y, y, GMP_RNDN);
      inexact = -inexact;
    }

  mpz_clear (m);

  return inexact;
}
