/* Test file for mpfr_can_round.

Copyright 1999, 2001, 2002, 2003 Free Software Foundation, Inc.

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

int
main (void)
{
  mpfr_t x;
  mp_prec_t i, j;

  tests_start_mpfr ();

  /* checks that rounds to nearest sets the last
     bit to zero in case of equal distance */
  mpfr_init2 (x, 59);
  mpfr_set_str_binary (x, "-0.10010001010111000011110010111010111110000000111101100111111E663"); 
  if (mpfr_can_round (x, 54, GMP_RNDZ, GMP_RNDZ, 53) != 0)
    {
      printf ("Error (1) in mpfr_can_round\n");
      exit (1);
    }

  mpfr_set_str_binary (x, "-Inf"); 
  if (mpfr_can_round (x, 2000, GMP_RNDZ, GMP_RNDZ, 2000) != 0)
    {
      printf ("Error (2) in mpfr_can_round\n");
      exit (1);
    }

  mpfr_set_prec (x, 64);
  mpfr_set_str_binary (x, "0.1011001000011110000110000110001111101011000010001110011000000000");
  if (mpfr_can_round (x, 65, GMP_RNDN, GMP_RNDN, 54))
    {
      printf ("Error (3) in mpfr_can_round\n");
      exit (1);
    }

  mpfr_set_prec (x, 62);
  mpfr_set_str (x, "0.ff4ca619c76ba69", 16, GMP_RNDZ);
  for (i = 30; i < 99; i++)
    for (j = 30; j < 99; j++)
      {
        mp_rnd_t r1, r2;
        for (r1 = 0; r1 < 4; r1++)
          for (r2 = 0; r2 < 4; r2++)
            mpfr_can_round (x, i, r1, r2, j); /* test for assertions */
      }

  mpfr_clear (x);

  tests_end_mpfr ();
  return 0;
}
