/* Test file for mpfr_asinh.

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
#include "mpfr.h"
#include "mpfr-test.h"

#define TEST_FUNCTION mpfr_asinh
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  mpfr_t x, y;

  mpfr_init (x);
  mpfr_init (y);

  mpfr_set_prec (x, 33);
  mpfr_set_prec (y, 43);
  mpfr_set_str_raw (x, "0.111001101100000110011001010000101");
  mpfr_asinh (y, x, GMP_RNDZ);

  test_generic (2, 100, 25);

  mpfr_clear (x);
  mpfr_clear (y);

  return 0;
}
