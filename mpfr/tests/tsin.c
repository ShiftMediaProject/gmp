/* Test file for mpfr_sin.

Copyright 2001, 2002 Free Software Foundation, Inc.

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
#include <math.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

void check53 _PROTO ((double, double, mp_rnd_t));

void
check53 (double x, double sin_x, mp_rnd_t rnd_mode)
{
  mpfr_t xx, s;

  mpfr_init2 (xx, 53);
  mpfr_init2 (s, 53);
  mpfr_set_d (xx, x, rnd_mode); /* should be exact */
  mpfr_sin (s, xx, rnd_mode);
  if (mpfr_get_d1 (s) != sin_x && (!isnan(sin_x) || !mpfr_nan_p(s)))
    {
      fprintf (stderr, "mpfr_sin failed for x=%1.20e, rnd=%s\n", x,
	       mpfr_print_rnd_mode (rnd_mode));
      fprintf (stderr, "mpfr_sin gives sin(x)=%1.20e, expected %1.20e\n",
	       mpfr_get_d1 (s), sin_x);
      exit(1);
    }
  mpfr_clear (xx);
  mpfr_clear (s);
}

#define TEST_FUNCTION mpfr_sin
#include "tgeneric.c"

int
main (int argc, char *argv[])
{
  mpfr_t x;

#ifdef HAVE_INFS
  check53 (DBL_NAN, DBL_NAN, GMP_RNDN);
  check53 (DBL_POS_INF, DBL_NAN, GMP_RNDN);
  check53 (DBL_NEG_INF, DBL_NAN, GMP_RNDN);
#endif
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
      fprintf (stderr, "mpfr_sin(0.5, GMP_RNDD) failed with precision=2\n");
      exit (1);
    }

  /* bug found by Kevin Ryde */
  mpfr_const_pi (x, GMP_RNDN);
  mpfr_mul_ui (x, x, 3L, GMP_RNDN);
  mpfr_div_ui (x, x, 2L, GMP_RNDN);
  mpfr_sin (x, x, GMP_RNDN);
  if (mpfr_cmp_ui (x, 0) >= 0)
    {
      fprintf (stderr, "Error: wrong sign for sin(3*Pi/2)\n");
      exit (1);
    }

  mpfr_clear (x);

  test_generic (2, 100, 80);

  return 0;
}
