/* Test file for mpfr_sub_ui

Copyright 2000, 2001, 2002, 2003 Free Software Foundation.

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
#include <time.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

#define check(x,y,r) check3(x,y,r,0.0)

/* checks that x+y gives the same results in double
   and with mpfr with 53 bits of precision */
static void
check3 (double x, unsigned long y, mp_rnd_t rnd_mode, double z1)
{
  double z2;
  mpfr_t xx,zz;

  mpfr_init (xx);
  mpfr_init (zz);
  mpfr_set_prec (xx, 53);
  mpfr_set_prec (zz, 53);
  mpfr_set_d (xx, x, rnd_mode);
  mpfr_sub_ui (zz, xx, y, rnd_mode);
  z2 = mpfr_get_d1 (zz);
  if (z1!=z2 && !(Isnan(z1) && Isnan(z2)))
    {
      printf ("expected sum is %1.20e, got %1.20e\n",z1,z2);
      printf ("mpfr_sub_ui failed for x=%1.20e y=%lu with rnd_mode=%s\n",
              x, y, mpfr_print_rnd_mode (rnd_mode));
      exit (1);
    }
  mpfr_clear (xx);
  mpfr_clear (zz);
}

/* FastTwoSum: if EXP(x) >= EXP(y), u = o(x+y), v = o(u-x), w = o(y-v),
               then x + y = u + w
thus if u = o(y-x), v = o(u+x), w = o(v-y), then y-x = u-w */
static void
check_two_sum (mp_prec_t p)
{
  unsigned int x;
  mpfr_t y, u, v, w;
  mp_rnd_t rnd;
  int inexact;
  
  mpfr_init2 (y, p);
  mpfr_init2 (u, p);
  mpfr_init2 (v, p);
  mpfr_init2 (w, p);
  do
    {
      x = LONG_RAND ();
    }
  while (x < 1);
  mpfr_random (y);
  rnd = LONG_RAND() % 4;
  rnd = GMP_RNDN;
  inexact = mpfr_sub_ui (u, y, x, GMP_RNDN);
  mpfr_add_ui (v, u, x, GMP_RNDN);
  mpfr_sub (w, v, y, GMP_RNDN);
  /* as u - (y-x) = w, we should have inexact and w of same sign */
  if (((inexact == 0) && mpfr_cmp_ui (w, 0)) ||
      ((inexact > 0) && (mpfr_cmp_ui (w, 0) <= 0)) ||
      ((inexact < 0) && (mpfr_cmp_ui (w, 0) >= 0)))
    {
      fprintf (stderr, "Wrong inexact flag for prec=%u, rnd=%s\n", (unsigned)p,
	       mpfr_print_rnd_mode (rnd));
      printf ("x=%u\n", x);
      printf ("y="); mpfr_print_binary(y); putchar('\n');
      printf ("u="); mpfr_print_binary(u); putchar('\n');
      printf ("v="); mpfr_print_binary(v); putchar('\n');
      printf ("w="); mpfr_print_binary(w); putchar('\n');
      printf ("inexact = %d\n", inexact);
      exit (1);
    }
  mpfr_clear (y);
  mpfr_clear (u);
  mpfr_clear (v);
  mpfr_clear (w);
}

static void
check_nans (void)
{
  mpfr_t  x, y;

  mpfr_init2 (x, 123L);
  mpfr_init2 (y, 123L);

  /* nan - 1 == nan */
  mpfr_set_nan (x);
  mpfr_sub_ui (y, x, 1L, GMP_RNDN);
  ASSERT_ALWAYS (mpfr_nan_p (y));

  /* +inf - 1 == +inf */
  mpfr_set_inf (x, 1);
  mpfr_sub_ui (y, x, 1L, GMP_RNDN);
  ASSERT_ALWAYS (mpfr_inf_p (y));
  ASSERT_ALWAYS (mpfr_sgn (y) > 0);

  /* -inf - 1 == -inf */
  mpfr_set_inf (x, -1);
  mpfr_sub_ui (y, x, 1L, GMP_RNDN);
  ASSERT_ALWAYS (mpfr_inf_p (y));
  ASSERT_ALWAYS (mpfr_sgn (y) < 0);

  mpfr_clear (x);
  mpfr_clear (y);
}

int
main (int argc, char *argv[])
{
  mp_prec_t p;
  int k;

  tests_start_mpfr ();

  check_nans ();

  for (p=2; p<200; p++)
    for (k=0; k<200; k++)
      check_two_sum (p);

  check3 (0.9999999999, 1, GMP_RNDN, -56295.0 / 562949953421312.0);

  tests_end_mpfr ();
  return 0;
}
