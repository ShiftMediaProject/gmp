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
  mp_limb_t *tmp;
  mp_limb_t b1;
  mp_prec_t aq, bq, cq;
  mp_size_t an, bn, cn, tn, k;
  TMP_DECL(marker);

  /* deal with special cases */
  if (MPFR_ARE_SINGULAR(b,c))
    {
      if (MPFR_IS_NAN(b) || MPFR_IS_NAN(c))
	{
	  MPFR_SET_NAN(a);
	  MPFR_RET_NAN;
	}
      sign_product = MPFR_MULT_SIGN( MPFR_SIGN(b) , MPFR_SIGN(c) );
      if (MPFR_IS_INF(b))
	{
	  if (MPFR_IS_INF(c) || MPFR_NOTZERO(c))
	    {
	      MPFR_SET_SIGN(a,sign_product);
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
	      MPFR_SET_SIGN(a, sign_product);
	      MPFR_SET_INF(a);
	      MPFR_RET(0); /* exact */
	    }
	  else
	    {
	      MPFR_SET_NAN(a);
	      MPFR_RET_NAN;
	    }
	}
      else if (MPFR_IS_ZERO(b) || MPFR_IS_ZERO(c))
	{
	  MPFR_SET_SIGN(a, sign_product);
	  MPFR_SET_ZERO(a);
	  MPFR_RET(0); /* 0 * 0 is exact */
	}
      /* Should never reach here */
      MPFR_ASSERTN(0);
    }
  MPFR_CLEAR_FLAGS(a);
  sign_product = MPFR_MULT_SIGN( MPFR_SIGN(b) , MPFR_SIGN(c) );
 
  bx = MPFR_GET_EXP (b);
  cx = MPFR_GET_EXP (c);
  /* Note: the exponent of the exact result will be e = bx + cx + ec with
     ec in {-1,0,1} and the following assumes that e is representable. */
  /* These ASSERT should be always true */
  MPFR_ASSERTN(MPFR_EMAX_MAX <= (MPFR_EXP_MAX >> 1));
  MPFR_ASSERTN(MPFR_EMIN_MIN >= -(MPFR_EXP_MAX >> 1));

  /* FIXME: Usefull since we do an exponent check after ?
   * It is usefull iff the precision is big, there is an overflow
   * and we are doing further mults...*/
#ifdef HUGE
  if (MPFR_UNLIKELY(bx + cx > __gmpfr_emax + 1))
    return mpfr_set_overflow (a, rnd_mode, sign_product);
  if (MPFR_UNLIKELY(bx + cx < __gmpfr_emin - 2))
  return mpfr_set_underflow (a, rnd_mode == GMP_RNDN ? GMP_RNDZ : rnd_mode,
			     sign_product);
#endif

  aq = MPFR_PREC(a);
  bq = MPFR_PREC(b);
  cq = MPFR_PREC(c);

  /* Set the sign: can't be done before to let the multiply done */
  MPFR_SET_SIGN(a, sign_product);
  
  MPFR_ASSERTD(bq+cq > bq); /* PREC_MAX is /2 so no integer overflow */
 
  an = (aq+BITS_PER_MP_LIMB-1)/BITS_PER_MP_LIMB; /* number of limbs of a */
  bn = (bq+BITS_PER_MP_LIMB-1)/BITS_PER_MP_LIMB; /* number of limbs of b */
  cn = (cq+BITS_PER_MP_LIMB-1)/BITS_PER_MP_LIMB; /* number of limbs of c */
  k = bn + cn; /* effective nb of limbs used by b*c (= tn or tn+1) below */
  tn = (bq + cq + BITS_PER_MP_LIMB - 1) / BITS_PER_MP_LIMB; 
  /* <= k, thus no int overflow */
  MPFR_ASSERTD(tn <= k);

  /* Check for no size_t overflow*/
  MPFR_ASSERTD((size_t) k <= ((size_t) ~0) / BYTES_PER_MP_LIMB);
  TMP_MARK(marker); 
  tmp = (mp_limb_t *) TMP_ALLOC((size_t) k * BYTES_PER_MP_LIMB);

  /* multiplies two mantissa in temporary allocated space */
  b1 = (MPFR_LIKELY(bn >= cn)) ? 
    mpn_mul (tmp, MPFR_MANT(b), bn, MPFR_MANT(c), cn)
    : mpn_mul (tmp, MPFR_MANT(c), cn, MPFR_MANT(b), bn);

  /* now tmp[0]..tmp[k-1] contains the product of both mantissa,
     with tmp[k-1]>=2^(BITS_PER_MP_LIMB-2) */
  b1 >>= BITS_PER_MP_LIMB - 1; /* msb from the product */

  tmp += k - tn;
  if (MPFR_UNLIKELY(b1 == 0))
    mpn_lshift (tmp, tmp, tn, 1);
  cc = mpfr_round_raw (MPFR_MANT(a), tmp, bq + cq, 
		       MPFR_IS_NEG_SIGN(sign_product), aq, rnd_mode, &inexact);
  if (MPFR_UNLIKELY(cc)) /* cc = 1 ==> result is a power of two */
    MPFR_MANT(a)[an-1] = MPFR_LIMB_HIGHBIT;

  TMP_FREE(marker);

  ax = (bx + cx) + (mp_exp_t) (b1 - 1 + cc);
  if (MPFR_UNLIKELY( ax > __gmpfr_emax))
    return mpfr_set_overflow (a, rnd_mode, sign_product);
  if (MPFR_UNLIKELY( ax < __gmpfr_emin))
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

  return inexact;
}
