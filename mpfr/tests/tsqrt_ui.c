/* Test file for mpfr_sqrt_ui.

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
along with the MPFR Library; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-test.h"

void check _PROTO((unsigned long, mp_rnd_t, double)); 

int maxulp=0;

void
check (unsigned long a, mp_rnd_t rnd_mode, double Q)
{
  mpfr_t q; double Q2; int u, ck;

  mpfr_init2(q, 53);
#ifdef HAVE_FESETROUND
  mpfr_set_machine_rnd_mode(rnd_mode);
#endif
  mpfr_sqrt_ui(q, a, rnd_mode);
  ck = (Q >= 0.0);
  if (!ck) Q = sqrt(1.0 * a);
  Q2 = mpfr_get_d1 (q);
  if (Q!=Q2 && (!isnan(Q) || !isnan(Q2))) {
    u = ulp(Q2,Q);
    if (ck) printf("mpfr_sqrt_ui failed");
    else printf("mpfr_sqrt_ui differs from sqrt");
    printf(" for a=%lu, rnd_mode=%s\n",
	   a, mpfr_print_rnd_mode(rnd_mode));
    printf("sqrt gives %1.20e, mpfr_sqrt_ui gives %1.20e (%d ulp)\n",Q,Q2,u);
    exit(1);
  }
  mpfr_clear(q);
}

int
main (void)
{
#ifdef HAVE_FESETROUND
  int i;
  unsigned long a;

  mpfr_test_init ();

  SEED_RAND (time(NULL));
  for (i=0;i<1000000;i++)
    {
      a = LONG_RAND();
      /* machine arithmetic must agree if a <= 2.0^53 */
      if (1.0*a < 9007199254872064.0)
        check(a, LONG_RAND() % 4, -1.0);
    }
#endif
  check (0, GMP_RNDN, 0.0);
  check (2116118, GMP_RNDU, 1.45468828276026215e3);

  return 0;
}
