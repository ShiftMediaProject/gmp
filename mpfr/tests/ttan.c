/* Test file for mpfr_tan.

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
check53 (double x, double tan_x, mp_rnd_t rnd_mode)
{
  mpfr_t xx, s;

  mpfr_init2 (xx, 53);
  mpfr_init2 (s, 53);
  mpfr_set_d (xx, x, rnd_mode); /* should be exact */
  mpfr_tan (s, xx, rnd_mode);
  if (mpfr_get_d1 (s) != tan_x && (!isnan(tan_x) || !mpfr_nan_p(s))) {
    fprintf (stderr, "mpfr_tan failed for x=%1.20e, rnd=%s\n", x,
	     mpfr_print_rnd_mode (rnd_mode));
    fprintf (stderr, "mpfr_tan gives tan(x)=%1.20e, expected %1.20e\n",
	     mpfr_get_d1 (s), tan_x);
    exit(1);
  }
  mpfr_clear (xx);
  mpfr_clear (s);
}

#define TEST_FUNCTION mpfr_tan
#include "tgeneric.c"

int
main(int argc, char *argv[])
{
  mpfr_t x;
  unsigned int i;
  unsigned int prec[10] = {14, 15, 19, 22, 23, 24, 25, 40, 41, 52};
  unsigned int prec2[10] = {4, 5, 6, 19, 70, 95, 100, 106, 107, 108};

#ifdef HAVE_INFS
  check53 (DBL_NAN, DBL_NAN, GMP_RNDN);
  check53 (DBL_POS_INF, DBL_NAN, GMP_RNDN);
  check53 (DBL_NEG_INF, DBL_NAN, GMP_RNDN);
#endif

  mpfr_init (x);

  mpfr_set_prec (x, 2);
  mpfr_set_d (x, 0.5, GMP_RNDN);
  mpfr_tan (x, x, GMP_RNDD);
  if (mpfr_get_d1 (x) != 0.5)
    {
      fprintf (stderr, "mpfr_tan(0.5, GMP_RNDD) failed\n");
      fprintf (stderr, "expected 0.5, got %f\n", mpfr_get_d1 (x));
      exit (1);
    }

  /* check that tan(3*Pi/4) ~ -1 */
  for (i=0; i<10; i++)
    {
      mpfr_set_prec (x, prec[i]);
      mpfr_const_pi (x, GMP_RNDN);
      mpfr_mul_ui (x, x, 3, GMP_RNDN);
      mpfr_div_ui (x, x, 4, GMP_RNDN);
      mpfr_tan (x, x, GMP_RNDN);
      if (mpfr_cmp_si (x, -1))
        {
          fprintf (stderr, "tan(3*Pi/4) fails for prec=%u\n", prec[i]);
          exit (1);
        }
    }

  /* check that tan(7*Pi/4) ~ -1 */
  for (i=0; i<10; i++)
    {
      mpfr_set_prec (x, prec2[i]);
      mpfr_const_pi (x, GMP_RNDN);
      mpfr_mul_ui (x, x, 7, GMP_RNDN);
      mpfr_div_ui (x, x, 4, GMP_RNDN);
      mpfr_tan (x, x, GMP_RNDN);
      if (mpfr_cmp_si (x, -1))
        {
          fprintf (stderr, "tan(3*Pi/4) fails for prec=%u\n", prec2[i]);
          exit (1);
        }
    }

  mpfr_clear (x);

  test_generic (2, 100, 100);

  return 0;
}
