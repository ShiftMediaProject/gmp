/* Test file for mpfr_acos.

Copyright 2001, 2002, 2003 Free Software Foundation.
Contributed by Mathieu Dutour.

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

#define TEST_FUNCTION mpfr_acos
#include "tgeneric.c"

int
main (void)
{
  mpfr_t x, y;

  tests_start_mpfr ();

  mpfr_init (x);
  mpfr_init (y);

  MPFR_SET_NAN(x);
  mpfr_acos (y, x, GMP_RNDN);
  if (mpfr_nan_p(y) == 0)
    {
      printf ("Error: acos(NaN) != NaN\n");
      exit (1);
    }

  mpfr_set_ui (x, 2, GMP_RNDN);
  mpfr_acos (y, x, GMP_RNDN);
  if (mpfr_nan_p(y) == 0)
    {
      printf ("Error: acos(2) != NaN\n");
      exit (1);
    }

  test_generic (2, 100, 7);

  mpfr_clear (x);
  mpfr_clear (y);

  tests_end_mpfr ();
  return 0;
}
