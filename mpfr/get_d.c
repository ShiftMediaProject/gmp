/* mpfr_get_d -- convert a multiple precision floating-point number
                 to a machine double precision float

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

#include <float.h>
#ifndef NO_MATH_DEFS
#include <math.h>
#endif
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-math.h"

static double mpfr_scale2 _PROTO ((double, int));

#define IEEE_DBL_MANT_DIG 53

/* multiplies 1/2 <= d <= 1 by 2^exp */
static double
mpfr_scale2 (double d, int exp)
{
#if _GMP_IEEE_FLOATS
  {
    union ieee_double_extract x;

    if (d == 1.0)
      {
        d = 0.5;
        exp ++;
      }

    /* now 1/2 <= d < 1 */

    /* infinities and zeroes have already been checked */
    MPFR_ASSERTN(-1073 <= exp && exp <= 1025);

    x.d = d;
    if (exp < -1021) /* subnormal case */
      {
        x.s.exp += exp + 52;
        x.d *= DBL_EPSILON;
      }
    else /* normalized case */
      {
        x.s.exp += exp;
      }
    return x.d;
  }
#else
  {
    double factor;

    if (exp < 0)
      {
        factor = 0.5;
        exp = -exp;
      }
    else
      {
        factor = 2.0;
      }
    while (exp != 0)
      {
        if ((exp & 1) != 0)
          d *= factor;
        exp >>= 1;
        factor *= factor;
      }
    return d;
  }
#endif
}

/* Assumes IEEE-754 double precision; otherwise, only an approximated
   result will be returned, without any guaranty (and special cases
   such as NaN must be avoided if not supported). */

double
mpfr_get_d3 (mpfr_srcptr src, mp_exp_t e, mp_rnd_t rnd_mode)
{
  double d;
  int negative;

  if (MPFR_IS_NAN(src))
    return MPFR_DBL_NAN;

  negative = MPFR_SIGN(src) < 0;

  if (MPFR_IS_INF(src))
    return negative ? MPFR_DBL_INFM : MPFR_DBL_INFP;

  if (MPFR_IS_ZERO(src))
    return negative ? -0.0 : 0.0;

  /* the smallest normalized number is 2^(-1022)=0.1e-1021, and the smallest
     subnormal is 2^(-1074)=0.1e-1073 */
  if (e < -1073)
    {
      d = negative ?
        (rnd_mode == GMP_RNDD ||
         (rnd_mode == GMP_RNDN && mpfr_cmp_si_2exp(src, -1, -1075) < 0)
         ? -DBL_MIN : -0.0) :
        (rnd_mode == GMP_RNDU ||
         (rnd_mode == GMP_RNDN && mpfr_cmp_si_2exp(src, 1, -1075) > 0)
         ? DBL_MIN : 0.0);
      if (d != 0.0)
        d *= DBL_EPSILON;
    }
  /* the largest normalized number is 2^1024*(1-2^(-53))=0.111...111e1024 */
  else if (e > 1024)
    {
      d = negative ?
        (rnd_mode == GMP_RNDZ || rnd_mode == GMP_RNDU ?
         -DBL_MAX : MPFR_DBL_INFM) :
        (rnd_mode == GMP_RNDZ || rnd_mode == GMP_RNDD ?
         DBL_MAX : MPFR_DBL_INFP);
    }
  else
    {
      int nbits;
      mp_size_t np, i;
      mp_ptr tp;
      int carry;

      nbits = IEEE_DBL_MANT_DIG; /* 53 */
      if (e < -1021) /* in the subnormal case, compute the exact number of
                        significant bits */
        {
          nbits += (1021 + e);
          MPFR_ASSERTN(nbits >= 1);
        }
      np = (nbits - 1) / BITS_PER_MP_LIMB;
      tp = (mp_ptr) (*__gmp_allocate_func) ((np + 1) * BYTES_PER_MP_LIMB);
      carry = mpfr_round_raw (tp, MPFR_MANT(src), MPFR_PREC(src), negative,
                             nbits, rnd_mode, (int *) 0);
      if (carry)
        d = 1.0;
      else
        {
          /* Warning: the rounding may still be incorrect in the rounding
             to the nearest mode when the result is a subnormal because of
             a double rounding (-> 53 bits -> final precision). */
          d = (double) tp[0] / MP_BASE_AS_DOUBLE;
          for (i = 1; i <= np; i++)
            d = (d + tp[i]) / MP_BASE_AS_DOUBLE;
          /* d is the mantissa (between 1/2 and 1) of the argument rounded
             to 53 bits */
        }
      d = mpfr_scale2 (d, e);
      if (negative)
        d = -d;

      (*__gmp_free_func) (tp, (np + 1) * BYTES_PER_MP_LIMB);
    }

  return d;
}

/* Note: do not read the exponent if it has no meaning (avoid possible
   traps on some implementations). */

double
mpfr_get_d (mpfr_srcptr src, mp_rnd_t rnd_mode)
{
  return mpfr_get_d3 (src, MPFR_IS_FP(src) && MPFR_NOTZERO(src) ?
                      MPFR_EXP(src) : 0, rnd_mode);
}

double
mpfr_get_d1 (mpfr_srcptr src)
{
  return mpfr_get_d3 (src, MPFR_IS_FP(src) && MPFR_NOTZERO(src) ?
                      MPFR_EXP(src) : 0, __gmp_default_rounding_mode);
}
