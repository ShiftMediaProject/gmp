/* mulmod_bnm1.c -- multiplication mod B^n-1.

   Contributed to the GNU project by Niels Möller, Torbjorn Granlund and
   Marco Bodrato.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY WILL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2009 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */


#include "gmp.h"
#include "gmp-impl.h"

#ifndef MULMOD_BNM1_THRESHOLD
#define MULMOD_BNM1_THRESHOLD 16
#endif

/* Inputs are {ap,rn} and {bp,rn}; output is {rp,rn}, computation is
   mod B^rn - 1, and values are semi-normalised; zero is represented
   as either 0 or B^n - 1.  Needs 2rn limbs at rp. */
static void
mpn_bc_mulmod_bnm1 (mp_ptr rp, mp_srcptr ap, mp_srcptr bp, mp_size_t rn)
{
  mp_limb_t cy;

  ASSERT (0 < rn);

  mpn_mul_n (rp, ap, bp, rn);
  cy = mpn_add_n (rp, rp, rp + rn, rn);
  /* If cy == 1, then the value of rp is at most B^rn - 2, so there can
   * be no overflow when adding in the carry. */
  MPN_INCR_U (rp, rn, cy);
}


/* Inputs are {ap,rn+1} and {bp,rn+1}; output is {rp,rn+1}, in
   semi-normalised representation, computation is mod B^rn + 1. Needs
   2rn + 2 limbs at rp. Output is normalised. */
static void
mpn_bc_mulmod_bnp1 (mp_ptr rp, mp_srcptr ap, mp_srcptr bp, mp_size_t rn)
{
  mp_limb_t cy;

  ASSERT (0 < rn);

  mpn_mul_n (rp, ap, bp, rn + 1);
  ASSERT( rp[2*rn+1] == 0);
  ASSERT( rp[2*rn] < GMP_NUMB_MAX);
  cy = rp[2*rn] + mpn_sub_n (rp, rp, rp+rn, rn);
  rp[rn] = 0;
  MPN_INCR_U (rp, rn+1, cy );
}


/* Computes {rp,rn} <- {ap,an}*{bp,bn} Mod(B^rn-1)
 * Requires both an and bn <= rn
 * Scratch need: rn + 2 + (need for recursive call OR rn + 2). This gives
 *
 * S(n) <= rn + 2 + MAX (rn + 2, S(n/2)) <= 2rn + 2 log2 rn + 2
 */
#define ALLOW_MISUSE 1
/* If we do not allow misuse, we assume two possible uses:
 * - rn, an, and bn are almost equal: more precisely an > rn/2 and
 *   bn > rn/2
 * - rn = mpn_mulmod_bnm1_next_size(an+bn) > MUL_FFT_MODF_THRESHOLD
 *   and an >= bn
 * Within this allowed uses we will never have an<rn when basecases
 * are needed.
 */
void
mpn_mulmod_bnm1 (mp_ptr rp, mp_size_t rn, mp_srcptr ap, mp_size_t an, mp_srcptr bp, mp_size_t bn, mp_ptr tp)
{
  ASSERT (0 < an && an <= rn);
  ASSERT (0 < bn && bn <= rn);
  ASSERT (0 < rn);

  if ((rn & 1) != 0 || BELOW_THRESHOLD (rn, MULMOD_BNM1_THRESHOLD))
    {
      if ( UNLIKELY(bn < rn) ) /* May happen only for misuse or _very_
				  unbalanced operands */
	{
	  MPN_COPY(tp, bp, bn);
	  MPN_ZERO(tp + bn, rn - bn);
	  bp = tp;
	}
      ASSERT ( ALLOW_MISUSE || (an >= rn) );
      if ( ALLOW_MISUSE && UNLIKELY(an < rn) )
	{
	  MPN_COPY(tp + rn, ap, an);
	  MPN_ZERO(tp + rn + an, rn - an);
	  ap = tp + rn;
	}
      mpn_bc_mulmod_bnm1 (rp, ap, bp, rn);
    }
  else
    {
      mp_size_t n;
      mp_limb_t cy;
      mp_limb_t hi;

      n = rn >> 1;

      /* Compute xm = a*b mod (B^n - 1), xp = a*b mod (B^n + 1)
	 and crt together as

	 x = xm + (B^n - 1) * [B^n/2 (xp - xm) mod (B^n+1)]
      */

#define a0 ap
#define a1 (ap + n)
#define b0 bp
#define b1 (bp + n)

#define xp  tp	/* 2n + 2 */
      /* am1  maybe in {xp, n} */
      /* bm1  maybe in {xp + n, n} */
#define so (tp + 2*n + 2)
      /* ap1  maybe in {so, n + 1} */
      /* bp1  maybe in {so + n + 1, n + 1} */

      {
	mp_srcptr am1, bm1;
	mp_size_t anm, bnm;

	if( an > n ) {
	  am1 = xp;
	  cy = mpn_add (xp, a0, n, a1, an - n);
	  MPN_INCR_U (xp, n, cy);
	  anm = n;
	} else {
	  am1 = a0;
	  anm = an;
	}

	if( bn > n ) {
	  bm1 = xp + n;
	  cy = mpn_add (xp + n, b0, n, b1, bn - n);
	  MPN_INCR_U (xp + n, n, cy);
	  bnm = n;
	} else {
	  bm1 = b0;
	  bnm = bn;
	}

	mpn_mulmod_bnm1 (rp, n, am1, anm, bm1, bnm, so);
      }

      {
	int       k;
	mp_srcptr ap1, bp1;
	mp_size_t anp, bnp;

	if( an > n ) {
	  ap1 = so;
	  cy = mpn_sub (so, a0, n, a1, an - n);
	  so[n] = 0;
	  MPN_INCR_U (so, n + 1, cy);
	  anp = n + ap1[n];
	} else {
	  ap1 = a0;
	  anp = an;
	}

	if( bn > n ) {
	  bp1 = so + n + 1;
	  cy = mpn_sub (so + n + 1, b0, n, b1, bn - n);
	  so[n+1+n] = 0;
	  MPN_INCR_U (so + n + 1, n + 1, cy);
	  bnp = n + bp1[n];
	} else {
	  bp1 = b0;
	  bnp = bn;
	}

	if (BELOW_THRESHOLD (n, MUL_FFT_MODF_THRESHOLD))
	  k=0;
	else
	  {
	    int mask;
	    k = mpn_fft_best_k (n, 0);
	    mask = (1<<k) -1;
	    while (n & mask) {k--; mask >>=1;};
	  }
	if (k >= FFT_FIRST_K)
	  xp[n] = mpn_mul_fft (xp, n, ap1, anp, bp1, bnp, k);
	else
	  {
	    if ( UNLIKELY(bp1 == b0) ) {
	      bp1 = so + n + 1;
	      MPN_COPY(so + n + 1, b0, bnp);
	      MPN_ZERO(so + n + 1 + bnp, n + 1 - bnp);
	    }
	    ASSERT ( ALLOW_MISUSE || ((an >= rn) && (ap1 != a0)) );
	    if ( ALLOW_MISUSE && UNLIKELY(ap1 == a0) ) {
	      ap1 = so;
	      MPN_COPY(so, a0, anp);
	      MPN_ZERO(so + anp, n + 1 - anp);
	    }
	    mpn_bc_mulmod_bnp1 (xp, ap1, bp1, n);
	  }
      }

      /* xp = xm - xp mod (B^n + 1). Assumes normalised
	 representation. Puts high bit in hi. */
      if (UNLIKELY (xp[n]))
	hi = mpn_add_1 (xp, rp, n, 1);
      else
	{
	  cy = mpn_sub_n (xp, rp, xp, n);
	  MPN_INCR_U (xp, n + 1 , cy);
	  hi = xp[n];
	}
      /* Multiply by -B^n/2, using

	 -B^n/2 * (2 x1 + x0) = x1 - B^n/2 x0 (mod (B^n + 1))
      */

      cy = mpn_rshift (rp+n, xp, n, 1);
      if (hi != cy)
	rp[2*n-1] |= GMP_LIMB_HIGHBIT;
      if (hi < cy)
	/* Underflow */
	hi = mpn_add_1 (rp+n, rp+n, n, 1);
      else
	hi = 0;

      cy = mpn_sub_n (rp, rp, rp+n, n);
      cy = mpn_sub_1 (rp+n, rp+n, n, cy + hi);
      ASSERT (cy == hi);
#undef a0
#undef a1
#undef b0
#undef b1
#undef xp
#undef so
    }
}

mp_size_t
mpn_mulmod_bnm1_next_size (mp_size_t n)
{
  if (BELOW_THRESHOLD (n,     MULMOD_BNM1_THRESHOLD))
    return n;
  if (BELOW_THRESHOLD (n, 2 * MULMOD_BNM1_THRESHOLD))
    return (n + (2-1)) & (-2);
  if (BELOW_THRESHOLD (n, 4 * MULMOD_BNM1_THRESHOLD))
    return (n + (4-1)) & (-4);
  if (BELOW_THRESHOLD (n, 8 * MULMOD_BNM1_THRESHOLD))
    return (n + (8-1)) & (-8);

  if (BELOW_THRESHOLD (n, MUL_FFT_MODF_THRESHOLD))
    return (n + (16-1)) & (-16);

  n = (n + 1) >> 1;
  return 2 * mpn_fft_next_size (n, mpn_fft_best_k (n, 0));
}
