/* mpfr_mul -- multiply two floating-point numbers

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_mul (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, mp_rnd_t rnd_mode) 
{
  int sign_product, cc, inexact;
  mp_exp_t ax, bx, cx;
  mp_limb_t *ap, *bp, *cp, *tmp;
  mp_limb_t b1;
  mp_prec_t aq, bq, cq;
  mp_size_t an, bn, cn, tn, k;
  TMP_DECL(marker);

  /* deal with NaN and zero */
  if (MPFR_IS_NAN(b) || MPFR_IS_NAN(c))
    {
      MPFR_SET_NAN(a);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(a);

  sign_product = MPFR_SIGN(b) * MPFR_SIGN(c);

  if (MPFR_IS_INF(b))
    {
      if (MPFR_IS_INF(c) || MPFR_NOTZERO(c))
        {
          if (MPFR_SIGN(a) != sign_product)
            MPFR_CHANGE_SIGN(a);
          MPFR_SET_INF(a);
          MPFR_RET(0); /* exact */
        }
      else
	{
	  MPFR_SET_NAN(a);
          MPFR_RET_NAN;
	}
    }
  else if (MPFR_IS_INF(c))
    {
      if (MPFR_NOTZERO(b))
        {
          if (MPFR_SIGN(a) != sign_product)
            MPFR_CHANGE_SIGN(a);
          MPFR_SET_INF(a);
          MPFR_RET(0); /* exact */
        }
      else
	{
	  MPFR_SET_NAN(a);
          MPFR_RET_NAN;
	}
    }

  MPFR_ASSERTN(MPFR_IS_FP(b) && MPFR_IS_FP(c));
  MPFR_CLEAR_INF(a); /* clear Inf flag */

  if (MPFR_IS_ZERO(b) || MPFR_IS_ZERO(c))
    {
      if (MPFR_SIGN(a) != sign_product)
        MPFR_CHANGE_SIGN(a);
      MPFR_SET_ZERO(a);
      MPFR_RET(0); /* 0 * 0 is exact */
    }

  bx = MPFR_GET_EXP (b);
  cx = MPFR_GET_EXP (c);
  /* Note: the exponent of the exact result will be e = bx + cx + ec with
     ec in {-1,0,1} and the following assumes that e is representable. */
  MPFR_ASSERTN(MPFR_EMAX_MAX <= (MP_EXP_T_MAX >> 1));
  MPFR_ASSERTN(MPFR_EMIN_MIN >= -(MP_EXP_T_MAX >> 1));
  if (bx + cx > __gmpfr_emax + 1)
    return mpfr_set_overflow (a, rnd_mode, sign_product);
  if (bx + cx < __gmpfr_emin - 2)
    return mpfr_set_underflow (a, rnd_mode == GMP_RNDN ? GMP_RNDZ : rnd_mode,
                               sign_product);

  ap = MPFR_MANT(a);
  bp = MPFR_MANT(b);
  cp = MPFR_MANT(c);

  aq = MPFR_PREC(a);
  bq = MPFR_PREC(b);
  cq = MPFR_PREC(c);

  an = (aq-1)/BITS_PER_MP_LIMB + 1; /* number of significant limbs of a */
  bn = (bq-1)/BITS_PER_MP_LIMB + 1; /* number of significant limbs of b */
  cn = (cq-1)/BITS_PER_MP_LIMB + 1; /* number of significant limbs of c */

  MPFR_ASSERTN((mp_size_unsigned_t) bn + cn <= MP_SIZE_T_MAX);
  k = bn + cn; /* effective nb of limbs used by b*c (= tn or tn+1) below */

  MPFR_ASSERTN(bq + cq >= bq); /* no integer overflow */
  tn = (bq + cq - 1) / BITS_PER_MP_LIMB + 1; /* <= k, thus no int overflow */

  MPFR_ASSERTN(k <= ((size_t) -1) / BYTES_PER_MP_LIMB);
  TMP_MARK(marker); 
  tmp = (mp_limb_t *) TMP_ALLOC((size_t) k * BYTES_PER_MP_LIMB);

  /* multiplies two mantissa in temporary allocated space */
  b1 = (bn >= cn) ? mpn_mul (tmp, bp, bn, cp, cn)
    : mpn_mul (tmp, cp, cn, bp, bn);

  /* now tmp[0]..tmp[k-1] contains the product of both mantissa,
     with tmp[k-1]>=2^(BITS_PER_MP_LIMB-2) */
  b1 >>= BITS_PER_MP_LIMB - 1; /* msb from the product */

  tmp += k - tn;
  if (b1 == 0)
    mpn_lshift (tmp, tmp, tn, 1);
  cc = mpfr_round_raw (ap, tmp, bq + cq, sign_product < 0, aq,
		       rnd_mode, &inexact);
  if (cc) /* cc = 1 ==> result is a power of two */
    ap[an-1] = MPFR_LIMB_HIGHBIT;

  TMP_FREE(marker);

  ax = (bx + cx) + (mp_exp_t) (b1 - 1 + cc);
  if (ax > __gmpfr_emax)
    return mpfr_set_overflow (a, rnd_mode, sign_product);
  if (ax < __gmpfr_emin)
    {
      /* In the rounding to the nearest mode, if the exponent of the exact
         result (i.e. before rounding, i.e. without taking cc into account)
         is < __gmpfr_emin - 1 or the exact result is a power of 2 (i.e. if
         both arguments are powers of 2), then round to zero. */
      if (rnd_mode == GMP_RNDN &&
          ((bx + cx) + (mp_exp_t) b1 < __gmpfr_emin ||
           (mpfr_powerof2_raw (b) && mpfr_powerof2_raw (c))))
        rnd_mode = GMP_RNDZ;
      return mpfr_set_underflow (a, rnd_mode, sign_product);
    }
  MPFR_SET_EXP (a, ax);

  if (MPFR_SIGN(a) != sign_product)
    MPFR_CHANGE_SIGN(a);

  return inexact;
}
