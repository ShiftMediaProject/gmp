/* Test file for mpfr_acosh.

Copyright 2001, 2002, 2003 Free Software Foundation.
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

#define TEST_FUNCTION mpfr_acosh
#define RAND_FUNCTION(x) (mpfr_random (x), mpfr_ui_div (x, 1, x, GMP_RNDN))
#include "tgeneric.c"

static
void check_inf(void)
{
  mpfr_t x,y;
  mpfr_init(x);
  mpfr_init(y);
  MPFR_SET_INF(x);
  mpfr_set_ui(y, 1, GMP_RNDN);
  mpfr_acosh(x, y, GMP_RNDN);
  if (MPFR_IS_INF(x) || MPFR_IS_NAN(x) )
    {
      printf("Inf flag not clears in acosh!\n");
      exit(1);
    }
  MPFR_SET_NAN(x);
  mpfr_acosh(x, y, GMP_RNDN);
  if (MPFR_IS_NAN(x) || MPFR_IS_INF(x) )
    {
      printf("NAN flag not clears in acosh!\n");
      exit(1);
    }
  mpfr_clear (x);
  mpfr_clear (y);
}

int
main (int argc, char *argv[])
{
  tests_start_mpfr ();

  test_generic (2, 100, 25);
  check_inf ();

  tests_end_mpfr ();
  return 0;
}
