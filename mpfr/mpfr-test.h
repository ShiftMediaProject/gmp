/* auxiliary functions for MPFR tests.

Copyright (C) 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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

#include <math.h>
#ifdef __mips
#include <sys/fpu.h>
#endif

#if defined (__hpux)
#define srandom srand48
#define random() (mrand48() & 0x7fffffff)
#endif

double drand _PROTO ((void)); 
int ulp _PROTO ((double, double)); 
double dbl _PROTO ((double, int)); 
double Ulp _PROTO ((double));

#define MINNORM 2.2250738585072013831e-308 /* 2^(-1022), smallest normalized */
#define MAXNORM 1.7976931348623157081e308 /* 2^(1023)*(2-2^(-52)) */

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef ABS
#define ABS(x) (((x)>0) ? (x) : (-x))
#endif

/* generate a random double using the whole range of possible values,
   including denormalized numbers, NaN, infinities, ... */
double
drand ()
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

/* returns ulp(x) for x a 'normal' double-precision number */
double
Ulp (double x)
{
   double y, eps;

   if (x < 0) x = -x;

   y = x * 2.220446049250313080847263336181640625e-16 ; /* x / 2^52 */

   /* as ulp(x) <= y = x/2^52 < 2*ulp(x),
   we have x + ulp(x) <= x + y <= x + 2*ulp(x),
   therefore o(x + y) = x + ulp(x) or x + 2*ulp(x) */

   eps =  x + y;
   eps = eps - x; /* ulp(x) or 2*ulp(x) */

   return (eps > y) ? 0.5 * eps : eps;
}

/* returns the number of ulp's between a and b */
int
ulp (double a, double b)
{
  if (a==0.0) {
    if (b==0.0) return 0;
    else if (b<0.0) return 2147483647;
    else return -2147483647;
  }
  return (a-b)/Ulp(a);
}

/* return double m*2^e */
double
dbl (double m, int e)
{
  if (e>=0) while (e-->0) m *= 2.0;
  else while (e++<0) m /= 2.0;
  return m;
}
