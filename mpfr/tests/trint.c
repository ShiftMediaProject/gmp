/* Test file for mpfr_trunc, mpfr_floor, mpfr_ceil, mpfr_round.

Copyright 2002 Free Software Foundation.

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

int
main (void)
{
  mp_size_t s;
  mpz_t z;
  mp_prec_t p;
  mpfr_t x, y, t;
  mp_rnd_t r;
  int inexact, sign_t;

  mpfr_init (x);
  mpfr_init (y);
  mpz_init (z);
  mpfr_init (t);
  mpz_set_ui (z, 1);
  for (s = 2; s < 100; s++)
    {
      /* z has exactly s bits */
      
      mpz_mul_2exp (z, z, 1);
      if (LONG_RAND () % 2)
        mpz_add_ui (z, z, 1);
      mpfr_set_prec (x, s);
      mpfr_set_prec (t, s);
      if (mpfr_set_z (x, z, GMP_RNDN))
        {
          fprintf (stderr, "Error: mpfr_set_z should be exact (s = %u)\n",
                   (unsigned int) s);
          exit (1);
        }
      for (p=2; p<100; p++)
        {
          mpfr_set_prec (y, p);
          for (r=0; r<4; r++)
            {
              if (r == GMP_RNDN)
                inexact = mpfr_round (y, x);
              else if (r == GMP_RNDZ)
                inexact = mpfr_trunc (y, x);
              else if (r == GMP_RNDU)
                inexact = mpfr_ceil (y, x);
              else /* r = GMP_RNDD */
                inexact = mpfr_floor (y, x);
              if (mpfr_sub (t, y, x, GMP_RNDN))
                {
                  fprintf (stderr, "Error: subtraction should be exact\n");
                  exit (1);
                }
              sign_t = mpfr_cmp_ui (t, 0);
              if (((inexact == 0) && (sign_t != 0)) ||
                  ((inexact < 0) && (sign_t >= 0)) ||
                  ((inexact > 0) && (sign_t <= 0)))
                {
                  fprintf (stderr, "Wrong inexact flag\n");
                  exit (1);
                }
            }
        }
    }
  mpfr_clear (x);
  mpfr_clear (y);
  mpz_clear (z);
  mpfr_clear (t);

  return 0;
}
