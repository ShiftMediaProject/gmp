/* Test file for mpfr_cmp_ui.

Copyright 1999, 2001 Free Software Foundation, Inc.

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

int
main (void)
{
  mpfr_t x; unsigned long i; long s;
  
  mpfr_init(x);

  mpfr_set_ui(x, 3, GMP_RNDZ);
  if (mpfr_cmp_ui(x, i=3)!=0) {    
    printf("Error in mpfr_cmp_ui(%1.20f,%lu)\n",mpfr_get_d1 (x), i); 
    mpfr_clear(x); 
    exit(1);
  }
  if (mpfr_cmp_ui(x, i=2)<=0) {
    printf("Error in mpfr_cmp_ui(%1.20f,%lu)\n",mpfr_get_d1 (x), i);
    mpfr_clear(x); 
    exit(1);
  }
  if (mpfr_cmp_ui(x, i=4)>=0) {
    printf("Error in mpfr_cmp_ui(%1.20f,%lu)\n",mpfr_get_d1 (x), i);
    mpfr_clear(x); 
    exit(1);
  }
  mpfr_set_ui (x, 0, GMP_RNDZ);
  mpfr_neg (x, x, GMP_RNDZ);
  if (mpfr_cmp_ui (x, i=0)) {
    printf("Error in mpfr_cmp_ui(%1.20f,%lu)\n",mpfr_get_d1 (x), i);
    mpfr_clear(x);
    exit(1);
  }

  mpfr_set_si(x, -3, GMP_RNDZ);
  if (mpfr_cmp_si(x, s=-3)!=0) {
    printf("Error in mpfr_cmp_si(%1.20f,%ld)\n",mpfr_get_d1 (x), s);
    mpfr_clear(x); 
    exit(1);
  }
  if (mpfr_cmp_si(x, s=-4)<=0) {
    printf("Error in mpfr_cmp_si(%1.20f,%ld)\n",mpfr_get_d1 (x), s);
    mpfr_clear(x); 
    exit(1);
  }
  if (mpfr_cmp_si(x, s=1)>=0) {
    printf("Error in mpfr_cmp_si(%1.20f,%ld)\n",mpfr_get_d1 (x), s);
    mpfr_clear(x); 
    exit(1);
  }

  mpfr_set_ui (x, 0, GMP_RNDN);
  mpfr_ui_div (x, 1, x, GMP_RNDU);
  if (mpfr_cmp_ui (x, 0) == 0)
    {
      fprintf (stderr, "Error in mpfr_cmp_ui (Inf, 0)\n");
      exit (1);
    }

  mpfr_clear(x); 

  return 0;
}
