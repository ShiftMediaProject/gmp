/* Test file for mpfr_arctan.

Copyright 2001, 2002, 2003 Free Software Foundation.
Written by Paul Zimmermann, INRIA Lorraine.

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

static void
worst_cases (void)
{
  mpfr_t x, y, z;

  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_init2 (z, 53);

  mpfr_set_str_binary (x, "1.0000100110000001100111100011001110101110100111011101");
  mpfr_set_str_binary (y, "1.1001101101110100101100110011011101101000011010111110e-1");
  mpfr_atan (z, x, GMP_RNDN);
  if (mpfr_cmp (y, z))
    {
      printf ("Error in mpfr_atan for prec=53, rnd=GMP_RNDN\n");
      printf ("x=");
      mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
      printf ("\nexpected ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\ngot      ");
      mpfr_out_str (stdout, 2, 0, z, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_set_inf (x, -1);
  mpfr_atan (y, x, GMP_RNDN);
  if (mpfr_sgn (y) >= 0)
    {
      printf ("Error: mpfr_atan (-inf) should be negative, got ");
      mpfr_print_binary (y);
      printf ("\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
}

#define TEST_FUNCTION mpfr_atan
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  unsigned int p0 = 2, p1 = 100, N = 7;

  tests_start_mpfr ();

  worst_cases ();

  /* tarctan prec - perform one random computation with precision prec */
  if (argc >= 2)
    {
      p0 = p1 = atoi (argv[1]);
      N = 1;
    }

  test_generic (p0, p1, N);

  tests_end_mpfr ();
  return 0;
}
