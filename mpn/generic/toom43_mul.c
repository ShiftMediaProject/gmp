/* mpn_toom43_mul -- Multiply {ap,an} and {bp,bn} where an is nominally 4/3
   times as large as bn.  Or more accurately, bn < an < 2 bn.

   Contributed to the GNU project by Marco Bodrato.

   The idea of applying toom to unbalanced multiplication is due to Marco
   Bodrato and Alberto Zanoni.

   THE FUNCTION IN THIS FILE IS INTERNAL WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH IT THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT IT WILL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

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

/* Evaluate in: -2, -1, 0, +1, +2, +inf

  <-s-><--n--><--n--><--n-->
   ___ ______ ______ ______
  |a3_|___a2_|___a1_|___a0_|
	|_b2_|___b1_|___b0_|
	<-t--><--n--><--n-->

  v0  =  a0             * b0          #   A(0)*B(0)
  v1  = (a0+ a1+ a2+ a3)*(b0+ b1+ b2) #   A(1)*B(1)      ah  <= 3  bh <= 2
  vm1 = (a0- a1+ a2- a3)*(b0- b1+ b2) #  A(-1)*B(-1)    |ah| <= 1 |bh|<= 1
  v2  = (a0+2a1+4a2+8a3)*(b0+2b1+4b2) #   A(2)*B(2)      ah  <= 14 bh <= 6
  vm2 = (a0-2a1+4a2-8a3)*(b0-2b1+4b2) #  A(-2)*B(-2)    |ah| <= 9 |bh|<= 4
  vinf=              a3 *         b2  # A(inf)*B(inf)
*/

void
mpn_toom43_mul (mp_ptr pp,
		mp_srcptr ap, mp_size_t an,
		mp_srcptr bp, mp_size_t bn, mp_ptr scratch)
{
  mp_size_t n, s, t;
  enum toom6_flags flags = toom6_all_pos;
  mp_limb_t cy;
  mp_ptr as1, as2;
/*   mp_ptr as1, asm1, as2, asm2; */
/*   mp_ptr bs1, bsm1, bs2, bsm2; */
  TMP_DECL;

#define a0  ap
#define a1  (ap + n)
#define a2  (ap + 2 * n)
#define a3  (ap + 3 * n)
#define b0  bp
#define b1  (bp + n)
#define b2  (bp + 2 * n)

  n = 1 + (3 * an >= 4 * bn ? (an - 1) >> 2 : (bn - 1) / (unsigned long) 3);

  s = an - 3 * n;
  t = bn - 2 * n;

  ASSERT (0 < s && s <= n);
  ASSERT (0 < t && t <= n);

  TMP_MARK;

#define v0    pp				/* 2n */
#define vm1   (scratch)				/* 2n+1 */
#define v1    (pp + 2*n)			/* 2n+1 */
#define vm2   (scratch + 2 * n + 1)		/* 2n+1 */
#define v2    (scratch + 4 * n + 2)		/* 2n+1 */
#define vinf  (pp + 5 * n)			/* s+t */
#define bs1    pp				/* n+1 */
#define bsm1  (scratch + 2 * n + 2)		/* n+1 */
#define asm1  (scratch + 3 * n + 3)		/* n+1 */
#define asm2  (scratch + 4 * n + 4)		/* n+1 */
#define bsm2  (pp + n + 1)			/* n+1 */
#define bs2   (pp + 2 * n + 2)			/* n+1 */

 /* Alloc one more byte, because products will overwrite 2n+2
    limbs. If n=1 asm2 reaches scratch+5n+5=scratch+10 and the last
    byte is used anyway. */
  scratch = TMP_SALLOC_LIMBS (6 * n + 3 + 1);

  /* #define as2   (pp + 3 * n + 3)			/\* n+1 *\/ */
  /* n+(n-2)<=n+(s+t), so the next test is always false if n>2.
     Consider replacing it with the above define if threshold is big
     enough.
  */
  if (n+s+t < 4)
    as2 = TMP_SALLOC_LIMBS (n + 1);
  else
    as2 = (pp + 3 * n + 3);

  /* #define as1   (pp + 4 * n + 4)			/\* n+1 *\/ */
  /* (n-2)<=(s+t), so the next test is always false if n>6.
     Consider replacing it with the above define if threshold is big
     enough.
  */
  if (s+t < 5)
    as1 = TMP_SALLOC_LIMBS (n + 1);
  else
    as1 = (pp + 4 * n + 4);

/*   bsm1 = TMP_SALLOC_LIMBS (n + 1); */
/*   bs2 = TMP_SALLOC_LIMBS (n + 1); */
/*   bsm2 = TMP_SALLOC_LIMBS (n + 1); */

#define a0a2  scratch
#define b0b2  scratch
#define a1a3  asm1
#define b1d   bsm1

  /* Compute as2 and asm2.  */
  cy = mpn_lshift (asm2, a3, s, 3);			/* 8a3                */
  a1a3[n] = mpn_lshift (a1a3, a1, n, 1);		/*           2a1      */
  cy += mpn_add_n (a1a3, a1a3, asm2, s);		/* 8a3      +2a1      */
  MPN_INCR_U(a1a3 + s, n - s + 1, cy);
  cy = mpn_lshift (asm2, a2, n, 2);			/*      4a2           */
  a0a2[n] = cy + mpn_add_n (a0a2, a0, asm2, n);		/*      4a2      + a0 */

#if HAVE_NATIVE_mpn_addsub_n
  if (mpn_cmp (a0a2, a1a3, n+1) < 0)
    {
      mpn_addsub_n (as2, asm2, a1a3, a0a2, n+1);
      flags ^= toom6_vm2_neg;
    }
  else
    {
      mpn_addsub_n (as2, asm2, a0a2, a1a3, n+1);
    }
#else
  mpn_add_n (as2, a0a2, a1a3, n+1);
  if (mpn_cmp (a0a2, a1a3, n+1) < 0)
    {
      mpn_sub_n (asm2, a1a3, a0a2, n+1);
      flags ^= toom6_vm2_neg;
    }
  else
    {
      mpn_sub_n (asm2, a0a2, a1a3, n+1);
    }
#endif

  /* Compute bs2 and bsm2.  */
  b1d[n] = mpn_lshift (b1d, b1, n, 1);			/*       2b1      */
  cy  = mpn_lshift (b0b2, b2, t, 2);			/*  4b2           */
  cy += mpn_add_n (b0b2, b0b2, b0, t);			/*  4b2      + b0 */
  if (t != n)
    cy = mpn_add_1 (b0b2 + t, b0 + t, n - t, cy);
  b0b2[n] = cy;

#if HAVE_NATIVE_mpn_addsub_n
  if (mpn_cmp (b0b2, b1d, n+1) < 0)
    {
      mpn_addsub_n (bs2, bsm2, b1d, b0b2, n+1);
      flags ^= toom6_vm2_neg;
    }
  else
    {
      mpn_addsub_n (bs2, bsm2, b0b2, b1d, n+1);
    }
#else
  mpn_add_n (bs2, b0b2, b1d, n+1);
  if (mpn_cmp (b0b2, b1d, n+1) < 0)
    {
      mpn_sub_n (bsm2, b1d, b0b2, n+1);
      flags ^= toom6_vm2_neg;
    }
  else
    {
      mpn_sub_n (bsm2, b0b2, b1d, n+1);
    }
#endif

  /* Compute as1 and asm1.  */
  a0a2[n] = mpn_add_n (a0a2, a0, a2, n);
  asm1[n] = mpn_add (asm1, a1, n, a3, s);
#if HAVE_NATIVE_mpn_addsub_n
  if (mpn_cmp (a0a2, asm1, n+1) < 0)
    {
      cy = mpn_addsub_n (as1, asm1, asm1, a0a2, n+1);
      flags ^= toom6_vm1_neg;
    }
  else
    {
      cy = mpn_addsub_n (as1, asm1, a0a2, asm1, n+1);
    }
#else
  mpn_add_n (as1, a0a2, asm1, n+1);
  if (mpn_cmp (a0a2, asm1, n+1) < 0)
    {
      mpn_sub_n (asm1, asm1, a0a2, n+1);
      flags ^= toom6_vm1_neg;
    }
  else
    {
      mpn_sub_n (asm1, a0a2, asm1, n+1);
    }
#endif

  /* Compute bs1 and bsm1.  */
  bsm1[n] = mpn_add (bsm1, b0, n, b2, t);
#if HAVE_NATIVE_mpn_addsub_n
  if (bsm1[n] == 0 && mpn_cmp (bsm1, b1, n) < 0)
    {
      cy = mpn_addsub_n (bs1, bsm1, b1, bsm1, n);
      bs1[n] = cy >> 1;
      flags ^= toom6_vm1_neg;
    }
  else
    {
      cy = mpn_addsub_n (bs1, bsm1, bsm1, b1, n);
      bs1[n] = bsm1[n] + (cy >> 1);
      bsm1[n]-= cy & 1;
    }
#else
  bs1[n] = bsm1[n] + mpn_add_n (bs1, bsm1, b1, n);
  if (bsm1[n] == 0 && mpn_cmp (bsm1, b1, n) < 0)
    {
      mpn_sub_n (bsm1, b1, bsm1, n);
      flags ^= toom6_vm1_neg;
    }
  else
    {
      bsm1[n] -= mpn_sub_n (bsm1, bsm1, b1, n);
    }
#endif

  ASSERT (as1[n] <= 3);
  ASSERT (bs1[n] <= 2);
  ASSERT (asm1[n] <= 1);
  ASSERT (bsm1[n] <= 1);
  ASSERT (as2[n] <=14);
  ASSERT (bs2[n] <= 6);
  ASSERT (asm2[n] <= 9);
  ASSERT (bsm2[n] <= 4);

  /* vm1, 2n+1 limbs */
  mpn_mul_n (vm1, asm1, bsm1, n+1);  /* W4 */

  /* vm2, 2n+1 limbs */
  mpn_mul_n (vm2, asm2, bsm2, n+1);  /* W2 */

  /* v2, 2n+1 limbs */
  mpn_mul_n (v2, as2, bs2, n+1);  /* W1 */

  /* v1, 2n+1 limbs */
  mpn_mul_n (v1, as1, bs1, n+1);  /* W3 */

  /* vinf, s+t limbs */   /* W0 */
  if (s > t)  mpn_mul (vinf, a3, s, b2, t);
  else        mpn_mul (vinf, b2, t, a3, s);

  /* v0, 2n limbs */
  mpn_mul_n (v0, ap, bp, n);  /* W5 */

  mpn_toom_interpolate_6pts (pp, n, flags, vm1, vm2, v2, t + s);

#undef v0
#undef vm1
#undef v1
#undef vm2
#undef v2
#undef vinf
#undef bs1
#undef bs2
#undef bsm1
#undef bsm2
#undef asm1
#undef asm2
/* #undef as1 */
/* #undef as2 */
#undef a0a2
#undef b0b2
#undef a1a3
#undef b1d
#undef a0
#undef a1
#undef a2
#undef a3
#undef b0
#undef b1
#undef b2

  TMP_FREE;
}
