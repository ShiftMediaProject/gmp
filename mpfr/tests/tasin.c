/* Test file for mpfr_asin.

Copyright 2001, 2002, 2003 Free Software Foundation.
Original version by Mathieu Dutour.

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
#include "mpfr.h"
#include "mpfr-test.h"

#define TEST_FUNCTION mpfr_asin
#include "tgeneric.c"

int
main (void)
{
  mpfr_t x, y, z;

  tests_start_mpfr ();

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);

  /* check that sin(-1) = -Pi/2 */
  mpfr_set_si (x, -1, GMP_RNDN);
  mpfr_asin (y, x, GMP_RNDN);
  mpfr_const_pi (z, GMP_RNDN);
  mpfr_div_2exp (z, z, 1, GMP_RNDN);
  mpfr_neg (z, z, GMP_RNDN);
  if (mpfr_cmp (y, z))
    {
      printf ("sin(-1) is wrong, expected %.20e, got %.20e\n",
              mpfr_get_d1 (z), mpfr_get_d1 (y));
      exit (1);
    }

  test_generic (2, 100, 7);

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);

  tests_end_mpfr ();

  return 0;
}
