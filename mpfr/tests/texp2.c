/* Test file for mpfr_exp2.

Copyright 2001 Free Software Foundation.
Adapted from tarctan.c.

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
#include "mpfr-test.h"

#define TEST_FUNCTION mpfr_exp2
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  mpfr_t x, y;

  mpfr_init (x);
  mpfr_init (y);

  mpfr_set_prec (x, 53);
  mpfr_set_prec (y, 53);
  mpfr_set_d (x, -1683977482443233.0 / 2199023255552.0, GMP_RNDN);
  mpfr_exp2 (y, x, GMP_RNDN);
  if (mpfr_get_d1 (y) != 2.991959870867646566478e-231)
    {
      fprintf (stderr, "Error for x=-1683977482443233/2^41\n");
      exit (1);
    }

  MPFR_CLEAR_NAN(x);
  MPFR_SET_INF(x);
  MPFR_SET_POS(x);
  mpfr_exp2 (y, x, GMP_RNDN);
  if(!MPFR_IS_INF(y))
    {
      fprintf (stderr, "evaluation of function in INF does not return INF\n");
      exit (1);
    }

  MPFR_CHANGE_SIGN(x);
  mpfr_exp2 (y, x, GMP_RNDN);
  if(!MPFR_IS_ZERO(y))
    {
      fprintf (stderr, "evaluation of function in -INF does not return 0\n");
      exit (1);
    }

  MPFR_SET_NAN(x);
  mpfr_exp2 (y, x, GMP_RNDN);
  if(!MPFR_IS_NAN(y))
    {
      fprintf (stderr, "evaluation of function in NaN does not return NaN\n");
      exit (1);
    }

  test_generic (2, 100, 100);

  mpfr_clear (x);
  mpfr_clear (y);

  return 0;
}
