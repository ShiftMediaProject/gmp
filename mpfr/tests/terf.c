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

static void
test_generic (void)
{
  unsigned int prec, err, yprec, n, p0 = 2, p1 = 100, N = 10;
  mp_rnd_t rnd;
  mpfr_t x, y, z, t;

  mpfr_init2 (x, 2);
  mpfr_init2 (y, 2);
  mpfr_init2 (z, 2);
  mpfr_init2 (t, 2);

  for (prec = p0; prec <= p1; prec++)
    {
      mpfr_set_prec (x, prec);
      mpfr_set_prec (z, prec);
      mpfr_set_prec (t, prec);
      yprec = prec + 10;

      for (n=0; n<N; n++)
	{
	  mpfr_random (x);
          mpfr_mul_2exp (x, x, randlimb () % 4, GMP_RNDN);
	  rnd = randlimb () % 4;
	  mpfr_set_prec (y, yprec);
	  mpfr_erf (y, x, rnd);
	  err = (rnd == GMP_RNDN) ? yprec + 1 : yprec;
	  if (mpfr_can_round (y, err, rnd, rnd, prec))
	    {
	      mpfr_set (t, y, rnd);
	      mpfr_erf (z, x, rnd);
	      if (mpfr_cmp (t, z))
		{
		  printf ("results differ for x=");
		  mpfr_out_str (stdout, 2, prec, x, GMP_RNDN);
		  printf (" prec=%u rnd_mode=%s\n", prec,
			  mpfr_print_rnd_mode (rnd));
		  printf ("   got ");
		  mpfr_out_str (stdout, 2, prec, z, GMP_RNDN);
		  puts ("");
		  printf ("   expected ");
		  mpfr_out_str (stdout, 2, prec, t, GMP_RNDN);
		  puts ("");
		  printf ("   approximation was ");
		  mpfr_print_binary (y);
		  puts ("");
		  exit (1);
		}
	    }
	}
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);
}

int
main (int argc, char *argv[])
{
  mpfr_t x, y;

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

  mpfr_clear (x);
  mpfr_clear (y);

  test_generic ();

  tests_end_mpfr ();
  return 0;
}
