/* Test file for mpfr_get_d

Copyright 1999, 2000, 2001, 2002 Free Software Foundation.

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
#include <float.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

int check_denorms _PROTO ((void));

int
check_denorms ()
{
  mp_rnd_t rnd_mode;
  mpfr_t x;
  double d, d2, dd, f;
  int fail = 0, k, n;

  mpfr_init2 (x, BITS_PER_MP_LIMB);

      rnd_mode = GMP_RNDN;
      for (k = -17; k <= 17; k += 2)
        {
          d = k * DBL_MIN; /* k * 2^(-1022) */
          f = 1.0;
          mpfr_set_si (x, k, GMP_RNDN);
          mpfr_div_2exp (x, x, 1022, GMP_RNDN); /* k * 2^(-1022) */
          for (n = 0; n <= 58; n++)
            {
              d2 = d * f;
              dd = mpfr_get_d (x, rnd_mode);
              if (d2 != dd) /* should be k * 2^(-1022-n) for n < 53 */
                {
                  fprintf (stderr,
                           "Wrong result for %d * 2^(%d), rnd_mode %d\n",
                           k, -1022-n, rnd_mode);
                  fprintf (stderr, "got %.20e instead of %.20e\n", dd, d2);
                  fail = 1;
                }
              f *= 0.5;
              mpfr_div_2exp (x, x, 1, GMP_RNDN);
            }
        }

  mpfr_clear (x);
  return fail;
}

int
main (void)
{

  mpfr_test_init ();

  if (check_denorms ())
    exit (1);

  return 0;
}
