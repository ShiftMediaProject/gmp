/* auxiliary functions for MPFR tests.

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#ifdef __mips
#include <sys/fpu.h>
#endif

/* set precision control to double on x86 */
#if (defined (__i386__) || defined (__i486__))
#ifdef __CYGWIN32__ /* no fpu_control.h under Cygnus */
#define _FPU_EXTENDED 0x300
#define _FPU_DOUBLE   0x200
#define _FPU_DEFAULT  0x137f
#define HAVE_SETFPUCW
#else
#ifdef HAVE_FPU_CONTROL_H
#include <fpu_control.h>
#define HAVE_SETFPUCW
#endif
#endif /* ifdef __CYGWIN32__ */
#ifndef __setfpucw
#define __setfpucw(cw) __asm__ ("fldcw %0" : : "m" (cw))
#endif /* ifndef __setfpucw */
#endif /* __i386__ */

/* generates a random long int, a random double,
   and corresponding seed initializing */
#ifdef HAVE_LRAND48
#define LONG_RAND lrand48
#define DBL_RAND  drand48
#define SEED_RAND srand48
#else
#define LONG_RAND random
#define DBL_RAND() ((double) random() / (double) RAND_MAX)
#define SEED_RAND srandom
#endif

#if defined (__hpux)
#define srandom srand48
#define random() (mrand48() & 0x7fffffff)
#endif

void mpfr_test_init _PROTO ((void));
double drand _PROTO ((void)); 
int ulp _PROTO ((double, double)); 
double dbl _PROTO ((double, int)); 
double Ulp _PROTO ((double));

#define MINNORM 2.2250738585072013831e-308 /* 2^(-1022), smallest normalized */
#define MAXNORM 1.7976931348623157081e308 /* 2^(1023)*(2-2^(-52)) */

/* The MAX, MIN and ABS macros may already be defined if gmp-impl.h has
   been included. They have the same semantics as in gmp-impl.h, but the
   expressions may be slightly different. So, it's better to undefine
   them first, as required by the ISO C standard. */
#undef MAX
#undef MIN
#undef ABS
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ABS(x) (((x)>0) ? (x) : -(x))

/* initialization function for tests using the hardware floats */
void
mpfr_test_init ()
{
#ifdef __mips
  /* to get denormalized numbers on IRIX64 */
  union fpc_csr exp;

  exp.fc_word = get_fpc_csr();
  exp.fc_struct.flush = 0;
  set_fpc_csr(exp.fc_word);
#endif

#ifdef HAVE_SETFPUCW
  /* sets the precision to double */
  __setfpucw((_FPU_DEFAULT & (~_FPU_EXTENDED)) | _FPU_DOUBLE);
#endif
}

/* generate a random double using the whole range of possible values,
   including denormalized numbers, NaN, infinities, ... */
double
drand (void)
{
  double d; int *i, expo;

  i = (int*) &d;
  d = 1.0;
  if (i[0] == 0)
    expo = 1; /* little endian, exponent in i[1] */
  else
    expo = 0;
  i[0] = LONG_RAND();
  i[1] = LONG_RAND();
  while (i[expo] >= 2146435072)
    i[expo] = LONG_RAND(); /* avoids NaNs */
  if ((LONG_RAND() % 2) && !isnan (d))
    d = -d; /* generates negative numbers */
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
