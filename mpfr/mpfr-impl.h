/* Utilities for MPFR developers, not exported.

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

/* generate a random double using the whole range of possible values,
   including denormalized numbers, NaN, infinities, ... */
double drand()
{
  volatile double d; int *i;

  i = (int*) &d;
  i[0] = mrand48();
  i[1] = mrand48();
  return d;
}

/* returns the number of ulp's between a and b */
int ulp(a,b) double a,b;
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
double dbl(m,e) double m; int e;
{
  if (e>=0) while (e-->0) m *= 2.0;
  else while (e++<0) m /= 2.0;
  return m;
}
