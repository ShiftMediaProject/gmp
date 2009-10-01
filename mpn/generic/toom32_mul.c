/* mpn_toom32_mul -- Multiply {ap,an} and {bp,bn} where an is nominally 1.5
   times as large as bn.  Or more accurately, bn < an < 3bn.

   Contributed to the GNU project by Torbjorn Granlund.
   Improvements by Marco Bodrato.

   The idea of applying toom to unbalanced multiplication is due to Marco
   Bodrato and Alberto Zanoni.

   THE FUNCTION IN THIS FILE IS INTERNAL WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH IT THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT IT WILL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2006, 2007, 2008, 2009 Free Software Foundation, Inc.

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

/* Evaluate in: -1, 0, +1, +inf

  <-s-><--n--><--n-->
   ___ ______ ______
  |a2_|___a1_|___a0_|
	|_b1_|___b0_|
	<-t--><--n-->

  v0  =  a0         * b0      #   A(0)*B(0)
  v1  = (a0+ a1+ a2)*(b0+ b1) #   A(1)*B(1)      ah  <= 2  bh <= 1
  vm1 = (a0- a1+ a2)*(b0- b1) #  A(-1)*B(-1)    |ah| <= 1  bh = 0
  vinf=          a2 *     b1  # A(inf)*B(inf)
*/

#define TOOM32_MUL_N_REC(p, a, b, n, ws)				\
  do {									\
    mpn_mul_n (p, a, b, n);						\
  } while (0)

void
mpn_toom32_mul (mp_ptr pp,
		mp_srcptr ap, mp_size_t an,
		mp_srcptr bp, mp_size_t bn,
		mp_ptr scratch)
{
  mp_size_t n, s, t;
  int vm1_neg;
  mp_limb_t cy;

#define a0  ap
#define a1  (ap + n)
#define a2  (ap + 2 * n)
#define b0  bp
#define b1  (bp + n)

  n = 1 + (2 * an >= 3 * bn ? (an - 1) / (size_t) 3 : (bn - 1) >> 1);

  s = an - 2 * n;
  t = bn - n;

  ASSERT (0 < s && s <= n);
  ASSERT (0 < t && t <= n);

#define as1   (pp + n + 1)			/* n+1 */
#define asm1  (scratch + n)			/* n+1 */
#define bs1   (pp)				/* n+1 */
#define bsm1  (scratch)			/* n */
#define a0_a2 (scratch)				/* n */

  /* Compute as1 and asm1.  */
  asm1[n] = mpn_add (a0_a2, a0, n, a2, s);
#if HAVE_NATIVE_mpn_addsub_n
  if (asm1[n] == 0 && mpn_cmp (a0_a2, a1, n) < 0)
    {
      cy = mpn_addsub_n (as1, asm1, a1, a0_a2, n);
      as1[n] = cy >> 1;
      vm1_neg = 1;
    }
  else
    {
      cy = mpn_addsub_n (as1, asm1, a0_a2, a1, n);
      as1[n] = asm1[n] + (cy >> 1);
      asm1[n]-= cy & 1;
      vm1_neg = 0;
    }
#else
  as1[n] = asm1[n] + mpn_add_n (as1, a0_a2, a1, n);
  if (asm1[n] == 0 && mpn_cmp (a0_a2, a1, n) < 0)
    {
      mpn_sub_n (asm1, a1, a0_a2, n);
      vm1_neg = 1;
    }
  else
    {
      cy = mpn_sub_n (asm1, a0_a2, a1, n);
      asm1[n]-= cy;
      vm1_neg = 0;
    }
#endif

  /* Compute bs1 and bsm1.  */
  if (t == n)
    {
#if HAVE_NATIVE_mpn_addsub_n
      if (mpn_cmp (b0, b1, n) < 0)
	{
	  cy = mpn_addsub_n (bs1, bsm1, b1, b0, n);
	  vm1_neg ^= 1;
	}
      else
	{
	  cy = mpn_addsub_n (bs1, bsm1, b0, b1, n);
	}
      bs1[n] = cy >> 1;
#else
      bs1[n] = mpn_add_n (bs1, b0, b1, n);

      if (mpn_cmp (b0, b1, n) < 0)
	{
	  mpn_sub_n (bsm1, b1, b0, n);
	  vm1_neg ^= 1;
	}
      else
	{
	  mpn_sub_n (bsm1, b0, b1, n);
	}
#endif
    }
  else
    {
      bs1[n] = mpn_add (bs1, b0, n, b1, t);

      if (mpn_zero_p (b0 + t, n - t) && mpn_cmp (b0, b1, t) < 0)
	{
	  mpn_sub_n (bsm1, b1, b0, t);
	  MPN_ZERO (bsm1 + t, n - t);
	  vm1_neg ^= 1;
	}
      else
	{
	  mpn_sub (bsm1, b0, n, b1, t);
	}
    }

  ASSERT (as1[n] <= 2);
  ASSERT (bs1[n] <= 1);
  ASSERT (asm1[n] <= 1);
/*ASSERT (bsm1[n] == 0); */

#define v0    pp				/* 2n */
#define v1    (scratch)				/* 2n+1 */
#define vinf  (pp + 3 * n)			/* s+t */
#define vm1   (scratch + 2 * n + 1)		/* 2n+1 */
#define scratch_out	scratch + 4 * n + 2

  /* vm1, 2n+1 limbs */
  TOOM32_MUL_N_REC (vm1, asm1, bsm1, n, scratch_out);
  cy = 0;
  if (asm1[n] != 0)
    cy = mpn_add_n (vm1 + n, vm1 + n, bsm1, n);
  vm1[2 * n] = cy;

  /* v1, 2n+1 limbs */
  TOOM32_MUL_N_REC (v1, as1, bs1, n, scratch_out);
  if (as1[n] == 1)
    {
      cy = bs1[n] + mpn_add_n (v1 + n, v1 + n, bs1, n);
    }
  else if (as1[n] == 2)
    {
#if HAVE_NATIVE_mpn_addlsh1_n
      cy = 2 * bs1[n] + mpn_addlsh1_n (v1 + n, v1 + n, bs1, n);
#else
      cy = 2 * bs1[n] + mpn_addmul_1 (v1 + n, bs1, n, CNST_LIMB(2));
#endif
    }
  else
    cy = 0;
  if (bs1[n] != 0)
    cy += mpn_add_n (v1 + n, v1 + n, as1, n);
  v1[2 * n] = cy;

  /* vinf, s+t limbs.  Use mpn_mul for now, to handle unbalanced operands */
  if (s > t)  mpn_mul (vinf, a2, s, b1, t);
  else        mpn_mul (vinf, b1, t, a2, s);

  /* v0, 2n limbs */
  TOOM32_MUL_N_REC (v0, ap, bp, n, scratch_out);

  /* Interpolate */

  if (vm1_neg)
    {
#if HAVE_NATIVE_mpn_rsh1add_n
      mpn_rsh1add_n (vm1, v1, vm1, 2 * n + 1);
#else
      mpn_add_n (vm1, v1, vm1, 2 * n + 1);
      mpn_rshift (vm1, vm1, 2 * n + 1, 1);
#endif
    }
  else
    {
#if HAVE_NATIVE_mpn_rsh1sub_n
      mpn_rsh1sub_n (vm1, v1, vm1, 2 * n + 1);
#else
      mpn_sub_n (vm1, v1, vm1, 2 * n + 1);
      mpn_rshift (vm1, vm1, 2 * n + 1, 1);
#endif
    }

  t += s;   /* limbs in vinf */
  if (LIKELY(t>=n)) /* We should consider this branch only, even better t>n */
    s = n;  /* limbs in Lvinf */
  else {
    s = t;  /* limbs in Lvinf */
    v1[2 * n]=0;
  }

  mpn_sub_n (v1, v1, vm1, s + n + 1);

  /*
    pp[] prior to operations:
     |_H vinf|_L vinf|_______|_H v0__|_L v0__|

    summation scheme for remaining operations:
     |______4|n_____3|n_____2|n______|n______|pp
     |_Hvinf_|_L*vinf|       |_H*v0__|_L v0__|
		    || H vm1 | L vm1 |
		     |-H vinf|-L*vinf|
	    || H v1  | L v1  |
	     |-H*v0  |-L v0  |

    We will avoid double computation of Hv0-Lvinf. Be careful! This
    operation can give a negative result!
  */

  cy = mpn_sub_n (v0 + n, v0 + n, vinf, s); /* Hv0-Lvinf*/
  v1[ n + s ] += cy - mpn_sub_n(pp + 2 * n, v1, v0, n + s); /* v1-v0+Lvinf */
  cy -= mpn_add_n (pp + n, pp + n, vm1, s); /* (Hv0-Lvinf)+Lvm1 */
  if (cy != 0) { /* carry and borrow did not cancel one another, apply the right one */
    if (cy == 1)
      MPN_DECR_U (vm1 + s, 2 * n + 1 - s, 1);
    else /* (cy == -1) */
      MPN_INCR_U (vm1 + s, 2 * n + 1 - s, 1);
  }
  if (LIKELY(t>n)) { /* It works also without this "if", t<=n implies t-s==0,v1[2*n]==0 */
    mpn_sub (vm1 + n, vm1 + n, n + 1, vinf + n, t - s); /* Hvm1-Hvinf */
    MPN_INCR_U (vinf + s, t - s, v1[2 * n]);
  }
  cy = mpn_add_n (pp + n + s, pp + n + s, vm1 + s, 2 * n + 1 - s);
  MPN_INCR_U (vinf + 1, t - 1, cy);

#undef a0
#undef a1
#undef a2
#undef b0
#undef b1
#undef v0
#undef v1
#undef vinf
#undef vm1
#undef a0_a2
#undef bsm1
#undef asm1
#undef bs1
#undef as1
}
