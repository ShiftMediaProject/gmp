/* Test file for mpfr_erf.

Copyright 2001, 2002, 2003 Free Software Foundation, Inc.
Contributed by Ludovic Meunier and Paul Zimmermann.

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

#define TEST_FUNCTION mpfr_erf
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  mpfr_t x, y;
  int inex;

  tests_start_mpfr ();

  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);

  mpfr_set_nan (x);
  mpfr_erf (y, x, GMP_RNDN);
  if (mpfr_nan_p (y) == 0)
    {
      printf ("mpfr_erf failed for x=NaN\n");
      exit (1);
    }

  mpfr_set_inf (x, 1);
  mpfr_erf (y, x, GMP_RNDN);
  if (mpfr_cmp_ui (y, 1))
    {
      printf ("mpfr_erf failed for x=+Inf\n");
      printf ("expected 1.0, got ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_set_inf (x, -1);
  mpfr_erf (y, x, GMP_RNDN);
  if (mpfr_cmp_si (y, -1))
    {
      printf ("mpfr_erf failed for x=-Inf\n");
      exit (1);
    }

  mpfr_set_ui (x, 0, GMP_RNDN); /* x = +0 */
  mpfr_erf (y, x, GMP_RNDN);
  if (mpfr_cmp_ui (y, 0) || MPFR_SIGN(y) < 0)
    {
      printf ("mpfr_erf failed for x=+0\n");
      exit (1);
    }

  mpfr_neg (x, x, GMP_RNDN); /* x = -0 */
  mpfr_erf (y, x, GMP_RNDN);
  if (mpfr_cmp_ui (y, 0) || MPFR_SIGN(y) > 0)
    {
      printf ("mpfr_erf failed for x=-0\n");
      exit (1);
    }

  mpfr_set_d (x, 1.0, GMP_RNDN);
  mpfr_erf (x, x, GMP_RNDN);
  mpfr_set_str_binary (y, "0.11010111101110110011110100111010000010000100010001011");
  if (mpfr_cmp (x, y))
    {
      printf ("mpfr_erf failed for x=1.0, rnd=GMP_RNDN\n");
      printf ("expected ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\n");
      printf ("got      ");
      mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_set_d (x, 6.6, GMP_RNDN);
  mpfr_erf (x, x, GMP_RNDN);
  if (mpfr_cmp_ui (x, 1))
    {
      printf ("mpfr_erf failed for x=6.6, rnd=GMP_RNDN\n");
      printf ("expected 1\n");
      printf ("got      ");
      mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_set_d (x, 6.6, GMP_RNDN);
  mpfr_erf (x, x, GMP_RNDZ);
  mpfr_set_str_binary (y, "0.11111111111111111111111111111111111111111111111111111");
  if (mpfr_cmp (x, y))
    {
      printf ("mpfr_erf failed for x=6.6, rnd=GMP_RNDZ\n");
      printf ("expected ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\n");
      printf ("got      ");
      mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_set_d (x, 4.5, GMP_RNDN);
  mpfr_erf (x, x, GMP_RNDN);
  mpfr_set_str_binary (y, "0.1111111111111111111111111111111100100111110100011");
  if (mpfr_cmp (x, y))
    {
      printf ("mpfr_erf failed for x=4.5, rnd=GMP_RNDN\n");
      printf ("expected ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\n");
      printf ("got      ");
      mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_set_prec (x, 120);
  mpfr_set_prec (y, 120);
  mpfr_set_str_binary (x, "0.110100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011001100110011E3");
  mpfr_erf (x, x, GMP_RNDN);
  mpfr_set_str_binary (y, "0.11111111111111111111111111111111111111111111111111111111111111111100111111000100111011111011010000110101111100011001101");
  if (mpfr_cmp (x, y))
    {
      printf ("mpfr_erf failed for x=6.6, rnd=GMP_RNDN\n");
      printf ("expected ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\n");
      printf ("got      ");
      mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
      printf ("\n");
      exit (1);
    }

  mpfr_set_prec (x, 8);
  mpfr_set_prec (y, 8);
  mpfr_set_ui (x, 50, GMP_RNDN);
  inex = mpfr_erf (y, x, GMP_RNDN);
  if (mpfr_cmp_ui (y, 1))
    {
      printf ("mpfr_erf failed for x=50, rnd=GMP_RNDN\n");
      printf ("expected 1, got ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\n");
      exit (1);
    }
  if (inex <= 0)
    {
      printf ("mpfr_erf failed for x=50, rnd=GMP_RNDN: wrong ternary value\n"
              "expected positive, got %d\n", inex);
      exit (1);
    }
  inex = mpfr_erf (x, x, GMP_RNDZ);
  mpfr_nextbelow (y);
  if (mpfr_cmp (x, y))
    {
      printf ("mpfr_erf failed for x=50, rnd=GMP_RNDZ\n");
      printf ("expected ");
      mpfr_out_str (stdout, 2, 0, y, GMP_RNDN);
      printf ("\n");
      printf ("got      ");
      mpfr_out_str (stdout, 2, 0, x, GMP_RNDN);
      printf ("\n");
      exit (1);
    }
  if (inex >= 0)
    {
      printf ("mpfr_erf failed for x=50, rnd=GMP_RNDN: wrong ternary value\n"
              "expected negative, got %d\n", inex);
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);

  test_generic (2, 100, 10);

  tests_end_mpfr ();
  return 0;
}
