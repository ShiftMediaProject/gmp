/* auxiliary functions for MPFR tests.

Copyright (C) 1999-2000 Free Software Foundation.

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
#ifdef __mips
#include <sys/fpu.h>
#endif

#if defined (hpux)
#define srandom srand48
#define random mrand48
#endif

double drand _PROTO((void)); 
int ulp _PROTO((double, double)); 
double dbl _PROTO((double, int)); 

#define MINNORM 2.2250738585072013831e-308 /* 2^(-1022), smallest normalized */
#define MAXNORM 1.7976931348623157081e308 /* 2^(1023)*(2-2^(-52)) */

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ABS(x) (((x)>0) ? (x) : (-x))

/* generate a random double using the whole range of possible values,
   including denormalized numbers, NaN, infinities, ... */
double drand ()
{
  double d; int *i, expo;

  i = (int*) &d;
  d = 1.0;
  if (i[0]==0) expo=1; /* little endian, exponent in i[1] */
  else expo=0;
  i[0] = lrand48();
  i[1] = lrand48();
  while (i[expo]>=2146435072) i[expo] = lrand48(); /* avoids NaNs */
  if (lrand48()%2 && !isnan(d)) d=-d; /* generates negative numbers */
  return d;
}

/* returns the number of ulp's between a and b */
int ulp (double a, double b)
{
  double eps=1.1102230246251565404e-16; /* 2^(-53) */
  if (a==0.0) {
    if (b==0.0) return 0;
    else if (b<0.0) return 2147483647;
    else return -2147483647;
  }
  b = (a-b)/a;
  if (b>0)
    return (int) floor(b/eps);
  else
    return (int) ceil(b/eps);
}

/* return double m*2^e */
double dbl (double m, int e)
{
  if (e>=0) while (e-->0) m *= 2.0;
  else while (e++<0) m /= 2.0;
  return m;
}
