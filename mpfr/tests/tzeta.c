/* tzeta -- test file for the Riemann Zeta function

Copyright 2003 Free Software Foundation.
Contributed by Jean-Luc Re'my and the Spaces project, INRIA Lorraine.

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

#define PREC_MAX 80
#define N 2

static void
test1 (void)
{
  mpfr_t x, y;

  mpfr_init2 (x, 32);
  mpfr_init2 (y, 42);
  mpfr_set_str_raw (x, "1.1111111101000111011010010010100e-1");
  mpfr_zeta (y, x, GMP_RNDN); /* shouldn't crash */
  mpfr_clear (x);
  mpfr_clear (y);
}

static void
test_generic (void)
{
  mp_prec_t prec, yprec;
  int n, err;
  mp_rnd_t rnd;
  mpfr_t x, y, z;
  mp_exp_t e;

  mpfr_init2 (x, MPFR_PREC_MIN);
  mpfr_init2 (y, MPFR_PREC_MIN);
  mpfr_init2 (z, MPFR_PREC_MIN);

  for (prec = 2; prec <= PREC_MAX; prec++)
    {
      mpfr_set_prec (x, prec);
      mpfr_set_prec (z, prec);
      yprec = prec + 10;

      for (n = 0; n < N; n++)
	{
	  mpfr_random (x); /* x is in [0, 1[ */
	  mpfr_add_ui (x, x, 1, GMP_RNDN);
	  e = randlimb () % 5;
	  mpfr_div_2exp (x, x, 1, GMP_RNDN); /* now in [1/2, 1[ */
	  mpfr_mul_2exp (x, x, e, GMP_RNDN); /* now in [2^(e-1), 2^e[ */
          if (randlimb () % 2)
            mpfr_ui_sub (x, 1, x, GMP_RNDN); /* now less or equal to 1/2 */
	  rnd = randlimb () % 4;
	  mpfr_set_prec (y, yprec);
	  mpfr_zeta (y, x, rnd);
	  err = (rnd == GMP_RNDN) ? yprec + 1 : yprec;
	  if (mpfr_can_round (y, err, rnd, rnd, prec))
	    {
	      mpfr_prec_round (y, prec, rnd);
	      mpfr_zeta (z, x, rnd);
	      if (mpfr_cmp (y, z))
		{
		  printf ("results differ for x=");
		  mpfr_out_str (stdout, 2, prec, x, GMP_RNDN);
		  printf (" prec=%lu rnd_mode=%s\n", prec,
			  mpfr_print_rnd_mode (rnd));
		  printf ("   got ");
		  mpfr_out_str (stdout, 2, prec, z, GMP_RNDN);
		  puts ("");
		  printf ("   expected ");
		  mpfr_out_str (stdout, 2, prec, y, GMP_RNDN);
		  puts ("");
		  exit (1);
		}
	    }
	}
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
}

/* Usage: tzeta - generic tests
          tzeta s prec rnd_mode - compute zeta(s) with precision 'prec'
                                  and rounding mode 'mode' */
int
main (int argc, char *argv[])
{
  mpfr_t s, y, z;
  mp_prec_t prec;
  mp_rnd_t rnd_mode;

  tests_start_mpfr ();

  if (argc != 1 && argc != 4)
    {
      fprintf (stderr, "Usage: tzeta\n");
      fprintf (stderr, "    or tzeta s prec rnd_mode\n");
      exit (1);
    }

  if (argc == 4)
    {
      prec = atoi(argv[2]);
      mpfr_init2 (s, prec);
      mpfr_init2 (z, prec);
      mpfr_set_str (s, argv[1], 10, GMP_RNDN);
      rnd_mode = atoi(argv[3]);

      mpfr_zeta (z, s, rnd_mode);
      mpfr_out_str (stdout, 10, 0, z, GMP_RNDN);
      printf ("\n");

      mpfr_clear (s);
      mpfr_clear (z);

      return 0;
    }

  test1();

  mpfr_init2 (s, MPFR_PREC_MIN);
  mpfr_init2 (y, MPFR_PREC_MIN);
  mpfr_init2 (z, MPFR_PREC_MIN);


  /* the following seems to loop */
  mpfr_set_prec (s, 6);
  mpfr_set_prec (z, 6);
  mpfr_set_str_raw (s, "1.10010e4");
  mpfr_zeta (z, s, GMP_RNDZ);


  mpfr_set_prec (s, 53);
  mpfr_set_prec (y, 53);
  mpfr_set_prec (z, 53);

  mpfr_set_ui (s, 1, GMP_RNDN);
  mpfr_zeta (z, s, GMP_RNDN);
  if (!mpfr_inf_p (z) || MPFR_SIGN (z) < 0)
    {
      fprintf (stderr, "Error in mpfr_zeta for s = 1 (should be +inf)\n");
      exit (1);
    }

  mpfr_set_str_raw (s, "0.1100011101110111111111111010000110010111001011001011");
  mpfr_set_str_raw (y, "-0.11111101111011001001001111111000101010000100000100100E2");
  mpfr_zeta (z, s, GMP_RNDN);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (1,RNDN)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDZ);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (1,RNDZ)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDU);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (1,RNDU)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDD);
  mpfr_add_one_ulp (y, GMP_RNDD);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (1,RNDD)\n");
      exit (1);
    }

  mpfr_set_str_raw (s, "0.10001011010011100110010001100100001011000010011001011");
  mpfr_set_str_raw (y, "-0.11010011010010101101110111011010011101111101111010110E1");
  mpfr_zeta (z, s, GMP_RNDN);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (2,RNDN)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDZ);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (2,RNDZ)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDU);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (2,RNDU)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDD);
  mpfr_add_one_ulp (y, GMP_RNDD);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (2,RNDD)\n");
      exit (1);
    }

  mpfr_set_str_raw (s, "0.1100111110100001111110111000110101111001011101000101");
  mpfr_set_str_raw (y, "-0.10010111010110000111011111001101100001111011000001010E3");
  mpfr_zeta (z, s, GMP_RNDN);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (3,RNDN)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDD);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (3,RNDD)\n");
      exit (1);
    }
  mpfr_sub_one_ulp (y, GMP_RNDZ);
  mpfr_zeta (z, s, GMP_RNDZ);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (3,RNDZ)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDU);
  if (mpfr_cmp (z, y) != 0)
    {
      fprintf (stderr, "Error in mpfr_zeta (3,RNDU)\n");
      exit (1);
    }

  test_generic ();

  mpfr_clear (s);
  mpfr_clear (y);
  mpfr_clear (z);

  tests_end_mpfr ();
  return 0;
}
