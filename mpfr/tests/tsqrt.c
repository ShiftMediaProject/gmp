/* Test file for mpfr_sqrt.

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

extern int isnan(), getpid();

#define check(a,r) check3(a,r,-1.0)

int maxulp=0;

void check3(a, rnd_mode, Q) double a; unsigned char rnd_mode; double Q;
{
  mpfr_t q; double Q2; int ck,u;

  ck = (Q!=-1.0); /* if ck=1, then Q is certified correct */
  mpfr_init2(q, 53);
  mpfr_set_d(q, a, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  mpfr_sqrt(q, q, rnd_mode);
  if (ck==0) Q = sqrt(a);
  Q2 = mpfr_get_d(q);
  if (Q!=Q2 && (!isnan(Q) || !isnan(Q2))) {
    u = ulp(Q2,Q);
    if (ck) {
      printf("mpfr_sqrt failed for a=%1.20e, rnd_mode=%d\n",a,rnd_mode);
      printf("expected sqrt is %1.20e, got %1.20e (%d ulp)\n",Q,Q2,u);
      exit(1);
    }
    else if (u>maxulp || u<-maxulp) {
      maxulp = (u>maxulp) ? u : -u;
      printf("libm.a differs from mpfr_sqrt for a=%1.20e, rnd_mode=%d\n",a,rnd_mode);
      printf("libm.a gives %1.20e, mpfr_sqrt gives %1.20e (%d ulp)\n",Q,Q2,u);
    }
  }
  mpfr_clear(q);
}

void check24(a, rnd_mode) float a; unsigned char rnd_mode;
{
  mpfr_t q; float Q,Q2;

  mpfr_init2(q, 24);
  mpfr_set_d(q, a, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  mpfr_sqrt(q, q, rnd_mode);
  Q = sqrt(a);
  Q2 = mpfr_get_d(q);
  if (Q!=Q2 && (!isnan(Q) || !isnan(Q2))) {
    printf("mpfr_sqrt failed for a=%1.10e, rnd_mode=%d\n",a,rnd_mode);
    printf("expected sqrt is %1.10e, got %1.10e\n",Q,Q2);
    exit(1);
  }
  mpfr_clear(q);
}

/* the following examples come from the paper "Number-theoretic Test 
   Generation for Directed Rounding" from Michael Parks, Table 3 */
void check_float()
{
  int i; float b = 8388608.0; /* 2^23 */

  for (i=0;i<4;i++) {
    check24(b*8388610.0, i);
    check24(b*2.0*16777214.0, i);
    check24(b*8388612.0, i);
    check24(b*2.0*16777212.0, i);
    check24(b*11946704.0, i);
    check24(b*14321479.0, i);
    check24(b*2.0*13689673.0, i);
    check24(b*8388614.0, i);
    check24(b*2.0*16777210.0, i);
    check24(b*10873622.0, i);
  }
}

int main()
{
  int i; double a;
#ifdef IRIX64
    /* to get denormalized numbers on IRIX64 */
    union fpc_csr exp;
    exp.fc_word = get_fpc_csr();
    exp.fc_struct.flush = 0;
    set_fpc_csr(exp.fc_word);
#endif

  check_float();
  srand(getpid());
/* the following examples come from the paper "Number-theoretic Test 
   Generation for Directed Rounding" from Michael Parks, Table 4 */
  a = 4503599627370496.0; /* 2^52 */
  for (i=0;i<4;i++) {
    check(a*2.0*8732221479794286.0, i);
    check(a*8550954388695124.0, i);
    check(a*7842344481681754.0, i);
    check(a*5935035262218600.0, i);
    check(a*5039650445085418.0, i);
    check(a*5039721545366078.0, i);
    check(a*8005963117781324.0, i);
    check(a*6703494707970582.0, i);
    check(a*8010323124937260.0, i);
    check(a*2.0*8010776873384260.0, i);
  }
  check(6.37983013646045901440e+32, GMP_RNDN);
  check(1.0, GMP_RNDN);
  check(1.0, GMP_RNDZ);
  check(3.725290298461914062500000e-9, GMP_RNDN);
  check(3.725290298461914062500000e-9, GMP_RNDZ);
  a=1190456976439861.0;
  check3(a, GMP_RNDZ, dbl(4630914205854029.0,-27));
  check3(1024.0*a, GMP_RNDZ, dbl(4630914205854029.0,-22));
  /* the following examples are bugs in Cygnus compiler/system, found by
     Fabrice Rouillier while porting mpfr to Windows */
  check3(9.89438396044940256501e-134, GMP_RNDU, 3.14553397063986684729e-67);
  check3(7.86528588050363751914e+31, GMP_RNDZ, 8.86864469944739400000e+15);
  for (i=0;i<100000;i++) {
    a = drand();
    check(a, rand() % 4);
  }
  exit (0);
}
