/* Test file for mpfr_get_d

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation.

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

static int
check_denorms ()
{
  mp_rnd_t rnd_mode;
  mpfr_t x;
  double d, d2, dd, f, dbl_min;
  int fail = 0, k, n;

  /* workaround for gcc bug on m68040-unknown-netbsd1.4.1,
     where DBL_MIN gives (1-2^(-52))/2^1022 */
  dbl_min = 1.0;
  for (d = DBL_MIN; d < 0.9; d *= 2.0)
    dbl_min /= 2.0;

  mpfr_init2 (x, BITS_PER_MP_LIMB);

  rnd_mode = GMP_RNDN;
  for (k = -17; k <= 17; k += 2)
    {
      d = (double) k * dbl_min; /* k * 2^(-1022) */
      f = 1.0;
      mpfr_set_si (x, k, GMP_RNDN);
      mpfr_div_2exp (x, x, 1022, GMP_RNDN); /* k * 2^(-1022) */
      for (n = 0; n <= 58; n++)
        {
          d2 = d * f;
          dd = mpfr_get_d (x, rnd_mode);
          if (d2 != dd) /* should be k * 2^(-1022-n) for n < 53 */
            {
              printf ("Wrong result for %d * 2^(%d), rnd_mode %d\n",
                      k, -1022-n, rnd_mode);
              printf ("got %.20e instead of %.20e\n", dd, d2);
              fail = 1;
            }
          f *= 0.5;
          mpfr_div_2exp (x, x, 1, GMP_RNDN);
        }
    }

  mpfr_clear (x);
  return fail;
}

static void
check_inf_nan ()
{
  /* only if nans and infs are available */
#if _GMP_IEEE_FLOATS
  mpfr_t  x;
  double  d;

  mpfr_init2 (x, 123);

  mpfr_set_inf (x, 1);
  d = mpfr_get_d (x, GMP_RNDZ);
  ASSERT_ALWAYS (d > 0);
  ASSERT_ALWAYS (DOUBLE_ISINF (d));

  mpfr_set_inf (x, -1);
  d = mpfr_get_d (x, GMP_RNDZ);
  ASSERT_ALWAYS (d < 0);
  ASSERT_ALWAYS (DOUBLE_ISINF (d));

  mpfr_set_nan (x);
  d = mpfr_get_d (x, GMP_RNDZ);
  ASSERT_ALWAYS (DOUBLE_ISNAN (d));

  mpfr_clear (x);
#endif
}

int
main (void)
{
  tests_start_mpfr ();
  mpfr_test_init ();

  if (check_denorms ())
    exit (1);

  check_inf_nan ();

  tests_end_mpfr ();
  return 0;
}
