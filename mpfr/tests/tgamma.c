/* mpfr_tgamma -- test file for gamma function

Copyright 2001, 2002, 2003 Free Software Foundation.

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

#include "mpfr-test.h"

int mpfr_gamma (mpfr_ptr, mpfr_srcptr, mp_rnd_t);

#define TEST_FUNCTION mpfr_gamma
#include "tgeneric.c"

int
main (void)
{
  mpfr_t x, y;

  tests_start_mpfr ();

  mpfr_init (x);
  mpfr_init (y);

  mpfr_set_prec (x, 53);
  mpfr_set_prec (y, 53);

#define CHECK_X1 "1.0762904832837976166"
#define CHECK_Y1 0.96134843256452096050

  mpfr_set_str (x, CHECK_X1, 10, GMP_RNDN);
  mpfr_gamma (y, x, GMP_RNDN);
  if (mpfr_get_d (y, GMP_RNDN) != CHECK_Y1 )
    {
      printf ("mpfr_gamma("CHECK_X1") is wrong: expected %1.20e, got %1.20e\n",
              CHECK_Y1, mpfr_get_d (y, GMP_RNDN));
      exit (1);
    }

#define CHECK_X2 "9.23709516716202383435e-01"
#define CHECK_Y2 1.0502315560291053398
  mpfr_set_str (x, CHECK_X2, 10, GMP_RNDN);
  mpfr_gamma (y, x, GMP_RNDN);
  if (mpfr_get_d (y, GMP_RNDN) != CHECK_Y2)
    {
      printf ("mpfr_gamma("CHECK_X2") is wrong: expected %1.20e, got %1.20e\n",
              CHECK_Y2, mpfr_get_d (y, GMP_RNDN));
      exit (1);
    }

  test_generic (2, 100, 2);

  mpfr_clear (x);
  mpfr_clear (y);

  tests_end_mpfr ();
  return 0;
}
