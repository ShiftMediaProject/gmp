/* mpfr_set_d, mpfr_get_d -- convert a multiple precision floating-point number
                             from/to a machine double precision float

Copyright (C) 1999, 2001 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "gmp-impl.h"
#include "longlong.h"

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
/* Part included from gmp temporary releases */
static double
__mpfr_scale2 (double d, int exp)
{
#if _GMP_IEEE_FLOATS
  {
    union ieee_double_extract x;
    x.d = d;
    exp += x.s.exp;
    x.s.exp = exp;
    if (exp >= 2047)
      {
        /* Return +-infinity */
        x.s.exp = 2047;
        x.s.manl = x.s.manh = 0;
      }
    else if (exp < 1)
      {
        x.s.exp = 1;            /* smallest exponent (biased) */
        /* Divide result by 2 until we have scaled it to the right IEEE
           denormalized number, but stop if it becomes zero.  */
        while (exp < 1 && x.d != 0)
          {
            x.d *= 0.5;
            exp++;
          }
      }
    return x.d;
  }
#else
  {
    double factor, r;

    factor = 2.0;
    if (exp < 0)
      {
        factor = 0.5;
        exp = -exp;
      }
    r = d;
    if (exp != 0)
      {
        if ((exp & 1) != 0)
          r *= factor;
        exp >>= 1;
        while (exp != 0)
          {
            factor *= factor;
            if ((exp & 1) != 0)
              r *= factor;
            exp >>= 1;
          }
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
mpfr_get_d2 (mpfr_srcptr src, long e)
{
  double res;
  mp_size_t size, i, n_limbs_to_use;
  mp_ptr qp;
  int negative;

  if (MPFR_IS_NAN(src))
    { 
#ifdef DEBUG
      printf("recognized NaN\n");
#endif
      return NaN;
    }

  if (MPFR_IS_INF(src))
    { 
#ifdef DEBUG
      printf("Found Inf.\n");
#endif
      return (MPFR_SIGN(src) == 1 ? Infp : Infm); 
    }

  if (MPFR_NOTZERO(src) == 0)
    return 0.0;

  size = 1+(MPFR_PREC(src)-1)/BITS_PER_MP_LIMB;
  qp = MPFR_MANT(src);
  negative = (MPFR_SIGN(src) < 0);

  /* Warning: don't compute the abs(res) and set the sign afterwards,
     otherwise the current machine rounding mode will not be taken
     correctly into account. */
  /* res = (negative) ? -(double)qp[size - 1] : qp[size - 1]; */
  res = 0.0;
  /* Warning: an arbitrary number of limbs may be required for an exact 
     rounding. The following code is correct but not optimal since one
     may be able to decide without considering all limbs. */
  /* n_limbs_to_use = MIN (MPFR_LIMBS_PER_DOUBLE, size); */
  n_limbs_to_use = size;
  /* Accumulate the limbs from less significant to most significant
     otherwise due to rounding we may accumulate several ulps,
     especially in rounding towards -/+infinity. */
  for (i = n_limbs_to_use; i>=1; i--) {
#if (BITS_PER_MP_LIMB == 32)
    res = res / MP_BASE_AS_DOUBLE +
      ((negative) ? -(double)qp[size - i] : qp[size - i]);
#else
#if (BITS_PER_MP_LIMB == 64)
    mp_limb_t q;
    q = qp[size - i] & CNST_LIMB(0xFFFFFFFF);
    res = res / MP_BASE_AS_DOUBLE + ((negative) ? -(double)q : q);
    q = qp[size - i] - q;
    res = res + ((negative) ? -(double)q : q);
#endif /* BITS_PER_MP_LIMB == 64 */
#endif /* BITS_PER_MP_LIMB == 32 */
  }
  res = __mpfr_scale2 (res, e - BITS_PER_MP_LIMB); 

  return res;
}

double 
mpfr_get_d (mpfr_srcptr src)
{
  return mpfr_get_d2 (src, MPFR_EXP(src));
}

