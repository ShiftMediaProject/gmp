/* Test file for mpfr_set_z.

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

#include <stdlib.h>
#include <unistd.h>
#include "gmp.h"
#include "mpfr.h"

/* tset_z z rnd prec */

void check(long i, unsigned char rnd) {
  mpfr_t f; mpz_t z; 

  mpfr_init2(f, 53); mpz_init(z);
  mpz_set_ui(z, i);
  mpfr_set_z(f, z, rnd);
  if ((long)mpfr_get_d(f) != i) {
    printf("Error in mpfr_set_z for i=%ld rnd_mode=%d\n",i,rnd);
    exit(1);
  }
  mpfr_clear(f); mpz_clear(z);
}

void check_large()
{
  mpz_t z; mpfr_t x,y;

  mpz_init(z); mpfr_init2(x, 160); mpfr_init2(y, 160);
  mpz_set_str(z, "77031627725494291259359895954016675357279104942148788042", 10);
  mpfr_set_z(x, z, GMP_RNDN);
  mpfr_set_str_raw(y, "0.1100100100001111110110101010001000100001011010001100001000110100110001001100011001100010100010111000000011011100000111001101000100101001000000100100111000001001E186");
  if (mpfr_cmp(x,y)) {
    fprintf(stderr, "Error in mpfr_set_z on large input\n"); exit(1);
  }
  mpz_clear(z); mpfr_clear(x); mpfr_clear(y);
}

int main(argc,argv) int argc; char *argv[];
{
  long j; 

  check_large();
  srand(getpid());
  for (j=0; j<1000000; j++)
    check(lrand48(), rand()%4);
  exit (0);
}

