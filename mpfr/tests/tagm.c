/* Test file for mpfr_agm.

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
#include <sys/types.h>
#include <unistd.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"

extern int isnan();

double drand_agm()
{
  double d; long int *i;

  i = (long int*) &d;
  do {
    i[0] = lrand48();
    i[1] = lrand48();
  /*if (lrand48()%2) d=-d; */ /* generates negative numbers */
                              /* useless here */
  } while ((d<1e-153)||(d>1e153));    /* to avoid underflow or overflow
					 in double calculus in sqrt(u*v) */

  return d;
}


double max(double a,double b) {
  if (a>=b)
    return a;
  return b;
}

double min(double a,double b) {
   if (b>=a)
    return a;
  return b;
}

double dagm(double a, double b) { 
  double u,v,tmpu,tmpv;
  
  if ((isnan(a))||(isnan(b)))
    return a+b;

  tmpv=max(a,b);
  tmpu=min(a,b);

  do
    {
      u=tmpu;
      v=tmpv;
      tmpu=sqrt(u*v);
      tmpv=(u+v)/2.0;
    }
  while (!(((tmpu==u)&&(tmpv==v))||(ulp(u,v)==0))); 

  /*  printf("difference : %i ulp\n",ulp(u,v)); */
	 return u;
}

#define check(a,b,r) check4(a,b,r,0.0)

void check4(double a, double b, unsigned char rnd_mode, double res1)
{
  mpfr_t ta, tb, tres;
  double res2;
  int ck=0;

  mpfr_init2(ta, 53);
  mpfr_init2(tb, 53);
  mpfr_init2(tres, 53);
  
  mpfr_set_d(ta, a, rnd_mode);
  mpfr_set_d(tb, b, rnd_mode);

  mpfr_agm(tres, ta, tb, rnd_mode);
  mpfr_set_machine_rnd_mode(rnd_mode);
  
  if (res1==0.0) res1=dagm(a,b); else ck=1;
  res2 = mpfr_get_d(tres);

  if (ck && res1!=res2 && (!isnan(res1) || !isnan(res2))) {
    printf("mpfr_agm failed for a=%1.20e, b=%1.20e, rnd_mode=%d\n",a,b,rnd_mode);
    printf("expected result is %1.20e, got %1.20e (%d ulp)\n",res1,res2,
	   ulp(res2,res1));
    /*exit(1);*/
  }
  mpfr_clear(ta); mpfr_clear(tb); mpfr_clear(tres); 
}

void check_large()
{
  mpfr_t a, b, agm;

  mpfr_init2(a, 82); mpfr_init2(b, 82); mpfr_init2(agm, 82);
  mpfr_set_ui(a, 1, GMP_RNDN);
  mpfr_set_str_raw(b, "0.1111101100001000000001011000110111101000001011111000100001000101010100011111110010E-39");
  mpfr_agm(agm, a, b, GMP_RNDN);
  mpfr_set_str_raw(a, "0.1110001000111101101010101010101101001010001001001011100101111011110101111001111100E-4");
  if (mpfr_cmp(agm, a)) {
    fprintf(stderr, "mpfr_agm failed for precision 82\n"); exit(1);
  }
  mpfr_clear(a); mpfr_clear(b); mpfr_clear(agm);
}

void slave(int N, int p) {
  int i;
  double a,b;
  mpfr_t ta, tb, tres;

  srand48(getpid());
  mpfr_init2(ta, 53);
  mpfr_init2(tb, 53);
  mpfr_init2(tres, p);
  for(i=0;i<N;i++) {
    a=drand_agm();
    b=drand_agm();
    mpfr_set_d(ta, a, GMP_RNDN);
    mpfr_set_d(tb, b, GMP_RNDN);
    mpfr_agm(tres, ta, tb, rand() % 4 );
  }
    mpfr_clear(ta); mpfr_clear(ta); mpfr_clear(tres); 
    printf("fin\n");
}


int main(int argc, char* argv[]) {
   int N;

   if (argc==3) {   /* tagm N p : N calculus with precision p*/
     printf("Doing %d random tests in %d precision\n",atoi(argv[1]),atoi(argv[2]));
     slave(atoi(argv[1]),atoi(argv[2]));
     exit (0);
   }

   if (argc==2) { /* tagm N: N tests with random double's */
     int i;
     double a,b;

     srand48(getpid()); 
     N = atoi(argv[1]);
     for (i=0;i<N;i++) {
       a = drand(); 
       b = drand();
       check(a, b, rand() % 4);
     } 
     exit (0);
   }
   else {
     check_large();
     check(2,1,GMP_RNDN);
     check(6,4,GMP_RNDN); 
     check(62,61,GMP_RNDN);
     check(0.5,1,GMP_RNDN);
     check(1,2,GMP_RNDN); 
     check4(234375765,234375000,GMP_RNDN,2.34375382499843955040e+08);
     check(8,1,GMP_RNDU);
     check(1,44,GMP_RNDU);  
     check(1,3.725290298461914062500000e-9,GMP_RNDU); 
   } 
   exit (0);
}
