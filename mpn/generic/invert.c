/* Compute {up,n}^(-1). Inversion using ApproximateReciprocal
   algorithm, returning either the correct result, or one less.

   Contributed to the GNU project by Marco Bodrato.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY WILL CHANGE OR DISAPPEAR IN A FUTURE GMP RELEASE.

Copyright (C) 2007, 2009 Free Software Foundation, Inc.

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

#define DEBUG 0
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#if DEBUG
#include <stdio.h>
int count = 0, cc = 0 ;
#endif

/* This is intended for constant THRESHOLDs only, where the compiler can
   completely fold the result.  */
#define LOG2C(n) \
 (((n) >=    0x1) + ((n) >=    0x2) + ((n) >=    0x4) + ((n) >=    0x8) + \
  ((n) >=   0x10) + ((n) >=   0x20) + ((n) >=   0x40) + ((n) >=   0x80) + \
  ((n) >=  0x100) + ((n) >=  0x200) + ((n) >=  0x400) + ((n) >=  0x800) + \
  ((n) >= 0x1000) + ((n) >= 0x2000) + ((n) >= 0x4000) + ((n) >= 0x8000))

#ifndef INV_MULMOD_BNM1_THRESHOLD
#define INV_MULMOD_BNM1_THRESHOLD (5*MULMOD_BNM1_THRESHOLD)
#endif

#ifndef INV_APPR_THRESHOLD
#define INV_APPR_THRESHOLD (0*INV_NEWTON_THRESHOLD)
#endif

#if TUNE_PROGRAM_BUILD
#define NPOWS \
 ((sizeof(mp_size_t) > 6 ? 48 : 8*sizeof(mp_size_t)))
#else
#define NPOWS \
 ((sizeof(mp_size_t) > 6 ? 48 : 8*sizeof(mp_size_t)) - LOG2C (INV_NEWTON_THRESHOLD))
#endif

static mp_size_t
mpn_invertappr_itch (mp_size_t n)
{
  return 3 * n + 2;
}

/* 
 Compute {ip,n}, the approximate reciprocal of the strictly normalised
 value {dp,n}, i.e. most significant bit must be set.

 Let e = mpn_invertappr (ip, dp, n, scratch), the following conditions
 are satisfied by the output:
   0 <= e <= 1;
   {dp,n}*(B^n+{ip,n}) < B^{2n} < {dp,n}*(B^n+{ip,n}+1+e).
 I.e. e=0 means that the result {ip,n} equals the one given by mpn_invert.
      e=1 means that the result may be one less than expected.

 Inspired by Algorithm "ApproximateReciprocal", published in
 "Modern Computer Arithmetic" by Richard P. Brent and Paul Zimmermann,
 algorithm 3.5, page 121 in version 0.4 of the book.

 Some adaptation were introduced, to allow product mod B^m-1 and
 return the value e.

 The iterative structure is copied from T.Granlund's binvert.c.

 FIXME: the scratch for mulmod_bnm1 does not currently fit in the
 scratch, it is allocated apart.

 With USE_MUL_N = 0 and WRAP_MOD_BNM1 = 0, the iteration is conformant
 to the algorithm described in the book.
 
 USE_MUL_N = 1 introduce a correction in such a way that "the value of
 B^{n+h}-T computed at step 8 cannot exceed B^n-1" (the book reads
 "2B^n-1"). This correction should not require to modify the proof.

 WRAP_MOD_BNM1 = 1 enables the wrapped product modulo B^m-1.
 NOTE: is there any normalisation problem for the [0] class?
 It shouldn't: we compute 2*|A*X_h - B^{n+h}| < B^m-1.
 We may get [0] if and only if we get AX_h = B^{n+h}.
 This can happen only if A=B^{n}/2, but this implies X_h = B^{h}*2-1
 i.e. AX_h = B^{n+h} - A, then we get into the "negative" branch,
 where X_h is not incremented (because A < B^n).

 Acknowledgements: Thanks to P. Zimmermann for many valuable suggestions.
*/

#define USE_MUL_N 1
#define WRAP_MOD_BNM1 1

static mp_limb_t
mpn_invertappr (mp_ptr ip, mp_srcptr dp, mp_size_t n, mp_ptr scratch)
{
  mp_limb_t cy;
  mp_ptr xp;
  mp_size_t rn, mn;
  mp_size_t sizes[NPOWS], *sizp;
  TMP_DECL;
#define rp scratch

  ASSERT (n > 0);
  ASSERT (dp[n-1] & GMP_LIMB_HIGHBIT);
  ASSERT (! MPN_OVERLAP_P (ip, n, dp, n));
  ASSERT (! MPN_OVERLAP_P (ip, n, scratch, mpn_invertappr_itch(n)));
  ASSERT (! MPN_OVERLAP_P (dp, n, scratch, mpn_invertappr_itch(n)));

  TMP_MARK;
  if (scratch == NULL)
    {
      scratch = TMP_ALLOC_LIMBS (mpn_invert_itch (n));
    }

  /* We search the inverse of 0.{dp,n}, we compute it as 1.{ip,n} */
  dp += n;
  ip += n;

  /* Compute the computation precisions from highest to lowest, leaving the
     base case size in 'rn'.  */
  sizp = sizes;
  rn = n;
  while (ABOVE_THRESHOLD (rn, INV_NEWTON_THRESHOLD)) {
    *sizp = rn;
    rn = ((rn) >> 1) + 1;
    sizp ++;
  }

  cy = 0;
  /* Compute a base value of rn limbs. */
  if (rn == 1)
    invert_limb (*(ip - 1),*(dp - 1));
  else { /* Maximum scratch needed by this branch: 3*n + 2 */
    mp_size_t i;
    xp = scratch + rn + 2;				/* 2 * rn limbs */
    for (i = rn - 1; i >= 0; i--)
      xp[i] = ~CNST_LIMB(0);
    mpn_com_n (xp + rn, dp - rn, rn);
    if (rn == 2) {
      mpn_tdiv_qr (rp, ip - rn, 0, xp, 2 * rn, dp - rn, rn);
      MPN_COPY (ip - rn, rp, rn);
    } else {
      gmp_pi1_t inv;
      invert_pi1 (inv, dp[-1], dp[-2]);
      if (BELOW_THRESHOLD (rn, DC_DIVAPPR_Q_THRESHOLD))
	mpn_sbpi1_divappr_q (ip - rn, xp, 2 * rn, dp - rn, rn, inv.inv32);
      else
	mpn_dcpi1_divappr_q (ip - rn, xp, 2 * rn, dp - rn, rn, &inv);
      MPN_DECR_U(ip - rn, rn, 1);
      cy = 1;
    }
  }

  /* Use Newton's iterations to get the desired precision.*/
  if (rn != n) {
    mp_ptr tp;
    if (WRAP_MOD_BNM1 && ABOVE_THRESHOLD (n, INV_MULMOD_BNM1_THRESHOLD)) 
      tp = TMP_ALLOC_LIMBS (mpn_mulmod_bnm1_itch (mpn_mulmod_bnm1_next_size (n + 1)));
    /* define rp scratch; 2rn + 1 limbs <= 2(n>>1 + 1) + 1 <= n + 3  limbs */
    /* Maximum scratch needed by this branch <= 3*n + 2	*/
    xp = scratch + n + 3;				/*  n + rn limbs */
    while (1) {
      mp_limb_t method;

      n = *--sizp;
      /*
	 v    n  v
	 +----+--+
	 ^ rn ^
      */

      /* Compute i_jd . */
      if (!WRAP_MOD_BNM1 || BELOW_THRESHOLD (n, INV_MULMOD_BNM1_THRESHOLD)
	  || ((mn = mpn_mulmod_bnm1_next_size (n + 1)) > (n + rn))) {
	/* FIXME: We do only need {xp,n+1}*/
	mpn_mul (xp, dp - n, n, ip - rn, rn);
	mpn_add_n (xp + rn, xp + rn, dp - n, n - rn + 1);
	method = 1; /* Remember we used (truncated) product */
	/* We computed cy.{xp,rn+n} <- 1.{ip,rn} * 0.{dp,n} */
      } else { /* Use B^n-1 wraparound */
	mpn_mulmod_bnm1 (xp, mn, dp - n, n, ip - rn, rn, tp);
	/* We computed {xp,mn} <- {ip,rn} * {dp,n} mod (B^mn-1) */
	/* We know that 2*|ip*dp + dp*B^rn - B^{rn+n}| < B^mn-1 */
	/* Add dp*B^rn mod (B^mn-1) */
	ASSERT (n >= mn - rn);
	xp[mn] = 1 + mpn_add_n (xp + rn, xp + rn, dp - n, mn - rn);
	cy = mpn_add_n (xp, xp, dp - (n - (mn - rn)), n - (mn - rn));
	MPN_INCR_U (xp + n - (mn - rn), mn + 1 - n + (mn - rn), cy);
	ASSERT (n + rn >=  mn);
	/* Subtract B^{rn+n}	*/
	MPN_DECR_U (xp + rn + n - mn, 2*mn + 1 - rn - n, 1);
	if (xp[mn])
	  MPN_INCR_U (xp, mn, xp[mn] - 1);
	else
	  MPN_DECR_U (xp, mn, 1);
	method = 0; /* Remember we are working Mod B^m-1 */
      }
#if WRAP_MOD_BNP1 && WANT_FFT && 0
      /* else */
      {
	ASSERT_ALWAYS(0); /* NOT IMPLEMENTED */
	/* Condition {xp,mn} = -1 Mod B^m+1 needs special handling...*/
	method = 2; /* Remember we are working Mod B^m+1 */
      }
#endif

      if (xp[n] < 2) { /* "positive" residue class */
	cy = 1;
	while (xp[n] || mpn_cmp (xp, dp - n, n)>0) {
	  xp[n] -= mpn_sub_n (xp, xp, dp - n, n);
	  cy ++;
	}
	MPN_DECR_U(ip - rn, rn, cy);
	ASSERT (cy <= 4); /* at most 3 cycles for the while above */
	ASSERT_NOCARRY (mpn_sub_n (xp, dp - n, xp, n));
	ASSERT (xp[n] == 0);
      } else { /* "negative" residue class */
	mpn_com_n (xp, xp, n + 1);
	MPN_INCR_U(xp, n + 1, method);
	ASSERT (xp[n] <= 1);
#if USE_MUL_N
	if (xp[n]) {
	  MPN_INCR_U(ip - rn, rn, 1);
	  ASSERT_CARRY (mpn_sub_n (xp, xp, dp - n, n));
	}
#endif
      }

      /* Compute x_ju_j. FIXME:We need {rp+rn,rn}, mulhi? */
#if USE_MUL_N
      mpn_mul_n (rp, xp + n - rn, ip - rn, rn);
#else
      rp[2*rn] = 0;
      mpn_mul (rp, xp + n - rn, rn + xp[n], ip - rn, rn);
#endif
      /* We need _only_ the carry from the next addition  */
      /* Anyway 2rn-n <= 2... we don't need to optimise.  */
      cy = mpn_add_n (rp + rn, rp + rn, xp + n - rn, 2*rn - n);
      cy = mpn_add_nc (ip - n, rp + 3*rn - n, xp + rn, n - rn, cy);
      MPN_INCR_U (ip - rn, rn, cy + (1-USE_MUL_N)*(rp[2*rn] + xp[n]));
      if (sizp == sizes) {
	cy = rp[3*rn - n - 1] > GMP_NUMB_MAX - 7;
/* 	cy = mpn_add_1 (rp + rn, rp + rn, 2*rn - n, 6); */
	break;
      }
      rn = n;
    }
  }

  TMP_FREE;
  return (cy);
#undef rp
}

mp_size_t
mpn_invert_itch (mp_size_t n)
{
  return mpn_invertappr_itch (n);
}

void
mpn_invert (mp_ptr ip, mp_srcptr dp, mp_size_t n, mp_ptr scratch)
{
  ASSERT (n > 0);
  ASSERT (dp[n-1] & GMP_LIMB_HIGHBIT);
  ASSERT (! MPN_OVERLAP_P (ip, n, dp, n));
  ASSERT (! MPN_OVERLAP_P (ip, n, scratch, mpn_invertappr_itch(n)));
  ASSERT (! MPN_OVERLAP_P (dp, n, scratch, mpn_invertappr_itch(n)));

  if (n == 1)
    invert_limb (*(ip),*(dp));
  else if (BELOW_THRESHOLD (n, INV_APPR_THRESHOLD)) {
    /* Maximum scratch needed by this branch: 3*n + 2 */
    mp_size_t i;
    mp_ptr xp;

    xp = scratch + n + 2;				/* 2 * n limbs */
    for (i = n - 1; i >= 0; i--)
      xp[i] = ~CNST_LIMB(0);
    mpn_com_n (xp + n, dp, n);
    mpn_tdiv_qr (scratch, ip, 0, xp, 2 * n, dp, n);
    MPN_COPY (ip, scratch, n);
  } else { /* Use approximated inverse; correct the result if needed. */
    mp_limb_t cy;

    cy = mpn_invertappr (ip, dp, n, scratch);

    if (cy) {
      /* Code to detects and correct the "off by one" approximation. */
      mpn_mul_n (scratch, ip, dp, n);
      ASSERT_NOCARRY (mpn_add_n (scratch + n, scratch + n, dp, n));
      if ( ! mpn_add (scratch, scratch, 2*n, dp, n)) {
#if DEBUG
	count ++;
	printf("? %d/%d ?\n", count, cc);
#endif
	MPN_INCR_U (ip, n, 1);
      }
#if DEBUG
      cc++;
#endif
    }
  }
}
