/* Test file for mpfr_trunc, mpfr_ceil, mpfr_floor.

Copyright 1999, 2000, 2001 Free Software Foundation, Inc.

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
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#define SIZEX 100

int
main (void)
{
  int j, k; mpfr_t x, y, z, t, y2, z2, t2; 

  mpfr_init2(x, SIZEX); 
  mpfr_init2(y, SIZEX); 
  mpfr_init2(z, SIZEX); 
  mpfr_init2(t, SIZEX); 
  mpfr_init2(y2, SIZEX); 
  mpfr_init2(z2, SIZEX); 
  mpfr_init2(t2, SIZEX);

  mpfr_set_d(x, 0.5, GMP_RNDN);
  mpfr_ceil(y, x);
  if (mpfr_get_d1 (y) != 1.0) {
    fprintf(stderr, "Error in mpfr_ceil for x=0.5: expected 1.0, got %f\n",
	    mpfr_get_d1 (y)); exit(1); 
  }

  mpfr_set_d(x, 0.0, GMP_RNDN);
  mpfr_ceil(y, x);
  if (mpfr_get_d1 (y) != 0.0) {
    fprintf(stderr, "Error in mpfr_ceil for x=0.0: expected 0.0, got %f\n",
	    mpfr_get_d1 (y)); exit(1); 
  }

  mpfr_set_d(x, 1.0, GMP_RNDN);
  mpfr_ceil(y, x);
  if (mpfr_get_d1 (y) != 1.0) {
    fprintf(stderr, "Error in mpfr_ceil for x=1.0: expected 1.0, got %f\n",
	    mpfr_get_d1 (y)); exit(1); 
  }

  for (j=0;j<1000;j++) {

    mpfr_random(x); 
    MPFR_EXP(x) = 2;
        
    for (k = 2; k <= SIZEX; k++) 
      {
	mpfr_set_prec(y, k); 
	mpfr_set_prec(y2, k); 
	mpfr_set_prec(z, k); 
	mpfr_set_prec(z2, k); 
	mpfr_set_prec(t, k); 
	mpfr_set_prec(t2, k);  

	mpfr_floor(y, x); 
	mpfr_set(y2, x, GMP_RNDD); 

	mpfr_trunc(z, x); 
	mpfr_set(z2, x, GMP_RNDZ); 

	mpfr_ceil(t, x); 
	mpfr_set(t2, x, GMP_RNDU); 

	if (!mpfr_eq(y, y2, k))
	  {
	    printf("Error in floor, x = "); mpfr_print_binary(x); printf("\n"); 
	    printf("floor(x) = "); mpfr_print_binary(y); printf("\n"); 
	    printf("round(x, RNDD) = "); mpfr_print_binary(y2); printf("\n"); 
	    mpfr_clear(x); 
	    mpfr_clear(y); 
	    mpfr_clear(y2); 
	    mpfr_clear(z); 
	    mpfr_clear(z2); 
	    mpfr_clear(t); 
	    mpfr_clear(t2); 
	    exit(-1); 
	  }

	if (!mpfr_eq(z, z2, k))
	  {
	    printf("Error in trunc, x = "); mpfr_print_binary(x); printf("\n"); 
	    printf("trunc(x) = "); mpfr_print_binary(z); printf("\n"); 
	    printf("round(x, RNDZ) = "); mpfr_print_binary(z2); printf("\n"); 
	    mpfr_clear(x); 
	    mpfr_clear(y); 
	    mpfr_clear(y2); 
	    mpfr_clear(z); 
	    mpfr_clear(z2); 
	    mpfr_clear(t); 
	    mpfr_clear(t2); 
	    exit(-1); 
	  }

	if (!mpfr_eq(y, y2, k))
	  {
	    printf("Error in ceil, x = "); mpfr_print_binary(x); printf("\n"); 
	    printf("ceil(x) = "); mpfr_print_binary(t); printf("\n"); 
	    printf("round(x, RNDU) = "); mpfr_print_binary(t2); printf("\n"); 
	    mpfr_clear(x); 
	    mpfr_clear(y); 
	    mpfr_clear(y2); 
	    mpfr_clear(z); 
	    mpfr_clear(z2); 
	    mpfr_clear(t); 
	    mpfr_clear(t2); 
	    exit(-1); 
	  }
	MPFR_EXP(x)++; 
      }
  }

  mpfr_clear(x); 
  mpfr_clear(y); 
  mpfr_clear(y2); 
  mpfr_clear(z); 
  mpfr_clear(z2); 
  mpfr_clear(t); 
  mpfr_clear(t2); 

  return 0;
}
