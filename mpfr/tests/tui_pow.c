/* Test file for mpfr_ui_pow.

Copyright 2001, 2002, 2003 Free Software Foundation.
Adapted from tarctan.c.

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
#include <limits.h>
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
  mpfr_init2 (y, 65);
  mpfr_set_str_binary (x, "-0.101110001001011011011e-9");
  mpfr_ui_pow (y, 7, x, GMP_RNDN);
  mpfr_clear (x);
  mpfr_clear (y);
}

static void
check1 (mpfr_ptr x, mp_prec_t prec, unsigned long nt, mp_rnd_t rnd)
{
  mpfr_t y, z, t;
  int inexact, compare, compare2;
  mp_prec_t yprec;
  mp_exp_t err;

  yprec = prec + 10;

  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (t);
  mpfr_set_prec (y, yprec);
  mpfr_set_prec (z, prec);
  mpfr_set_prec (t, prec);

  compare = mpfr_ui_pow (y, nt, x, rnd);
  err = (rnd == GMP_RNDN) ? yprec + 1 : yprec;
  if (mpfr_can_round (y, err, rnd, rnd, prec))
    {
      mpfr_set (t, y, rnd);
      inexact = mpfr_ui_pow (z, nt, x, rnd);
      if (mpfr_cmp (t, z))
        {
          printf ("results differ for x=");
          mpfr_out_str (stdout, 2, prec, x, GMP_RNDN);
          printf (" n=%lu", nt);
          printf (" prec=%u rnd_mode=%s\n", (unsigned) prec,
                  mpfr_print_rnd_mode (rnd));
          printf ("got      ");
          mpfr_out_str (stdout, 2, prec, z, GMP_RNDN);
          puts ("");
          printf ("expected ");
          mpfr_out_str (stdout, 2, prec, t, GMP_RNDN);
          puts ("");
          printf ("approx  ");
          mpfr_print_binary (y);
          puts ("");
          exit (1);
        }
      compare2 = mpfr_cmp (t, y);
      /* if rounding to nearest, cannot know the sign of t - f(x)
         because of composed rounding: y = o(f(x)) and t = o(y) */
      if ((rnd != GMP_RNDN) && (compare * compare2 >= 0))
        compare = compare + compare2;
      else
        compare = inexact; /* cannot determine sign(t-f(x)) */
      if (((inexact == 0) && (compare != 0)) ||
          ((inexact > 0) && (compare <= 0)) ||
          ((inexact < 0) && (compare >= 0)))
        {
          printf ("Wrong inexact flag for rnd=%s: expected %d, got %d\n",
                  mpfr_print_rnd_mode (rnd), compare, inexact);
          printf ("x="); mpfr_print_binary (x); puts ("");
          printf ("y="); mpfr_print_binary (y); puts ("");
          printf ("t="); mpfr_print_binary (t); puts ("");
          exit (1);
        }
    }

  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);
}

int
main (int argc, char *argv[])
{
  mpfr_t x, y;
  unsigned long int n;

  tests_start_mpfr ();

  mpfr_init (x);
  mpfr_init (y);

  n = randlimb ();

  MPFR_CLEAR_NAN(x);
  MPFR_SET_INF(x);
  mpfr_ui_pow (y, n, x, GMP_RNDN);
  if(!MPFR_IS_INF(y))
    {
      printf ("evaluation of function in INF does not return INF\n");
      exit (1);
    }

  MPFR_CHANGE_SIGN(x);
  mpfr_ui_pow (y, n,x, GMP_RNDN);
  if(!MPFR_IS_ZERO(y))
    {
      printf ("evaluation of function in -INF does not return 0");
      exit (1);
    }

  MPFR_SET_NAN(x);
  mpfr_ui_pow (y, n,x, GMP_RNDN);
  if(!MPFR_IS_NAN(y))
    {
      printf ("evaluation of function in NAN does not return NAN");
      exit (1);
    }

  test1 ();

  {
  mpfr_t z, t;
  mp_prec_t prec;
  mp_rnd_t rnd;
  unsigned int n;

  int p0=2;
  int p1=100;
  int N=20;

  mpfr_init2 (z, 38);
  mpfr_init2 (t, 6);

  /* check exact power */
  mpfr_set_str_binary (t, "0.110000E5");
  mpfr_ui_pow (z, 3, t, GMP_RNDN);

  mpfr_clear (z);
  mpfr_clear (t);

  mpfr_set_prec (x, 2);
  mpfr_set_d (x, 0.5, GMP_RNDN);
  check1 (x, 2, 398441521, GMP_RNDN);  /* 398441521 = 19961^2 */

  /* generic test */
  for (prec = p0; prec <= p1; prec++)
    {
      mpfr_set_prec (x, prec);
      for (n=0; n<N; n++)
	{
          int nt;
          nt = randlimb () & INT_MAX;
	  mpfr_random (x);
	  rnd = randlimb () % 4;
          check1 (x, prec, nt, rnd);
	}
    }
  }

  mpfr_clear (x);
  mpfr_clear (y);

  tests_end_mpfr ();
  return 0;
}
