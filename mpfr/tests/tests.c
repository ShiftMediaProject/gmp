/* Miscellaneous support for test programs.

Copyright 2001, 2002 Free Software Foundation, Inc.

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

#include <stdio.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"


void
tests_start_mpfr (void)
{
  tests_memory_start ();
}

void
tests_end_mpfr (void)
{
  if (__gmpfr_const_pi_prec != 0)
    {
      mpfr_clear (__mpfr_const_pi);
      __gmpfr_const_pi_prec = 0;
    }

  if (__gmpfr_const_log2_prec != 0)
    {
      mpfr_clear (__mpfr_const_log2);
      __gmpfr_const_log2_prec = 0;
    }

  if (__gmp_rands_initialized)
    {
      gmp_randclear (__gmp_rands);
      __gmp_rands_initialized = 0;
    }

  tests_memory_end ();
}

/* initialization function for tests using the hardware floats */
void
mpfr_test_init ()
{
  double c, d;
  int j;
#ifdef __mips
  /* to get denormalized numbers on IRIX64 */
  union fpc_csr exp;

  exp.fc_word = get_fpc_csr();
  exp.fc_struct.flush = 0;
  set_fpc_csr(exp.fc_word);
#endif
#ifdef HAVE_DENORMS
  d = DBL_MIN;
  if (2.0 * (d / 2.0) != d)
    {
      fprintf (stderr, "Warning: no denormalized numbers\n");
      exit (1);
    }
#endif

#ifdef HAVE_SETFPUCW
  /* sets the precision to double */
  __setfpucw((_FPU_DEFAULT & (~_FPU_EXTENDED)) | _FPU_DOUBLE);
#endif
  c = 1.46484375e-3;
  d = 1.0;
  for (j=0; j<54; j++) d *= 0.5;
  d = 0.75 + d;
  d /= 1 << 9;
  if (c != d)
    {
      fprintf (stderr, "Warning: extended precision not disabled\n");
      exit (1);
    }
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

/* returns the number of ulp's between a and b,
   where a and b can be any floating-point number, except NaN
 */
int
ulp (double a, double b)
{
  double twoa;

  if (a == b) return 0; /* also deals with a=b=inf or -inf */

  twoa = a + a;
  if (twoa == a) /* a is +/-0.0 or +/-Inf */
    return ((b < a) ? INT_MAX : -INT_MAX);

  return (a - b) / Ulp (a);
}

/* return double m*2^e */
double
dbl (double m, int e)
{
  if (e >=0 )
    while (e-- > 0)
      m *= 2.0;
  else
    while (e++ < 0)
      m /= 2.0;
  return m;
}

#ifndef HAVE_ISNAN
int
Isnan (double d)
{
  return (d) != (d);
}
#endif
