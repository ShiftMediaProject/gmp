/* sqrmod_bnm1.c -- squaring mod B^n-1.

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

/* Input is {ap,rn}; output is {rp,rn}, computation is
   mod B^rn - 1, and values are semi-normalised; zero is represented
   as either 0 or B^n - 1.  Needs a scratch of 2rn limbs at tp.
   tp==rp is allowed. */
static void
mpn_bc_sqrmod_bnm1 (mp_ptr rp, mp_srcptr ap, mp_size_t rn, mp_ptr tp)
{
  mp_limb_t cy;

  ASSERT (0 < rn);

  mpn_sqr_n (tp, ap, rn);
  cy = mpn_add_n (rp, tp, tp + rn, rn);
  /* If cy == 1, then the value of rp is at most B^rn - 2, so there can
   * be no overflow when adding in the carry. */
  MPN_INCR_U (rp, rn, cy);
}


/* Input is {ap,rn+1}; output is {rp,rn+1}, in
   semi-normalised representation, computation is mod B^rn + 1. Needs
   a scratch area of 2rn + 2 limbs at tp; tp == rp is allowed.
   Output is normalised. */
static void
mpn_bc_sqrmod_bnp1 (mp_ptr rp, mp_srcptr ap, mp_size_t rn, mp_ptr tp)
{
  mp_limb_t cy;

  ASSERT (0 < rn);

  mpn_sqr_n (tp, ap, rn + 1);
  ASSERT (tp[2*rn+1] == 0);
  ASSERT (tp[2*rn] < GMP_NUMB_MAX);
  cy = tp[2*rn] + mpn_sub_n (rp, tp, tp+rn, rn);
  rp[rn] = 0;
  MPN_INCR_U (rp, rn+1, cy );
}


/* Computes {rp,rn} <- {ap,an}^2 Mod(B^rn-1)
 *
 * The result is expected to be ZERO if and only if the operand
 * already is. Otherwise the class [0] Mod(B^rn-1) is represented by
 * B^rn-1.
 * Moreover it should not be a problem if sqrmod_bnm1 is used to
 * compute the full square with an <= 2*rn, because this condition
 * implies (B^an-1)^2 < (B^rn-1) .
 *
 * Requires 0 < an <= rn
 * Scratch need: rn + 2 + (need for recursive call OR rn + 2). This gives
 *
 * S(n) <= rn + 2 + MAX (rn + 2, S(n/2)) <= 2rn + 2 log2 rn + 2
 */
void
mpn_sqrmod_bnm1 (mp_ptr rp, mp_size_t rn, mp_srcptr ap, mp_size_t an, mp_ptr tp)
{
  ASSERT (0 < an);
  ASSERT (an <= rn);

  if ((rn & 1) != 0 || BELOW_THRESHOLD (rn, SQRMOD_BNM1_THRESHOLD))
    {
      if (UNLIKELY (an < rn))
	{
	  if (UNLIKELY (2*an <= rn))
	    {
	      mpn_sqr_n (rp, ap, an);
	      MPN_ZERO (rp + 2*an, rn - 2*an);
	    }
	  else
	    {
	      mp_limb_t cy;
	      mpn_sqr_n (tp, ap, an);
	      cy = mpn_add (rp, tp, rn, tp + rn, 2*an - rn);
	      MPN_INCR_U (rp, rn, cy);
	    }
	}
      else
	mpn_bc_sqrmod_bnm1 (rp, ap, rn, tp);
    }
  else
    {
      mp_size_t n;
      mp_limb_t cy;
      mp_limb_t hi;

      n = rn >> 1;

      /* Compute xm = a^2 mod (B^n - 1), xp = a^2 mod (B^n + 1)
	 and crt together as

	 x = xm + (B^n - 1) * [B^n/2 (xp - xm) mod (B^n+1)]
      */

#define a0 ap
#define a1 (ap + n)

#define xp  tp	/* 2n + 2 */
      /* am1  maybe in {xp, n} */
#define so (tp + 2*n + 2)
      /* ap1  maybe in {so, n + 1} */

      {
	mp_srcptr am1;
	mp_size_t anm;

	if( an > n ) {
	  am1 = xp;
	  cy = mpn_add (xp, a0, n, a1, an - n);
	  MPN_INCR_U (xp, n, cy);
	  anm = n;
	} else {
	  am1 = a0;
	  anm = an;
	}

	mpn_sqrmod_bnm1 (rp, n, am1, anm, so);
      }

      {
	int       k;
	mp_srcptr ap1;
	mp_size_t anp;

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

	if (BELOW_THRESHOLD (n, MUL_FFT_MODF_THRESHOLD))
	  k=0;
	else
	  {
	    int mask;
	    k = mpn_fft_best_k (n, 1);
	    mask = (1<<k) -1;
	    while (n & mask) {k--; mask >>=1;};
	  }
	if (k >= FFT_FIRST_K)
	  xp[n] = mpn_mul_fft (xp, n, ap1, anp, ap1, anp, k);
	else
	  {
	    if (UNLIKELY (ap1 == a0)) {
	      ap1 = so;
	      MPN_COPY (so, a0, anp);
	      MPN_ZERO (so + anp, n + 1 - anp);
	    }
	    mpn_bc_sqrmod_bnp1 (xp, ap1, n, xp);
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
#undef xp
#undef so
    }
}
