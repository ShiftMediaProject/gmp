/* mulmod_bnm1.c -- multiplication mod B^n-1.

   Contributed to the GNU project by Niels Möller.

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
#include "longlong.h"

#ifndef MULMOD_BNM1_THRESHOLD
#define MULMOD_BNM1_THRESHOLD 16
#endif

/* Inputs and outputs are n limbs, computation is mod B^n - 1, and
   values are semi-normalised; zero is represented as either 0 or B^n - 1.
   Needs 2n limbs at rp. */
static void
mpn_bc_mulmod_bnm1 (mp_ptr rp, mp_srcptr ap, mp_srcptr bp, mp_size_t n)
{
  mp_limb_t cy;
  mpn_mul_n (rp, ap, bp, n);
  cy = mpn_add_n (rp, rp, rp+n, n);
  /* If cy == 1, then the value of rp is at most B^n - 2, so there can
   * be no overflow when adding in the carry. */
  MPN_INCR_U (rp, n, cy);
}


/* Inputs and outputs are n + 1, in semi-normalised representation,
   computation is mod B^n + 1. Needs 2n + 2 limbs at rp. Output is
   normalised. */
static void
mpn_bc_mulmod_bnp1 (mp_ptr rp, mp_srcptr ap, mp_srcptr bp, mp_size_t n)
{
  mp_limb_t cy;
  mpn_mul_n (rp, ap, bp, n+1);
  ASSERT(rp[2*n+1] == 0);
  cy = mpn_sub_n (rp, rp, rp+n, n);
  rp[n] = 0;
  MPN_INCR_U (rp, n+1, cy + rp[2*n]);
}


/* Scratch need: 3*n + 4 + need for recursive call. This gives
 *
 * S(n) <= 3*n + 4 + S(n/2) <= 6n + c log n
 */
#define MUL_2NM1_DC_ITCH(n) (6*(n) + GMP_LIMB_BITS)

void
mpn_mulmod_bnm1 (mp_ptr rp, mp_srcptr ap, mp_srcptr bp, mp_size_t size, mp_ptr tp)
{
  if ((size & 1) != 0 || BELOW_THRESHOLD (size, MULMOD_BNM1_THRESHOLD))
    mpn_bc_mulmod_bnm1 (rp, ap, bp, size);
  else
    {
      mp_size_t n;
      mp_limb_t cy;
      mp_limb_t hi;

      n = size / 2;

      /* Compute xm = a*b mod (B^n - 1), xp = a*b mod (B^n + 1)
	 and crt together as

	 x = xm + (B^n - 1) * [B^n/2 (xp - xm) mod (B^n+1)]
      */

#define a0 ap
#define a1 (ap + n)
#define b0 bp
#define b1 (bp + n)

#define am1 tp		/* n */
#define bm1 (tp + n)	/* n */
#define ap1 (tp + 2*n)	/* n + 1 */
#define bp1 (tp + 3*n+1)	/* n + 1 */
#define xp (tp + 4*n + 2)	/* 2n + 2 */
#define scratch_out (tp + 6*n + 4)

      cy = mpn_add_n (am1, a0, a1, n);
      MPN_INCR_U (am1, n, cy);
      cy = mpn_sub_n (ap1, a0, a1, n);
      ap1[n] = mpn_add_1 (ap1, ap1, n, cy);

      cy = mpn_add_n (bm1, b0, b1, n);
      MPN_INCR_U (bm1, n, cy);
      cy = mpn_sub_n (bp1, b0, b1, n);
      bp1[n] = mpn_add_1 (bp1, bp1, n, cy);

      mpn_mulmod_bnm1 (rp, am1, bm1, n, scratch_out);
#if 0
      if (ABOVE_THRESHOLD (n, MUL_FFT_MODF_THRESHOLD))
	{
	  int k;
	  mp_size_t m;
	  k = mpn_fft_best_k (n, 0);
	  m = mpn_fft_next_size (n, k);

#if 1
	  if (ap1[n] != 0)
	    {
	      MPN_DECR_U (ap1, n + 1, CNST_LIMB(1));
	      if (ap1[n] == 0)
		{
		  MPN_ZERO (ap1, n);
		  ap1[n] = 1;
		}
	      else
		ap1[n] = 0;
	    }

	  if (bp1[n] != 0)
	    {
	      MPN_DECR_U (bp1, n + 1, CNST_LIMB(1));
	      if (bp1[n] == 0)
		{
		  MPN_ZERO (bp1, n);
		  bp1[n] = 1;
		}
	      else
		bp1[n] = 0;
	    }
#endif

	  if (m == n && ((ap1[n] | bp1[n]) == 0))
	    {
	      mpn_mul_fft (xp, m, ap1, n, bp1, n, k);
	    }
	  else
	    mpn_bc_mulmod_bnp1 (xp, ap1, bp1, n);
	}
      else
#endif
	mpn_bc_mulmod_bnp1 (xp, ap1, bp1, n);


      /* xp = xm - xp mod (B^n + 1). Assumes normalised
	 representation. Puts high bit in hi. */
      if (UNLIKELY (xp[n]))
	hi = mpn_add_1 (xp, rp, n, 1);
      else
	{
	  cy = mpn_sub_n (xp, rp, xp, n);
	  hi = mpn_add_1 (xp, xp, n, cy);
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
#undef am1
#undef bm1
#undef ap1
#undef bp1
#undef xp
#undef scratch_out
    }
}

mp_size_t
mpn_mulmod_bnm1_next_size (mp_size_t n)
{
  return n + (-n & 0xf);
}
