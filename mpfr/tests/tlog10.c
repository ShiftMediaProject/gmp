/* Test file for mpfr_log10.

Copyright 2001, 2002 Free Software Foundation.
Adapted from tsinh.c.

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

#define TEST_FUNCTION mpfr_log10
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  mpfr_t x, y;
  unsigned int n;

  test_generic (2, 100, 100);

  /* check log10(10^n)=n */
  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_set_ui (x, 1, GMP_RNDN);

  for (n=1; n<=15; n++)
    {
      mpfr_mul_ui (x, x, 10, GMP_RNDN); /* x = 10^n */
      mpfr_log10 (y, x, GMP_RNDN);
      if (mpfr_get_d1 (y) != (double) n)
        {
          fprintf (stderr, "log10(10^n) <> n for n=%u\n", n);
          exit (1);
        }
    }

  mpfr_clear (x);
  mpfr_clear (y);

  return 0;
}
