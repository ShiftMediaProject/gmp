/* Test file for mpfr_mul_ui.

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

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

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "time.h"

int
main(int argc, char **argv)
{
  mpfr_t x, y;
  
  mpfr_init2(x, 53); mpfr_init2(y, 53);

  /* checks that result is normalized */
  mpfr_set_d(y, 6.93147180559945286227e-01, GMP_RNDZ);
  mpfr_mul_ui(x, y, 1, GMP_RNDZ);
  if (MANT(x)[PREC(x)/BITS_PER_MP_LIMB] >> (BITS_PER_MP_LIMB-1) == 0) {
    fprintf(stderr, "Error in mpfr_mul_ui: result not normalized\n");
    exit(1);
  }
  if (mpfr_cmp(x,y)) {
    fprintf(stderr, "Error in mpfr_mul_ui: 1*y != y\n");
    printf("y=  "); mpfr_print_raw(y); putchar('\n');
    printf("1*y="); mpfr_print_raw(x); putchar('\n');
    exit(1);
  }

  mpfr_set_d(x, 1.0/3.0, GMP_RNDZ); 
  mpfr_mul_ui(x, x, 3, GMP_RNDU); 
  if (mpfr_get_d(x) != 1.0) {
    fprintf(stderr, "Error in mpfr_mul_ui: U(Z(1/3)*3) does not give 1\n"); exit(1);
  }

  /* checks sign is correct */
  mpfr_set_d(x, -2.0, GMP_RNDZ);
  mpfr_set_d(y, 3.0, GMP_RNDZ);
  mpfr_mul_ui(x, y, 4, GMP_RNDZ);
  if (SIGN(x) != 1) {
    fprintf(stderr, "Error in mpfr_mul_ui: 4*3.0 does not give a positive result\n"); exit(1);
  }

  mpfr_clear(x); mpfr_clear(y);
  exit (0);
}
