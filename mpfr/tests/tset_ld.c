/* Test file for mpfr_set_ld and mpfr_get_ld.

Copyright 2002, 2003 Free Software Foundation, Inc.

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
#include "mpfr.h"
#include "mpfr-test.h"

/* checks that a long double converted to a mpfr (with precision >=113),
   then converted back to a long double gives the initial value,
   or in other words mpfr_get_ld(mpfr_set_ld(d)) = d.
*/
static void
check_set_get (long double d, mpfr_t x)
{
  mp_rnd_t r;
  long double e;

  for (r=0; r<4; r++)
    {
      if (mpfr_set_ld (x, d, r))
        {
          fprintf (stderr, "Error: mpfr_set_ld should be exact\n");
          exit (1);
        }
      e = mpfr_get_ld (x, r);
      if (e != d && !(LONGDOUBLE_ISNAN(e) && LONGDOUBLE_ISNAN(d)))
        {
          fprintf (stderr, "Error: mpfr_get_ld o mpfr_set_ld <> Id\n");
          fprintf (stderr, "d=%1.30Le get_ld(set_ld(d))=%1.30Le\n", d, e);
          exit (1);
        }
    }
}

int
main (int argc, char *argv[])
{
  long double d, e;
  double f;
  mpfr_t x;
  int i;

  tests_start_mpfr ();
  mpfr_test_init ();
  tests_machine_prec_long_double ();

  mpfr_init2 (x, 113);

  /* check +0.0 and -0.0 */
  d = 0.0;
  check_set_get (d, x);
  d = -0.0;
  check_set_get (d, x);

  /* checks that sign of -0.0 is set */
  mpfr_set_ld (x, -0.0, GMP_RNDN);
  if (MPFR_SIGN(x) > 0)
    {
      fprintf (stderr, "Error: sign of -0.0 is not set correctly\n");
      exit (1);
    }

  /* checks NaN, Inf and -Inf */
  mpfr_set_nan (x);
  d = mpfr_get_ld (x, GMP_RNDN);
  check_set_get (d, x);

  mpfr_set_inf (x, 1);
  d = mpfr_get_ld (x, GMP_RNDN);
  check_set_get (d, x);

  mpfr_set_inf (x, -1);
  d = mpfr_get_ld (x, GMP_RNDN);
  check_set_get (d, x);

  /* checks the largest power of two */
  d = 1.0; while (d < LDBL_MAX / 2.0) d += d;
  check_set_get (d, x);
  check_set_get (-d, x);

  /* checks largest long double */
  d = LDBL_MAX;
  check_set_get (d, x);
  check_set_get (-d, x);

  /* checks the smallest power of two */
  d = 1.0; while ((e = d / 2.0) != (long double) 0.0) d = e;
  check_set_get (d, x);
  check_set_get (-d, x);

  /* checks 2^1024 */
  f = 1.3407807929942597100e155; /* 2^512 */
  d = (long double) f;
  d = d * d; /* 2^1024 */
  check_set_get (d, x);

  /* checks that 2^i, 2^i+1 and 2^i-1 are correctly converted */
  d = 1.0;
  for (i=1; i<=113; i++)
    {
      d = 2.0 * d; /* d = 2^i */
      check_set_get (d, x);
      check_set_get (d + 1.0, x);
      check_set_get (d - 1.0, x);
    }

  for (i=0; i<10000; i++)
    {
      mpfr_random (x);
      d = mpfr_get_ld (x, GMP_RNDN);
      check_set_get (d, x);
    }
     
  mpfr_clear (x);

  tests_end_mpfr ();

  return 0; 
}
