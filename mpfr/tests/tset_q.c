/* Test file for mpfr_set_q.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-test.h"

void check _PROTO((long int, long int, mp_rnd_t, double)); 

void
check (long int n, long int d, mp_rnd_t rnd, double y)
{
  mpq_t q;
  mpfr_t x, t;
  double z;
  int inexact, compare;

  mpfr_init2 (x, 53);
  mpfr_init2 (t, mpfr_get_prec (x) + mp_bits_per_limb);
  mpq_init (q);
  mpq_set_si (q, n, d);
#ifdef MPFR_HAVE_FESETROUND
  mpfr_set_machine_rnd_mode (rnd);
  y = (double) n / d;
#endif
  inexact = mpfr_set_q (x, q, rnd);
  z = mpfr_get_d1 (x);

  /* check values */
  if (y != z)
    {
    fprintf (stderr, "Error for q=%ld/%lu and rnd=%s\n", n, d, 
	     mpfr_print_rnd_mode (rnd));
    fprintf (stderr, "libm.a gives %1.20e, mpfr_set_q gives %1.20e\n", y, z);
    exit (1);
    }

  /* check inexact flag */
  if (mpfr_mul_ui (t, x, (d < 0) ? (-d) : d, rnd))
    {
      fprintf (stderr, "t <- x * d should be exact\n");
      exit (1);
    }
  compare = mpfr_cmp_si (t, n);
  if (((inexact == 0) && (compare != 0)) ||
      ((inexact < 0) && (compare >= 0)) ||
      ((inexact > 0) && (compare <= 0)))
    {
      fprintf (stderr, "wrong inexact flag: expected %d, got %d\n", compare,
	       inexact);
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (t);
  mpq_clear (q);
}

int
main (void)
{
#ifdef MPFR_HAVE_FESETROUND
  long int i, n;
  unsigned long int d;
  double y;
  unsigned char rnd;

  mpfr_test_init ();

  SEED_RAND(time(NULL));
  for (i=0;i<1000000;i++) {
    n = LONG_RAND();
    d = LONG_RAND();
    if (LONG_RAND()%2) n = -n;
    rnd = LONG_RAND() % 4;
    y = (double) n / d;
    check(n, d, rnd, y);
  }
#endif
  check(-1647229822, 40619231, GMP_RNDZ, -4.055295438754120596e1);
  check(-148939696, 1673285490, GMP_RNDZ, -8.9010331404953485501e-2);
  check(-441322590, 273662545, GMP_RNDZ, -1.6126525096812205362);
  check(-1631156895, 1677687197, GMP_RNDU, -9.722652100563177191e-1);
  check(2141332571, 3117601, GMP_RNDZ, 6.8685267004982347316e2);
  check(75504803, 400207282, GMP_RNDU, 1.8866424074712365155e-1);
  check(643562308, 23100894, GMP_RNDD, 2.7858762002890447462e1);
  check(632549085, 1831935802, GMP_RNDN, 3.4528998467600230393e-1);

  return 0;
}
