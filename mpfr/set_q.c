/* mpfr_set_q -- set a floating-point number from a multiple-precision rational

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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
  mp_prec_t prec;

  MPFR_CLEAR_FLAGS (f);
  num = mpq_numref (q);
  if (mpz_cmp_ui (num, 0) == 0)
    {
      MPFR_SET_ZERO (f);
      MPFR_SET_POS (f);
      MPFR_RET (0);
    }

  den = mpq_denref (q);
  mpfr_save_emin_emax ();
  prec = mpz_sizeinbase (num, 2);
  if (prec < MPFR_PREC_MIN)
    prec = MPFR_PREC_MIN;
  mpfr_init2 (n, prec);
  if (mpfr_set_z (n, num, GMP_RNDZ)) /* result is exact unless overflow */
    {
      mpfr_clear (n);
      mpfr_restore_emin_emax ();
      MPFR_SET_NAN (f);
      MPFR_RET_NAN;
    }
  prec = mpz_sizeinbase(den, 2);
  if (prec < MPFR_PREC_MIN)
    prec = MPFR_PREC_MIN;
  mpfr_init2 (d, prec);
  if (mpfr_set_z (d, den, GMP_RNDZ)) /* result is exact unless overflow */
    {
      mpfr_clear (d);
      mpfr_clear (n);
      mpfr_restore_emin_emax ();
      MPFR_SET_NAN (f);
      MPFR_RET_NAN;
    }
  inexact = mpfr_div (f, n, d, rnd);
  mpfr_clear (n);
  mpfr_clear (d);
  MPFR_RESTORE_RET (inexact, f, rnd);
}
