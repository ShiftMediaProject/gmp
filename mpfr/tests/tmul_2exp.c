/* Test file for mpfr_mul_2exp.

Copyright (C) 1999, 2001 Free Software Foundation.

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
#include "mpfr-impl.h"

/* checks that x*y gives the same results in double
   and with mpfr with 53 bits of precision */

int
main (int argc, char *argv[])
{
  double x, z; mpfr_t w; unsigned long k; 

  mpfr_init2(w, 53); 

  mpfr_set_d(w, 1.0/0.0, 0); 
  mpfr_mul_2exp(w, w, 10, GMP_RNDZ); 
  if (!MPFR_IS_INF(w)) { fprintf(stderr, "Inf != Inf"); exit(-1); }
  
  mpfr_set_d(w, 0.0/0.0, 0); 
  mpfr_mul_2exp(w, w, 10, GMP_RNDZ); 
  if (!MPFR_IS_NAN(w)) { fprintf(stderr, "NaN != NaN"); exit(-1); }

  for (k = 0; k < 100000; k++) {
    srand48(time(NULL)); 
    x = drand48(); 
    mpfr_set_d(w, x, 0);
    mpfr_mul_2exp(w, w, 10, GMP_RNDZ); 
    if (x != (z = mpfr_get_d(w)/1024))
      {
	fprintf(stderr, "%f != %f\n", x, z); 
	return -1;
      }

    mpfr_set_d(w, x, 0);
    mpfr_div_2exp(w, w, 10, GMP_RNDZ);
    if (x != (z = mpfr_get_d(w)*1024))
      {
	fprintf(stderr, "%f != %f\n", x, z);
	mpfr_clear(w);
	return -1;
      }
  }

  mpfr_clear(w);

  return 0;
}
