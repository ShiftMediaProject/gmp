/* Test file for mpfr_sin.

Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "mpfr-impl.h"
#include "mpfr-test.h"

static void
check53 (double x, double sin_x, mp_rnd_t rnd_mode)
{
  mpfr_t xx, s;

  mpfr_init2 (xx, 53);
  mpfr_init2 (s, 53);
  mpfr_set_d (xx, x, rnd_mode); /* should be exact */
  mpfr_sin (s, xx, rnd_mode);
  if (mpfr_get_d1 (s) != sin_x && !(Isnan(sin_x) && mpfr_nan_p(s)))
    {
      printf ("mpfr_sin failed for x=%1.20e, rnd=%s\n",
              x, mpfr_print_rnd_mode (rnd_mode));
      printf ("mpfr_sin gives sin(x)=%1.20e, expected %1.20e\n",
              mpfr_get_d1 (s), sin_x);
      exit(1);
    }
  mpfr_clear (xx);
  mpfr_clear (s);
}

static void
test_sign (void)
{
  mpfr_t pid, piu, x, y;
  int p, k;

  mpfr_init2 (pid, 4096);
  mpfr_const_pi (pid, GMP_RNDD);
  mpfr_init2 (piu, 4096);
  mpfr_const_pi (piu, GMP_RNDU);
  mpfr_init (x);
  mpfr_init2 (y, 2);
  for (p = 8; p <= 128; p++)
    for (k = 2; k <= 6; k += 2)
      {
        mpfr_set_prec (x, p);
        mpfr_mul_ui (x, pid, k, GMP_RNDD);
        mpfr_sin (y, x, GMP_RNDN);
        if (MPFR_SIGN(y) > 0)
          {
            printf ("Error in test_sign for sin(%dpi-epsilon), prec = %d"
                    " for argument.\nResult should have been negative.\n",
                    k, p);
            exit (1);
          }
        mpfr_mul_ui (x, piu, k, GMP_RNDU);
        mpfr_sin (y, x, GMP_RNDN);
        if (MPFR_SIGN(y) < 0)
          {
            printf ("Error in test_sign for sin(%dpi+epsilon), prec = %d"
                    " for argument.\nResult should have been positive.\n",
                    k, p);
            exit (1);
          }
      }
  mpfr_clear (pid);
  mpfr_clear (piu);
  mpfr_clear (x);
  mpfr_clear (y);
}

static void
check_nans (void)
{
  mpfr_t  x, y;

  mpfr_init2 (x, 123L);
  mpfr_init2 (y, 123L);

  mpfr_set_nan (x);
  mpfr_sin (y, x, GMP_RNDN);
  if (! mpfr_nan_p (y))
    {
      printf ("Error: sin(NaN) != NaN\n");
      exit (1);
    }

  mpfr_set_inf (x, 1);
  mpfr_sin (y, x, GMP_RNDN);
  if (! mpfr_nan_p (y))
    {
      printf ("Error: sin(Inf) != NaN\n");
      exit (1);
    }

  mpfr_set_inf (x, -1);
  mpfr_sin (y, x, GMP_RNDN);
  if (! mpfr_nan_p (y))
    {
      printf ("Error: sin(-Inf) != NaN\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);
}

#define TEST_FUNCTION mpfr_sin
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  mpfr_t x;

  tests_start_mpfr ();

  check_nans ();

  /* worst case from PhD thesis of Vincent Lefe`vre: x=8980155785351021/2^54 */
  check53 (4.984987858808754279e-1, 4.781075595393330379e-1, GMP_RNDN);
  check53 (4.984987858808754279e-1, 4.781075595393329824e-1, GMP_RNDD);
  check53 (4.984987858808754279e-1, 4.781075595393329824e-1, GMP_RNDZ);
  check53 (4.984987858808754279e-1, 4.781075595393330379e-1, GMP_RNDU);
  check53 (1.00031274099908640274,  8.416399183372403892e-1, GMP_RNDN);
  check53 (1.00229256850978698523,  8.427074524447979442e-1, GMP_RNDZ);
  check53 (1.00288304857059840103,  8.430252033025980029e-1, GMP_RNDZ);
  check53 (1.00591265847407274059,  8.446508805292128885e-1, GMP_RNDN);

  check53 (1.00591265847407274059,  8.446508805292128885e-1, GMP_RNDN);

  mpfr_init2 (x, 2);

  mpfr_set_d (x, 0.5, GMP_RNDN);
  mpfr_sin (x, x, GMP_RNDD);
  if (mpfr_get_d1 (x) != 0.375)
    {
      printf ("mpfr_sin(0.5, GMP_RNDD) failed with precision=2\n");
      exit (1);
    }

  /* bug found by Kevin Ryde */
  mpfr_const_pi (x, GMP_RNDN);
  mpfr_mul_ui (x, x, 3L, GMP_RNDN);
  mpfr_div_ui (x, x, 2L, GMP_RNDN);
  mpfr_sin (x, x, GMP_RNDN);
  if (mpfr_cmp_ui (x, 0) >= 0)
    {
      printf ("Error: wrong sign for sin(3*Pi/2)\n");
      exit (1);
    }

  mpfr_clear (x);

  test_generic (2, 100, 20);
  test_sign ();

  tests_end_mpfr ();
  return 0;
}
