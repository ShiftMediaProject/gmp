/* Test file for mpfr_get_str.

Copyright 1999, 2001, 2002 Free Software Foundation, Inc.

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#ifdef MPFR_HAVE_FESETROUND
#include "mpfr-test.h"
#endif

void check _PROTO((double, mp_rnd_t)); 
void check3 _PROTO((double, mp_rnd_t, char *)); 
void check_small _PROTO((void)); 

void
check (double d, mp_rnd_t rnd)
{
  mpfr_t x; char *str; mp_exp_t e;

  mpfr_init2(x, 53);
  mpfr_set_d(x, d, rnd);
  str = mpfr_get_str(NULL, &e, 10, 5, x, rnd);
  mpfr_clear(x);
  free(str);
}

void
check3 (double d, mp_rnd_t rnd, char *res)
{
  mpfr_t x; char *str; mp_exp_t e;

  mpfr_init2 (x, 53);
  mpfr_set_d (x, d, rnd);
  str = mpfr_get_str (NULL, &e, 10, 5, x, rnd);
  if (strcmp(str, res))
    {
      fprintf (stderr, "Error in mpfr_get_str for x=%1.20e\n", d);
      fprintf (stderr, "got %s instead of %s\n", str, res);
    }
  mpfr_clear (x);
  free (str);
}

void
check_small (void)
{
  mpfr_t x;
  char *s;
  mp_exp_t e;
  
  mpfr_init(x);

  /* problem found by Fabrice Rouillier */
  mpfr_set_prec(x, 63);
  mpfr_set_d(x, 5e14, GMP_RNDN);
  s = mpfr_get_str(NULL, &e, 10, 18, x, GMP_RNDU);
  free(s);

  /* bug found by Johan Vervloet */
  mpfr_set_prec(x, 6);
  mpfr_set_d(x, 688.0, GMP_RNDN);
  s = mpfr_get_str(NULL, &e, 2, 4, x, GMP_RNDU);
  if (strcmp(s, "1011") || (e!=10)) {
    fprintf(stderr, "Error in mpfr_get_str: 688 printed up to 4 bits should give 1.011e9\ninstead of ");
    mpfr_out_str(stderr, 2, 4, x, GMP_RNDU); putchar('\n');
    exit(1);
  }
  free(s);

  mpfr_set_prec (x, 38);
  mpfr_set_str_raw (x, "1.0001110111110100011010100010010100110e-6");
  s = mpfr_get_str (NULL, &e, 8, 10, x, GMP_RNDU);
  if (strcmp (s, "1073721522") || (e != -1))
    {
      fprintf (stderr, "Error in mpfr_get_str (3): s=%s e=%d\n", s, (int) e);
      exit (1);
    }
  free (s);

  mpfr_clear(x);
}

int
main (int argc, char *argv[])
{
#ifdef MPFR_HAVE_FESETROUND
  int i;
  double d;

  SEED_RAND (time(NULL));
  for (i=0;i<100000;i++) {
    do { d = drand(); } while (isnan(d));
    check(d, GMP_RNDN);
  }
#endif
  check_small();
  check3(4.059650008e-83, GMP_RNDN, "40597");
  check3(-6.606499965302424244461355e233, GMP_RNDN, "-66065");
  check3(-7.4, GMP_RNDN, "-74000");
  check3(0.997, GMP_RNDN, "99700");
  check3(-4.53063926135729747564e-308, GMP_RNDN, "-45306");
  check3(2.14478198760196000000e+16, GMP_RNDN, "21448");
  check3(7.02293374921793516813e-84, GMP_RNDN, "70229");
  check3(-6.7274500420134077e-87, GMP_RNDN, "-67275"); 

  return 0;
}
