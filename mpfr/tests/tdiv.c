/* Test file for mpfr_div.

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#ifdef IRIX64
#include <sys/fpu.h>
#endif

extern int isnan();

/* #define DEBUG */

#define check(n,d,r) check4(n,d,r,53)

void check4(N, D, rnd_mode, p) double N, D; unsigned char rnd_mode; int p;
{
  mpfr_t q, n, d; double Q,Q2;

#ifdef DEBUG
  printf("N=%1.20e D=%1.20e rnd_mode=%d\n",N,D,rnd_mode);
#endif
  mpfr_init2(q, p); mpfr_init2(n, p); mpfr_init2(d, p);
  mpfr_set_d(n, N, rnd_mode);
  mpfr_set_d(d, D, rnd_mode);
  mpfr_div(q, n, d, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  Q = N/D;
  Q2 = mpfr_get_d(q);
#ifdef DEBUG
    printf("expected quotient is %1.20e, got %1.20e (%d ulp)\n",Q,Q2,
	   ulp(Q2,Q));
    mpfr_print_raw(q); putchar('\n');
#endif
  if (Q!=Q2 && (!isnan(Q) || !isnan(Q2))) {
    printf("mpfr_div failed for n=%1.20e, d=%1.20e, rnd_mode=%d\n",N,D,rnd_mode);
    printf("expected quotient is %1.20e, got %1.20e (%d ulp)\n",Q,Q2,
	   ulp(Q2,Q));
    exit(1);
  }
  mpfr_clear(q); mpfr_clear(n); mpfr_clear(d);  
}

void check24(float N, float D, unsigned char rnd_mode)
{
  mpfr_t q, n, d; float Q,Q2;

  mpfr_init2(q, 24); mpfr_init2(n, 24); mpfr_init2(d, 24);
  mpfr_set_d(n, N, rnd_mode);
  mpfr_set_d(d, D, rnd_mode);
  mpfr_div(q, n, d, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  Q = N/D;
  Q2 = mpfr_get_d(q);
  if (Q!=Q2 && (!isnan(Q) || !isnan(Q2))) {
    printf("mpfr_div failed for n=%1.10e, d=%1.10e, prec=24, rnd_mode=%d\n",N,D,rnd_mode);
    printf("expected quotient is %1.10e, got %1.10e\n",Q,Q2);
    exit(1);
  }
  mpfr_clear(q); mpfr_clear(n); mpfr_clear(d);  
}

/* the following examples come from the paper "Number-theoretic Test 
   Generation for Directed Rounding" from Michael Parks, Table 2 */
void check_float()
{
  int i; float b=8388608.0; /* 2^23 */

  for (i=0;i<4;i++) {
    check24(b*8388610.0, 8388609.0, i);
    check24(b*16777215.0, 16777213.0, i);
    check24(b*8388612.0, 8388611.0, i);
    check24(b*12582914.0, 12582911.0, i);
    check24(12582913.0, 12582910.0, i);
    check24(b*16777215.0, 8388609.0, i);
    check24(b*8388612.0, 8388609.0, i);
    check24(b*12582914.0, 8388610.0, i);
    check24(b*12582913.0, 8388610.0, i);
  }
}

void check_convergence()
{
  mpfr_t x, y;
  
  mpfr_init2(x, 130); 
  mpfr_set_str_raw(x, "0.1011111101011010101000001010011111101000011100011101010011111011000011001010000000111100100111110011001010110100100001001000111001E6944");
  mpfr_init_set_ui(y, 5, 130, GMP_RNDN);
  mpfr_div(x, x, y, GMP_RNDD); /* exact division */
  mpfr_clear(x); mpfr_clear(y);
}

int main(int argc, char *argv[])
{
  int i, N; double n, d, e; 
#ifdef IRIX64
    /* to get denormalized numbers on IRIX64 */
    union fpc_csr exp;
    exp.fc_word = get_fpc_csr();
    exp.fc_struct.flush = 0;
    set_fpc_csr(exp.fc_word);
#endif

  N = (argc>1) ? atoi(argv[1]) : 100000;
  check_float(); /* checks single precision */
  check_convergence();
  check(0.0, 1.0, 1);
  check(-7.49889692246885910000e+63, 4.88168664502887320000e+306, GMP_RNDD);
  check(-1.33225773037748601769e+199, 3.63449540676937123913e+79, GMP_RNDZ);
  d = 1.0; for (i=0;i<52;i++) d *= 2.0;
  check4(4.0, d, GMP_RNDZ, 62);
  check4(1.0, 2.10263340267725788209e+187, 2, 65);
  check4(2.44394909079968374564e-150, 2.10263340267725788209e+187, 2, 65);
  /* the following tests when d is an exact power of two */
  check(9.89438396044940256501e-134, 5.93472984109987421717e-67, 2);
  check(9.89438396044940256501e-134, -5.93472984109987421717e-67, 2);
  check(-4.53063926135729747564e-308, 7.02293374921793516813e-84, 3);
  check(6.25089225176473806123e-01, -2.35527154824420243364e-230, 3);
  check(6.52308934689126000000e+15, -1.62063546601505417497e+273, 0);
  check(1.04636807108079349236e-189, 3.72295730823253012954e-292, 1);
  srand48(getpid());
  for (i=0;i<N;i++) {
    do { n = drand(); d = drand(); e = fabs(n)/fabs(d); }
    /* smallest normalized is 2^(-1022), largest is 2^(1023)*(2-2^(-52)) */
    while (e>=1.7976931348623157081e308 || e<2.225073858507201383e-308);
    check(n, d, rand() % 4);
  }
  exit (0);
}
