/* Test file for mpfr_mul.

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
#include "mpfr.h"
#include "mpfr-impl.h"

#define MINNORM 2.2250738585072013831e-308 /* 2^(-1022), smallest normalized */

/* checks that x*y gives the same results in double
   and with mpfr with 53 bits of precision */
void check(double x, double y, unsigned int rnd_mode, unsigned int px, 
unsigned int py, unsigned int pz, double res)
{
  double z1,z2; mpfr_t xx,yy,zz;

  mpfr_init2(xx, px);
  mpfr_init2(yy, py);
  mpfr_init2(zz, pz);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_set_d(yy, y, rnd_mode);
  mpfr_mul(zz, xx, yy, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  z1 = (res==0.0) ? x*y : res;
  z2 = mpfr_get_d(zz);
  if (z1!=z2 && (z1>=MINNORM || z1<=-MINNORM)) {
    printf("mpfr_mul ");
    if (res==0.0) printf("differs from libm.a"); else printf("failed");
    printf(" for x=%1.20e y=%1.20e with rnd_mode=%u\n",x,y,rnd_mode);
    mpfr_print_raw(zz); putchar('\n');
    printf("libm.a gives %1.20e, mpfr_mul gives %1.20e\n",z1,z2);
    exit(1);
  }
  mpfr_clear(xx); mpfr_clear(yy); mpfr_clear(zz);
}

/* checks that x*y gives the same results in double
   and with mpfr with 24 bits of precision */
void check24(float x, float y, unsigned int rnd_mode, float res)
{
  float z1,z2; mpfr_t xx,yy,zz;

  mpfr_init2(xx, 24);
  mpfr_init2(yy, 24);
  mpfr_init2(zz, 24);
  mpfr_set_d(xx, x, rnd_mode);
  mpfr_set_d(yy, y, rnd_mode);
  mpfr_mul(zz, xx, yy, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  z1 = (res==0.0) ? x*y : res;
  z2 = (float) mpfr_get_d(zz);
  if (z1!=z2) {
    printf("mpfr_mul ");
    if (res==0.0) printf("differs from libm.a"); else printf("failed");
    printf(" for x=%1.10e y=%1.10e with prec=24 and rnd_mode=%u\n",x,y,rnd_mode);
    printf("libm.a gives %1.10e, mpfr_mul gives %1.10e\n",z1,z2);
    if (res!=0.0) exit(1);
  }
  mpfr_clear(xx); mpfr_clear(yy); mpfr_clear(zz);
}

/* the following examples come from the paper "Number-theoretic Test 
   Generation for Directed Rounding" from Michael Parks, Table 1 */
void check_float()
{
  int i;
  for (i=0;i<4;i++) {
    if (i!=2) {
    check24(8388609.0, 8388609.0, i, 0.0);
    check24(16777213.0, 8388609.0, i, 0.0);
    check24(8388611.0, 8388609.0, i, 0.0);
    check24(12582911.0, 8388610.0, i, 0.0);
    check24(12582914.0, 8388610.0, i, 0.0);
    check24(13981013.0, 8388611.0, i, 0.0);
    check24(11184811.0, 8388611.0, i, 0.0);
    check24(11184810.0, 8388611.0, i, 0.0);
    check24(13981014.0, 8388611.0, i, 0.0);
    }
  }
  i=GMP_RNDU;
  check24(8388609.0,  8388609.0, GMP_RNDU, 70368769343488.0);
  check24(16777213.0, 8388609.0, GMP_RNDU, 140737479966720.0);
  check24(8388611.0,  8388609.0, GMP_RNDU, 70368786120704.0);
  check24(12582911.0, 8388610.0, GMP_RNDU, 105553133043712.0);
  check24(12582914.0, 8388610.0, GMP_RNDU, 105553166598144.0);
  check24(13981013.0, 8388611.0, GMP_RNDU, 117281279442944.0);
  check24(11184811.0, 8388611.0, GMP_RNDU, 93825036976128.0);
  check24(11184810.0, 8388611.0, GMP_RNDU, 93825020198912.0);
  check24(13981014.0, 8388611.0, GMP_RNDU, 117281296220160.0);
}

/* check sign of result */
void check_sign()
{
  mpfr_t a, b;

  mpfr_init2(a, 53); mpfr_init2(b, 53);
  mpfr_set_d(a, -1.0, GMP_RNDN);
  mpfr_set_d(b, 2.0, GMP_RNDN);
  mpfr_mul(a, b, b, GMP_RNDN);
  if (mpfr_get_d(a) != 4.0) {
    fprintf(stderr,"2.0*2.0 gives %1.20e\n", mpfr_get_d(a)); exit(1);
  }
  mpfr_clear(a); mpfr_clear(b);
}

int main(argc,argv) int argc; char *argv[];
{
  double x,y,z; int i,prec,rnd_mode;

  check_float();
  check(6.9314718055994530941514e-1, 0.0, GMP_RNDZ, 53, 53, 53, 0.0);
  check(0.0, 6.9314718055994530941514e-1, GMP_RNDZ, 53, 53, 53, 0.0);
  prec = (argc<2) ? 53 : atoi(argv[1]);
  rnd_mode = (argc<3) ? -1 : atoi(argv[2]);
  check_sign();
  check(-4.165000000e4, -0.00004801920768307322868063274915, GMP_RNDN, 
	53, 53, 53, 2.0); 
  check(2.71331408349172961467e-08, -6.72658901114033715233e-165,
	GMP_RNDZ, 53, 53, 53, 0.0);
  x=0.31869277231188065; y=0.88642843322303122;
  check(x, y, GMP_RNDZ, 53, 53, 53, 0.0);
  x=8.47622108205396074254e-01; y=3.24039313247872939883e-01;
  check(x, y, GMP_RNDU, 28, 45, 1, 0.5);
  x=2.63978122803639081440e-01; 
  y=5736014.0/8388608.0; /* 6.83786096444222835089e-01; */
  check(x, y, GMP_RNDN, 34, 23, 31, 0.180504585267044603);
  x=9.84891017624509146344e-01; /* rounded to 1.0 with prec=6 */
  x=1.0;
  y=1.18351709358762491320e-01;
  check(x, y, GMP_RNDU, 6, 41, 36, 0.1183517093595583);
  /* the following checks that rounding to nearest sets the last
     bit to zero in case of equal distance */
  check(67108865.0, 134217729.0, GMP_RNDN, 53, 53, 53, 0.0);
  x=1.37399642157394197284e-01; y=2.28877275604219221350e-01;
  check(x, y, GMP_RNDN, 49, 15, 32, 0.0314472340833162888);
  x=4.03160720978664954828e-01; y=5.85483042917246621073e-01;
  check(x, y, GMP_RNDZ, 51, 22, 32, 0.2360436821472831);
  x=3.90798504668055102229e-14; y=9.85394674650308388664e-04;
  check(x, y, GMP_RNDN, 46, 22, 12, 0.385027296503914762e-16);
  x=4.58687081072827851358e-01; y=2.20543551472118792844e-01;
  check(x, y, GMP_RNDN, 49, 3, 1, 0.125);
  for (i=0;i<1000000;) {
    x = drand();
    y = drand();
    z = x*y; if (z<0) z=-z;
    if (z<1e+308 && z>1e-308) /* don't test overflow/underflow for now */
      { i++;
      check(x, y, (rnd_mode==-1) ? lrand48()%4 : rnd_mode, 
	    prec, prec, prec, 0.0);
      }
  } 
  exit (0);
}

