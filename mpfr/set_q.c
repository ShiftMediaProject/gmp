/* mpfr_set_q -- set a floating-point number from a multiple-precision rational

Copyright 2000, 2001 Free Software Foundation, Inc.

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
along with the MPFR Library; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* set f to the rational q */
int
mpfr_set_q (mpfr_ptr f, mpq_srcptr q, mp_rnd_t rnd)
{
  mpz_srcptr num, den;
  mpfr_t n, d;
  int inexact;

  MPFR_CLEAR_FLAGS(f);
  num = mpq_numref (q);
  if (mpz_cmp_ui (num, 0) == 0)
    {
      MPFR_SET_ZERO(f);
      return 0;
    }

  den = mpq_denref(q);
  mpfr_init2 (n, mpz_sizeinbase(num, 2));
  mpfr_set_z (n, num, GMP_RNDZ); /* result is exact */
  mpfr_init2 (d, mpz_sizeinbase(den, 2));
  mpfr_set_z (d, den, GMP_RNDZ); /* result is exact */
  inexact = mpfr_div (f, n, d, rnd);
  mpfr_clear (n);
  mpfr_clear (d);
  MPFR_RET(inexact);
}



