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
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

static void
test1 (void)
{
  mpfr_t x, y;

  mpfr_init2 (x, 32);
  mpfr_init2 (y, 42);
  mpfr_set_str_binary (x, "1.1111111101000111011010010010100e-1");
  mpfr_zeta (y, x, GMP_RNDN); /* shouldn't crash */
  mpfr_clear (x);
  mpfr_clear (y);
}

#define TEST_FUNCTION mpfr_zeta
#include "tgeneric.c"

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
      printf ("Usage: tzeta\n"
              "    or tzeta s prec rnd_mode\n");
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
  mpfr_set_str_binary (s, "1.10010e4");
  mpfr_zeta (z, s, GMP_RNDZ);


  mpfr_set_prec (s, 53);
  mpfr_set_prec (y, 53);
  mpfr_set_prec (z, 53);

  mpfr_set_ui (s, 1, GMP_RNDN);
  mpfr_zeta (z, s, GMP_RNDN);
  if (!mpfr_inf_p (z) || MPFR_SIGN (z) < 0)
    {
      printf ("Error in mpfr_zeta for s = 1 (should be +inf)\n");
      exit (1);
    }

  mpfr_set_str_binary (s, "0.1100011101110111111111111010000110010111001011001011");
  mpfr_set_str_binary (y, "-0.11111101111011001001001111111000101010000100000100100E2");
  mpfr_zeta (z, s, GMP_RNDN);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (1,RNDN)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDZ);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (1,RNDZ)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDU);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (1,RNDU)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDD);
  mpfr_add_one_ulp (y, GMP_RNDD);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (1,RNDD)\n");
      exit (1);
    }

  mpfr_set_str_binary (s, "0.10001011010011100110010001100100001011000010011001011");
  mpfr_set_str_binary (y, "-0.11010011010010101101110111011010011101111101111010110E1");
  mpfr_zeta (z, s, GMP_RNDN);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (2,RNDN)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDZ);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (2,RNDZ)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDU);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (2,RNDU)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDD);
  mpfr_add_one_ulp (y, GMP_RNDD);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (2,RNDD)\n");
      exit (1);
    }

  mpfr_set_str_binary (s, "0.1100111110100001111110111000110101111001011101000101");
  mpfr_set_str_binary (y, "-0.10010111010110000111011111001101100001111011000001010E3");
  mpfr_zeta (z, s, GMP_RNDN);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (3,RNDN)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDD);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (3,RNDD)\n");
      exit (1);
    }
  mpfr_sub_one_ulp (y, GMP_RNDZ);
  mpfr_zeta (z, s, GMP_RNDZ);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (3,RNDZ)\n");
      exit (1);
    }
  mpfr_zeta (z, s, GMP_RNDU);
  if (mpfr_cmp (z, y) != 0)
    {
      printf ("Error in mpfr_zeta (3,RNDU)\n");
      exit (1);
    }

  test_generic (2, 70, 1);

  mpfr_clear (s);
  mpfr_clear (y);
  mpfr_clear (z);

  tests_end_mpfr ();
  return 0;
}
