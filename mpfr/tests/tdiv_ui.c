/* Test file for mpfr_div_ui.

Copyright (C) 1999 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-test.h"

void check _PROTO((double, unsigned long, mp_rnd_t, double)); 

void check (double d, unsigned long u, mp_rnd_t rnd, double e)
{
  mpfr_t x, y; double f;

  mpfr_init2(x, 53); mpfr_init2(y, 53);
#ifdef TEST
  mpfr_set_machine_rnd_mode(rnd);
#endif
  if (e==0.0) e = d / u;
  mpfr_set_d(x, d, rnd); 
  mpfr_div_ui(y, x, u, rnd); 
  f = mpfr_get_d(y);
  if (f != e && (!isnan(f) || !isnan(e))) {
    printf("mpfr_div_ui failed for x=%1.20e, u=%lu, rnd=%s\n", d, u,
	   mpfr_print_rnd_mode(rnd));
    printf("expected result is %1.20e, got %1.20e, dif=%d ulp\n",e,f,ulp(e,f));
    exit(1);
  }
  mpfr_clear(x); mpfr_clear(y); 
}

int
main(int argc, char **argv)
{
  mpfr_t x;
#ifdef TEST
  int i; unsigned long u; double d;

  srand(getpid());
  for (i=0;i<1000000;i++) {
    do { u = lrand48(); } while (u==0);
    do { d = drand(); } while (fabs(d/u)<2.2e-307);
    check(d, u, rand() % 4, 0.0);
  }
#endif

  check(1.0, 3, GMP_RNDN, 3.3333333333333331483e-1);
  check(1.0, 3, GMP_RNDZ, 3.3333333333333331483e-1);
  check(1.0, 3, GMP_RNDU, 3.3333333333333337034e-1);
  check(1.0, 3, GMP_RNDD, 3.3333333333333331483e-1);
  check(1.0, 2116118, GMP_RNDN, 4.7256343927890600483e-7);
  check(1.098612288668109782, 5, GMP_RNDN, 0.21972245773362195087);

  mpfr_init2(x, 100);
  mpfr_set_prec(x, 53);
  mpfr_set_ui(x, 3, GMP_RNDD);
  mpfr_log(x, x, GMP_RNDD);
  mpfr_div_ui(x, x, 5, GMP_RNDD);
  if (mpfr_get_d(x) != 0.21972245773362189536) {
    fprintf(stderr, "Error in mpfr_div_ui for x=ln(3), u=5\n"); exit(1);
  }
  mpfr_clear(x);

  return 0;
}
