/* mpfr_set_d, mpfr_get_d -- convert a multiple precision floating-point number
                             from/to a machine double precision float

Copyright (C) 1999-2002 Free Software Foundation, Inc.

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
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#if (BITS_PER_MP_LIMB==32)
#define MPFR_LIMBS_PER_DOUBLE 2
#elif (BITS_PER_MP_LIMB >= 64)
#define MPFR_LIMBS_PER_DOUBLE 1
#elif (BITS_PER_MP_LIMB == 16)
#define MPFR_LIMBS_PER_DOUBLE 4
#endif

static int __mpfr_extract_double _PROTO ((mp_ptr, double, int));
static double __mpfr_scale2 _PROTO ((double, int));

#define NaN (0./0.) /* ensures a machine-independent NaN */
#define Infp (1/0.)
#define Infm (-1/0.)

/* Included from gmp-2.0.2, patched to support denorms */

#ifdef XDEBUG
#undef _GMP_IEEE_FLOATS
#endif

#ifndef _GMP_IEEE_FLOATS
#define _GMP_IEEE_FLOATS 0
#endif

static int
__mpfr_extract_double (mp_ptr rp, double d, int e)
     /* e=0 iff BITS_PER_MP_LIMB=32 and rp has only one limb */
{
  long exp;
  mp_limb_t manl;
#if BITS_PER_MP_LIMB == 32
  mp_limb_t manh;
#endif

  /* BUGS

     1. Should handle Inf and NaN in IEEE specific code.
     2. Handle Inf and NaN also in default code, to avoid hangs.
     3. Generalize to handle all BITS_PER_MP_LIMB >= 32.
     4. This lits is incomplete and misspelled.
   */

  if (d == 0.0)
    {
      rp[0] = 0;
#if BITS_PER_MP_LIMB == 32
      if (e) rp[1] = 0;
#endif
      return 0;
    }

#if _GMP_IEEE_FLOATS
  {
    union ieee_double_extract x;
    x.d = d;

    exp = x.s.exp;
    if (exp)
      {
#if BITS_PER_MP_LIMB == 64
	manl = ((MP_LIMB_T_ONE << 63)
		| ((mp_limb_t) x.s.manh << 43) | ((mp_limb_t) x.s.manl << 11));
#else
	manh = (MP_LIMB_T_ONE << 31) | (x.s.manh << 11) | (x.s.manl >> 21);
	manl = x.s.manl << 11;
#endif
      }
    else
      {
#if BITS_PER_MP_LIMB == 64
	manl = ((mp_limb_t) x.s.manh << 43) | ((mp_limb_t) x.s.manl << 11);
#else
    manh = (x.s.manh << 11) | (x.s.manl >> 21);
	manl = x.s.manl << 11;
#endif
      }
  }
#else
  {
    /* Unknown (or known to be non-IEEE) double format.  */
    exp = 0;
    if (d >= 1.0)
      {
        if (d * 0.5 == d)
          abort ();

        while (d >= 32768.0)
          {
            d *= (1.0 / 65536.0);
            exp += 16;
          }
        while (d >= 1.0)
          {
            d *= 0.5;
            exp += 1;
          }
      }
    else if (d < 0.5)
      {
        while (d < (1.0 / 65536.0))
          {
            d *=  65536.0;
            exp -= 16;
          }
        while (d < 0.5)
          {
            d *= 2.0;
            exp -= 1;
          }
      }

    d *= MP_BASE_AS_DOUBLE;
#if BITS_PER_MP_LIMB == 64
    manl = d;
#else
    manh = d;
    manl = (d - manh) * MP_BASE_AS_DOUBLE;
#endif

    exp += 1022;
  }
#endif

  if (exp) exp = (unsigned) exp - 1022; else exp = -1021;

#if BITS_PER_MP_LIMB == 64
      rp[0] = manl;
#else
      if (e) {
	rp[1] = manh;
	rp[0] = manl;
      }
      else {
	rp[0] = manh;
      }
#endif

  return exp;
}

/* End of part included from gmp-2.0.2 */
/* Part included from gmp temporary releases and modified */
static double
__mpfr_scale2 (double d, int exp)
{
#if _GMP_IEEE_FLOATS
  {
    union ieee_double_extract x;

    if (exp < -2099)
      return 0.0 * d; /* 0 with the correct sign */

    x.d = d;
    if (exp >= 2047 || exp + x.s.exp >= 2047)
      {
        /* Return +-infinity */
        x.s.exp = 2047;
        x.s.manl = x.s.manh = 0;
      }
    else if (exp + x.s.exp < 1)
      {
        exp += x.s.exp;
        if (exp <= -52)
          return 0.0 * d; /* 0 with the correct sign */
        x.s.exp = 1; /* smallest exponent (biased) */
        x.d *= __mpfr_scale2(1.0, exp - 1);
      }
    else
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
    return r;
  }
#endif
}

/* End of part included from gmp */

int
mpfr_set_d (mpfr_ptr r, double d, mp_rnd_t rnd_mode)
{
  int signd, sizer, sizetmp, inexact;
  unsigned int cnt;
  mpfr_ptr tmp;
  TMP_DECL(marker);

  TMP_MARK(marker);
  MPFR_CLEAR_FLAGS(r);

  if (d == 0)
    {
      union ieee_double_extract x;

      MPFR_SET_ZERO(r);
      /* set correct sign */
      x.d = d;
      if (((x.s.sig == 1) && (MPFR_SIGN(r) > 0))
	  || ((x.s.sig == 0) && (MPFR_SIGN(r) < 0)))
	MPFR_CHANGE_SIGN(r);
      return 0; /* 0 is exact */
  }

  else if (DOUBLE_ISNAN(d))
    {
      MPFR_SET_NAN(r);
      return 1; /* a NaN is always inexact */
    }

  else if (DOUBLE_ISINF(d))
    {
      MPFR_SET_INF(r);
      if ((d > 0 && (MPFR_SIGN(r) == -1)) || (d < 0 && (MPFR_SIGN(r) == 1)))
	MPFR_CHANGE_SIGN(r);
      return 0; /* infinity is exact */
    }

  sizer = (MPFR_PREC(r) - 1) / BITS_PER_MP_LIMB + 1;

  /* warning: don't use tmp=r here, even if sizer >= MPFR_LIMBS_PER_DOUBLE,
     since PREC(r) may be different from PREC(tmp), and then both variables
     would have same precision in the mpfr_set4 call below. */
  tmp = (mpfr_ptr) TMP_ALLOC(sizeof(mpfr_t));
  MPFR_MANT(tmp) = TMP_ALLOC(MPFR_LIMBS_PER_DOUBLE * BYTES_PER_MP_LIMB);
  MPFR_PREC(tmp) = 53;
  MPFR_SIZE(tmp) = MPFR_LIMBS_PER_DOUBLE;
  sizetmp = MPFR_LIMBS_PER_DOUBLE;

  signd = (d < 0) ? -1 : 1;
  d = ABS (d);

  MPFR_EXP(tmp) = __mpfr_extract_double (MPFR_MANT(tmp), d, 1);

  count_leading_zeros(cnt, MPFR_MANT(tmp)[sizetmp - 1]);

  if (cnt)
    mpn_lshift (MPFR_MANT(tmp), MPFR_MANT(tmp), sizetmp, cnt);

  MPFR_EXP(tmp) -= cnt;

  /* tmp is exact since PREC(tmp)=53 */
  inexact = mpfr_set4(r, tmp, rnd_mode, signd);

  TMP_FREE(marker);
  return inexact;
}

double
mpfr_get_d2 (mpfr_srcptr src, mp_exp_t e)
{
  double d;
  mpfr_t tmp;
  int s, negative;

  if (MPFR_IS_NAN(src))
    return NaN;

  negative = MPFR_SIGN(src) < 0;

  if (MPFR_IS_INF(src))
    return negative ? Infm : Infp;

  if (MPFR_IS_ZERO(src))
    return negative ? -0.0 : 0.0;

  if (e < -1076)
    { /* Simulate the underflow with the current IEEE rounding mode. */
      d = DBL_MIN;
      d *= negative ? -DBL_MIN : DBL_MIN;
      /* -> double precision forced by the affectation */
    }
  else if (e > 1024)
    { /* Simulate the overflow with the current IEEE rounding mode. */
      d = DBL_MAX;
      d *= negative ? -2 : 2;
    }
  else
    {
      mp_ptr tp;
      mp_size_t np, i;
      double r;

      mpfr_save_emin_emax();

      /* Truncate src to 54 bits
       * --> rounding bit stored to the 54th bit
       * --> sticky bit stored to variable s */
      mpfr_init2(tmp, 54);
      s = mpfr_set(tmp, src, GMP_RNDZ);
      MPFR_ASSERTN(MPFR_IS_FP(tmp) && MPFR_NOTZERO(tmp));

      /* Warning: the rounding may still be incorrect in the rounding
         to the nearest mode when the result is a subnormal because of
         a double rounding (-> 53 bits -> final precision). */
      tp = MPFR_MANT(tmp);
      d = (tp[0] & (MP_LIMB_T_MAX << 11)) / MP_BASE_AS_DOUBLE;
      np = (MPFR_PREC(tmp) - 1) / BITS_PER_MP_LIMB;
      for (i = 1; i <= np; i++)
        d = (d + tp[i]) / MP_BASE_AS_DOUBLE;
      /* d is the mantissa (between 1/2 and 1) of the argument truncated
         to 53 bits */
      r = (((tp[0] >> 9) & 2) + (s != 0)) * (DBL_EPSILON / 8.0);
      d += r; /* double precision forced by the affectation */
      d = __mpfr_scale2(d, e);
      if (negative)
        d = -d;

      mpfr_clear(tmp);
      mpfr_restore_emin_emax();
    }

  return d;
}

double
mpfr_get_d (mpfr_srcptr src)
{
  return mpfr_get_d2 (src, MPFR_EXP(src));
}
