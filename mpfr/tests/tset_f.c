/* Test file for mpfr_set_f.

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-test.h"

int
main (void)
{
  mpfr_t x, u;
  mpf_t y, z;
  unsigned long k, pr;
  
  mpf_init(y); 
  mpf_init(z);

  mpf_set_d (y, 0.0);

  /* check prototype of mpfr_init_set_f */
  mpfr_init_set_f (x, y, GMP_RNDN);
  mpfr_set_prec (x, 100);
  mpfr_set_f (x, y, GMP_RNDN);

  SEED_RAND (time(NULL));
  mpf_random2(y, 10, 0); 
  mpfr_set_f(x, y, LONG_RAND() & 3);

  /* bug found by Jean-Pierre Merlet */
  mpfr_set_prec(x, 256);
  mpf_set_prec(y, 256);
  mpfr_init2(u, 256);
  mpfr_set_str(u,
     "7.f10872b020c49ba5e353f7ced916872b020c49ba5e353f7ced916872b020c498@2",
     16, GMP_RNDN);
  mpf_set_str(y, "2033.033", 10);
  mpfr_set_f(x, y, GMP_RNDN);
  if (mpfr_cmp(x, u)) {
    fprintf(stderr, "mpfr_set_f failed for y=2033.033\n"); exit(1);
  }
  mpf_set_str(y, "-2033.033", 10);
  mpfr_set_f(x, y, GMP_RNDN);
  mpfr_neg(u, u, GMP_RNDN);
  if (mpfr_cmp(x, u)) {
    fprintf(stderr, "mpfr_set_f failed for y=-2033.033\n"); exit(1);
  }

  mpfr_clear(u);
  mpfr_clear(x);

  for (k = 1; k <= 100000; k++)
    {
      pr = 2 + (LONG_RAND()&255);
      mpf_set_prec (z, pr);
      mpf_random2 (z, z->_mp_prec, 0);
      mpfr_init2 (x, pr);
      mpfr_set_f (x, z, 0);
      mpfr_clear (x);
    }
  mpf_clear (y);
  mpf_clear (z);

  return 0;
}
