/* mpn_mul_toom44 -- Multiply {ap,an} and {bp,bn} where an and bn are close in
   size.  Or more accurately, bn <= an < (4/3)bn.

   Contributed to the GNU project by Torbjorn Granlund and Marco Bodrato.

   The idea of using asymmetric operands was suggested by Marco Bodrato and
   Alberto Zanoni.

   THE FUNCTION IN THIS FILE IS INTERNAL WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH IT THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT IT WILL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2006, 2007 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */


/*
  Things to work on:

  1. Trim allocation.  The allocations for as1, asm1, bs1, and bsm1 could be
     avoided by instead reusing the pp area and the scratch allocation.
*/

#include "gmp.h"
#include "gmp-impl.h"

/* Evaluate in: -1, -1/2, 0, +1/2, +1, +2, +inf

  <-s--><--n--><--n--><--n-->
   ____ ______ ______ ______
  |_a3_|___a2_|___a1_|___a0_|
   |b3_|___b2_|___b1_|___b0_|
   <-t-><--n--><--n--><--n-->

  v0  =   a0             *  b0              #    A(0)*B(0)
  v1  = ( a0+ a1+ a2+ a3)*( b0+ b1+ b2+ b3) #    A(1)*B(1)      ah  <= 3   bh  <= 3
  vm1 = ( a0- a1+ a2- a3)*( b0- b1+ b2- b3) #   A(-1)*B(-1)    |ah| <= 1  |bh| <= 1
  v2  = ( a0+2a1+4a2+8a3)*( b0+2b1+4b2+8b3) #    A(2)*B(2)      ah  <= 14  bh  <= 14
  vh  = (8a0+4a1+2a2+ a3)*(8b0+4b1+2b2+ b3) #  A(1/2)*B(1/2)    ah  <= 14  bh  <= 14
  vmh = (8a0-4a1+2a2- a3)*(8b0-4b1+2b2- b3) # A(-1/2)*B(-1/2)  -4<=ah<=9  -4<=bh<=9
  vinf=               a3 *          b2      #  A(inf)*B(inf)
*/

void
mpn_mul_toom44 (mp_ptr pp,
		mp_srcptr ap, mp_size_t an,
		mp_srcptr bp, mp_size_t bn)
{
  mp_size_t n, s, t;
  int vm1_neg, vmh_neg;
  mp_limb_t cy;
  mp_ptr gp, hp;
  mp_ptr as1, asm1, as2, ash, asmh;
  mp_ptr bs1, bsm1, bs2, bsh, bsmh;
  mp_ptr scratch;
  enum toom4_flags flags;
  TMP_DECL;

#define a0  ap
#define a1  (ap + n)
#define a2  (ap + 2*n)
#define a3  (ap + 3*n)
#define b0  bp
#define b1  (bp + n)
#define b2  (bp + 2*n)
#define b3  (bp + 3*n)

  n = (an + 3) >> 2;

  s = an - 3 * n;
  t = bn - 3 * n;

  ASSERT (0 < s && s <= n);
  ASSERT (0 < t && t <= n);

  TMP_MARK;

  as1  = TMP_SALLOC_LIMBS (n + 1);
  asm1 = TMP_SALLOC_LIMBS (n + 1);
  as2  = TMP_SALLOC_LIMBS (n + 1);
  ash  = TMP_SALLOC_LIMBS (n + 1);
  asmh = TMP_SALLOC_LIMBS (n + 1);

  bs1  = TMP_SALLOC_LIMBS (n + 1);
  bsm1 = TMP_SALLOC_LIMBS (n + 1);
  bs2  = TMP_SALLOC_LIMBS (n + 1);
  bsh  = TMP_SALLOC_LIMBS (n + 1);
  bsmh = TMP_SALLOC_LIMBS (n + 1);

  gp = pp;
  hp = pp + n + 1;

  vm1_neg = 0;
  vmh_neg = 0;

  /* Compute as1 and asm1.  */
  gp[n]  = mpn_add_n (gp, a0, a2, n);
  hp[n]  = mpn_add (hp, a1, n, a3, s);
#if HAVE_NATIVE_mpn_addsub_n
  if (mpn_cmp (gp, hp, n + 1) < 0)
    {
      mpn_addsub_n (as1, asm1, hp, gp, n + 1);
      vm1_neg ^= 1;
    }
  else
    {
      mpn_addsub_n (as1, asm1, gp, hp, n + 1);
    }
#else
  mpn_add_n (as1, gp, hp, n + 1);
  if (mpn_cmp (gp, hp, n + 1) < 0)
    {
      mpn_sub_n (asm1, hp, gp, n + 1);
      vm1_neg ^= 1;
    }
  else
    {
      mpn_sub_n (asm1, gp, hp, n + 1);
    }
#endif

  /* Compute as2.  */
#if HAVE_NATIVE_mpn_addlsh1_n
  cy  = mpn_addlsh1_n (as2, a2, a3, s);
  if (s != n)
    cy = mpn_add_1 (as2 + s, a2 + s, n - s, cy);
  cy = 2 * cy + mpn_addlsh1_n (as2, a1, as2, n);
  cy = 2 * cy + mpn_addlsh1_n (as2, a0, as2, n);
#else
  cy  = mpn_lshift (as2, a3, s, 1);
  cy += mpn_add_n (as2, a2, as2, s);
  if (s != n)
    cy = mpn_add_1 (as2 + s, a2 + s, n - s, cy);
  cy = 2 * cy + mpn_lshift (as2, as2, n, 1);
  cy += mpn_add_n (as2, a1, as2, n);
  cy = 2 * cy + mpn_lshift (as2, as2, n, 1);
  cy += mpn_add_n (as2, a0, as2, n);
#endif
  as2[n] = cy;

  /* Compute ash and asmh.  */
  cy  = mpn_lshift (gp, a0, n, 3);			/*  8a0             */
#if HAVE_NATIVE_mpn_addlsh1_n
  gp[n] = cy + mpn_addlsh1_n (gp, gp, a2, n);		/*  8a0 + 2a2       */
#else
  cy += mpn_lshift (hp, a2, n, 1);			/*        2a2       */
  gp[n] = cy + mpn_add_n (gp, gp, hp, n);		/*  8a0 + 2a2       */
#endif
  cy = mpn_lshift (hp, a1, n, 2);			/*  4a1             */
  hp[n] = cy + mpn_add (hp, hp, n, a3, s);		/*  4a1 +  a3       */
#if HAVE_NATIVE_mpn_addsub_n
  if (mpn_cmp (gp, hp, n + 1) < 0)
    {
      mpn_addsub_n (ash, asmh, hp, gp, n + 1);
      vmh_neg ^= 1;
    }
  else
    {
      mpn_addsub_n (ash, asmh, gp, hp, n + 1);
    }
#else
  mpn_add_n (ash, gp, hp, n + 1);
  if (mpn_cmp (gp, hp, n + 1) < 0)
    {
      mpn_sub_n (asmh, hp, gp, n + 1);
      vmh_neg ^= 1;
    }
  else
    {
      mpn_sub_n (asmh, gp, hp, n + 1);
    }
#endif

  /* Compute bs1 and bsm1.  */
  gp[n]  = mpn_add_n (gp, b0, b2, n);
  hp[n]  = mpn_add (hp, b1, n, b3, t);
#if HAVE_NATIVE_mpn_addsub_n
  if (mpn_cmp (gp, hp, n + 1) < 0)
    {
      mpn_addsub_n (bs1, bsm1, hp, gp, n + 1);
      vm1_neg ^= 1;
    }
  else
    {
      mpn_addsub_n (bs1, bsm1, gp, hp, n + 1);
    }
#else
  mpn_add_n (bs1, gp, hp, n + 1);
  if (mpn_cmp (gp, hp, n + 1) < 0)
    {
      mpn_sub_n (bsm1, hp, gp, n + 1);
      vm1_neg ^= 1;
    }
  else
    {
      mpn_sub_n (bsm1, gp, hp, n + 1);
    }
#endif

  /* Compute bs2.  */
#if HAVE_NATIVE_mpn_addlsh1_n
  cy  = mpn_addlsh1_n (bs2, b2, b3, t);
  if (t != n)
    cy = mpn_add_1 (bs2 + t, b2 + t, n - t, cy);
  cy = 2 * cy + mpn_addlsh1_n (bs2, b1, bs2, n);
  cy = 2 * cy + mpn_addlsh1_n (bs2, b0, bs2, n);
#else
  cy  = mpn_lshift (bs2, b3, t, 1);
  cy += mpn_add_n (bs2, b2, bs2, t);
  if (t != n)
    cy = mpn_add_1 (bs2 + t, b2 + t, n - t, cy);
  cy = 2 * cy + mpn_lshift (bs2, bs2, n, 1);
  cy += mpn_add_n (bs2, b1, bs2, n);
  cy = 2 * cy + mpn_lshift (bs2, bs2, n, 1);
  cy += mpn_add_n (bs2, b0, bs2, n);
#endif
  bs2[n] = cy;

  /* Compute bsh and bsmh.  */
  cy  = mpn_lshift (gp, b0, n, 3);			/*  8b0             */
#if HAVE_NATIVE_mpn_addlsh1_n
  gp[n] = cy + mpn_addlsh1_n (gp, gp, b2, n);		/*  8b0 + 2b2       */
#else
  cy += mpn_lshift (hp, b2, n, 1);			/*        2b2       */
  gp[n] = cy + mpn_add_n (gp, gp, hp, n);		/*  8b0 + 2b2       */
#endif
  cy = mpn_lshift (hp, b1, n, 2);			/*  4b1             */
  hp[n] = cy + mpn_add (hp, hp, n, b3, t);		/*  4b1 +  b3       */
#if HAVE_NATIVE_mpn_addsub_n
  if (mpn_cmp (gp, hp, n + 1) < 0)
    {
      mpn_addsub_n (bsh, bsmh, hp, gp, n + 1);
      vmh_neg ^= 1;
    }
  else
    {
      mpn_addsub_n (bsh, bsmh, gp, hp, n + 1);
    }
#else
  mpn_add_n (bsh, gp, hp, n + 1);
  if (mpn_cmp (gp, hp, n + 1) < 0)
    {
      mpn_sub_n (bsmh, hp, gp, n + 1);
      vmh_neg ^= 1;
    }
  else
    {
      mpn_sub_n (bsmh, gp, hp, n + 1);
    }
#endif

  ASSERT (as1[n] <= 3);
  ASSERT (bs1[n] <= 3);
  ASSERT (asm1[n] <= 1);
  ASSERT (bsm1[n] <= 1);
  ASSERT (as2[n] <= 14);
  ASSERT (bs2[n] <= 14);
  ASSERT (ash[n] <= 14);
  ASSERT (bsh[n] <= 14);
  ASSERT (asmh[n] <= 9);
  ASSERT (bsmh[n] <= 9);

  scratch = TMP_SALLOC_LIMBS (10 * n + 10);

#define v0    pp				/* 2n */
#define v1    (scratch + 6 * n + 6)		/* 2n+1 */
#define vm1   scratch				/* 2n+1 */
#define v2    (scratch + 2 * n + 2)		/* 2n+1 */
#define vinf  (pp + 6 * n)			/* s+t */
#define vh    (pp + 2 * n)			/* 2n+1 */
#define vmh   (scratch + 4 * n + 4)

  /* vm1, 2n+1 limbs */
  mpn_mul_n (vm1, asm1, bsm1, n + 1);		/* vm1, 2n+1 limbs */

  mpn_mul_n (v2, as2, bs2, n + 1);		/* v2,  2n+1 limbs */

  /* vinf, s+t limbs */
  if (s > t)  mpn_mul (vinf, a3, s, b3, t);
  else        mpn_mul (vinf, b3, t, a3, s);

  mpn_mul_n (v1, as1, bs1, n + 1);		/* v1,  2n+1 limbs */

  mpn_mul_n (vh, ash, bsh, n + 1);

  mpn_mul_n (vmh, asmh, bsmh, n + 1);

  mpn_mul_n (v0, ap, bp, n);			/* v0,  2n limbs */

  flags =  vm1_neg ? toom4_w3_neg : 0;
  flags |= vmh_neg ? toom4_w1_neg : 0;

  mpn_toom_interpolate_7pts (pp, n, flags, vmh, vm1, v1, v2, s + t, scratch + 8 * n + 8);

  TMP_FREE;
}


#define CONCAT(name,M,N)  name ## M ## N

#define M 4
#define N 4
#define mpn_mul_toomMN CONCAT(mpn_mul_toom,4,4)

#ifdef CHECK
#include <stdlib.h>
#include <stdio.h>

#ifndef SIZE
#define SIZE 10
#endif

void
dumpy (mp_srcptr p, mp_size_t n)
{
  mp_size_t i;
  for (i = n - 1; i >= 0; i--)
    {
      printf ("%0*lx", (int) (2 * sizeof (mp_limb_t)), p[i]);
      printf (" " + (i == 0));
    }
  puts ("");
}

int
main (int argc, char **argv)
{
  mp_size_t n, s, t, an, bn, clearn;
  mp_ptr ap, bp, refp, pp;
  mp_limb_t keep;
  int test;
  int maxn;
  int norandom;
  int err = 0;
  TMP_DECL;
  TMP_MARK;

  an = M * SIZE;
  bn = N * SIZE;
  norandom = 0;

  if (argc >= 2)
    {
      maxn = strtol (argv[1], 0, 0);
      an = M * maxn;
      bn = N * maxn;
      if (argc == 3)
	{
	  an = maxn;
	  bn = strtol (argv[2], 0, 0);
	  norandom = 1;
	}
    }
  else
    return 1;

  ap = TMP_ALLOC_LIMBS (an);
  bp = TMP_ALLOC_LIMBS (bn);
  refp = TMP_ALLOC_LIMBS (an + bn);
  pp = TMP_ALLOC_LIMBS (an + bn + 1);

  for (test = 0;; test++)
    {
      if (err == 0 && test % 0x1000 == 0)
	{
	  printf ("\r%d", test);  fflush (stdout);
	}
      if (! norandom)
	{
	  n = random () % maxn + 1;
	  s = random () % n + 1;
#if M == N
	  t = random () % s + 1;
#else
	  t = random () % n + 1;
#endif
	  an = (M - 1) * n + s;
	  bn = (N - 1) * n + t;
	}
      mpn_random2 (ap, an);
      clearn = random () % (an + 1);
      MPN_ZERO (ap + clearn, an - clearn);

      mpn_random2 (bp, bn);
      clearn = random () % (bn + 1);
      MPN_ZERO (bp + clearn, bn - clearn);

      mpn_random2 (pp, an + bn + 1);
      keep = pp[an + bn];

      mpn_mul_toomMN (pp, ap, an, bp, bn);
      mpn_mul (refp, ap, an, bp, bn);
      if (pp[an + bn] != keep || mpn_cmp (refp, pp, an + bn) != 0)
	{
	  printf ("ERROR in test %d\n", test);
	  if (pp[an + bn] != keep)
	    {
	      printf ("pp high:"); dumpy (pp + an + bn, 1);
	      printf ("keep:   "); dumpy (&keep, 1);
	    }
	  dumpy (ap, an);
	  dumpy (bp, bn);
	  dumpy (pp, an + bn);
	  dumpy (refp, an + bn);
	  if (++err > 5)
	    abort();
	}
    }
  TMP_FREE;
}
#endif

#ifdef TIMING
#include <stdlib.h>
#include <stdio.h>

#include "timing.h"

#ifndef SIZE
#define SIZE 10
#endif

int
main (int argc, char **argv)
{
  mp_size_t an, bn, n, s, t;
  mp_ptr ap, bp, refp, pp;
  double ti;
  TMP_DECL;
  TMP_MARK;

  if (argc >= 2)
    {
      an = bn = strtol (argv[1], 0, 0);
      if (argc == 3)
	bn = strtol (argv[2], 0, 0);
    }
  else
    return 1;

  n = (an + 3) >> 2;

  s = an - 3 * n;
  t = bn - 3 * n;

  if (! (0 < s && s <= n && 0 < t && t <= n))
    {
      fprintf (stderr, "Invalid value combination of an,bn\n");
      return 1;
    }

  ap = TMP_ALLOC_LIMBS (an);
  bp = TMP_ALLOC_LIMBS (bn);
  refp = TMP_ALLOC_LIMBS (an + bn);
  pp = TMP_ALLOC_LIMBS (an + bn);

  mpn_random (ap, an);
  mpn_random (bp, bn);
  TIME (ti, mpn_mul_toomMN (pp, ap, an, bp, bn));
  printf ("mpn_mul_toom%d%d:   %f\n", M, N, ti);
  TIME (ti, mpn_mul (refp, ap, an, bp, bn));
  printf ("mpn_mul:          %f\n", ti);
  TIME (ti, mpn_mul_basecase (refp, ap, an, bp, bn));
  printf ("mpn_mul_basecase: %f\n", ti);
  TMP_FREE;

  return 0;
}
#endif
