/* mpn_mul_toom33 -- Multiply {ap,an} and {bp,bn} where an and bn are close in
   size.  Or more accurately, bn <= an < (3/2)bn.

   Contributed to the GNU project by Torbjorn Granlund.

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

/* Evaluate in: -1, 0, +1, +2, +inf

  <-s-><--n--><--n--><--n-->
   ___ ______ ______ ______
  |a3_|___a2_|___a1_|___a0_|
	       |_b1_|___b0_|
	       <-t--><--n-->

  v0  =  a0         * b0          #   A(0)*B(0)
  v1  = (a0+ a1+ a2)*(b0+ b1+ b2) #   A(1)*B(1)      ah  <= 2  bh <= 2
  vm1 = (a0- a1+ a2)*(b0- b1+ b2) #  A(-1)*B(-1)    |ah| <= 1  bh <= 1
  v2  = (a0+2a1+4a2)*(b0+2b1+4b2) #   A(2)*B(2)      ah  <= 6  bh <= 6
  vinf=          a2 *         b2  # A(inf)*B(inf)
*/

void
mpn_mul_toom33 (mp_ptr pp,
		mp_srcptr ap, mp_size_t an,
		mp_srcptr bp, mp_size_t bn)
{
  mp_size_t n, s, t;
  int vm1_neg;
  mp_limb_t cy, vinf0;
  mp_ptr gp;
  mp_ptr as1, asm1, as2;
  mp_ptr bs1, bsm1, bs2;
  mp_ptr scratch;
  TMP_DECL;

#define a0  ap
#define a1  (ap + n)
#define a2  (ap + 2*n)
#define b0  bp
#define b1  (bp + n)
#define b2  (bp + 2*n)

  n = (an + 2) / (unsigned long) 3;

  s = an - 2 * n;
  t = bn - 2 * n;

  ASSERT (0 < s && s <= n);
  ASSERT (0 < t && t <= n);

  TMP_MARK;

  as1 = TMP_SALLOC_LIMBS (n + 1);
  asm1 = TMP_SALLOC_LIMBS (n + 1);
  as2 = TMP_SALLOC_LIMBS (n + 1);

  bs1 = TMP_SALLOC_LIMBS (n + 1);
  bsm1 = TMP_SALLOC_LIMBS (n + 1);
  bs2 = TMP_SALLOC_LIMBS (n + 1);

  gp = pp;

  vm1_neg = 0;

  /* Compute as1 and asm1.  */
  cy = mpn_add (gp, a0, n, a2, s);
#if HAVE_NATIVE_mpn_addsub_n
  if (cy == 0 && mpn_cmp (gp, a1, n) < 0)
    {
      cy = mpn_addsub_n (as1, asm1, a1, gp, n);
      as1[n] = 0;
      asm1[n] = 0;
      vm1_neg = 1;
    }
  else
    {
      cy2 = mpn_addsub_n (as1, asm1, gp, a1, n);
      as1[n] = cy + (cy2 >> 1);
      asm1[n] = cy - (cy & 1);
    }
#else
  as1[n] = cy + mpn_add_n (as1, gp, a1, n);
  if (cy == 0 && mpn_cmp (gp, a1, n) < 0)
    {
      mpn_sub_n (asm1, a1, gp, n);
      asm1[n] = 0;
      vm1_neg = 1;
    }
  else
    {
      cy -= mpn_sub_n (asm1, gp, a1, n);
      asm1[n] = cy;
    }
#endif

  /* Compute as2.  */
#if HAVE_NATIVE_mpn_addlsh1_n
  cy  = mpn_addlsh1_n (as2, a1, a2, s);
  if (s != n)
    cy = mpn_add_1 (as2 + s, a1 + s, n - s, cy);
  cy = 2 * cy + mpn_addlsh1_n (as2, a0, as2, n);
#else
  cy  = mpn_lshift (as2, a2, s, 1);
  cy += mpn_add_n (as2, a1, as2, s);
  if (s != n)
    cy = mpn_add_1 (as2 + s, a1 + s, n - s, cy);
  cy = 2 * cy + mpn_lshift (as2, as2, n, 1);
  cy += mpn_add_n (as2, a0, as2, n);
#endif
  as2[n] = cy;

  /* Compute bs1 and bsm1.  */
  cy = mpn_add (gp, b0, n, b2, t);
#if HAVE_NATIVE_mpn_addsub_n
  if (cy == 0 && mpn_cmp (gp, b1, n) < 0)
    {
      cy = mpn_addsub_n (bs1, bsm1, b1, gp, n);
      bs1[n] = 0;
      bsm1[n] = 0;
      vm1_neg ^= 1;
    }
  else
    {
      cy2 = mpn_addsub_n (bs1, bsm1, gp, b1, n);
      bs1[n] = cy + (cy2 >> 1);
      bsm1[n] = cy - (cy & 1);
    }
#else
  bs1[n] = cy + mpn_add_n (bs1, gp, b1, n);
  if (cy == 0 && mpn_cmp (gp, b1, n) < 0)
    {
      mpn_sub_n (bsm1, b1, gp, n);
      bsm1[n] = 0;
      vm1_neg ^= 1;
    }
  else
    {
      cy -= mpn_sub_n (bsm1, gp, b1, n);
      bsm1[n] = cy;
    }
#endif

  /* Compute bs2.  */
#if HAVE_NATIVE_mpn_addlsh1_n
  cy  = mpn_addlsh1_n (bs2, b1, b2, t);
  if (t != n)
    cy = mpn_add_1 (bs2 + t, b1 + t, n - t, cy);
  cy = 2 * cy + mpn_addlsh1_n (bs2, b0, bs2, n);
#else
  cy  = mpn_lshift (bs2, b2, t, 1);
  cy += mpn_add_n (bs2, b1, bs2, t);
  if (t != n)
    cy = mpn_add_1 (bs2 + t, b1 + t, n - t, cy);
  cy = 2 * cy + mpn_lshift (bs2, bs2, n, 1);
  cy += mpn_add_n (bs2, b0, bs2, n);
#endif
  bs2[n] = cy;

  ASSERT (as1[n] <= 2);
  ASSERT (bs1[n] <= 2);
  ASSERT (asm1[n] <= 1);
  ASSERT (bsm1[n] <= 1);
  ASSERT (as2[n] <= 6);
  ASSERT (bs2[n] <= 6);

#define v0    pp				/* 2n */
#define v1    (pp + 2 * n)			/* 2n+1 */
#define vinf  (pp + 4 * n)			/* s+t */
#define vm1   scratch				/* 2n+1 */
#define v2    (scratch + 2 * n + 1)		/* 2n+2 */

  /* We could trim this to 4n+3 if HAVE_NATIVE_mpn_sublsh1_n, since
     mpn_toom_interpolate_5pts only needs scratch otherwise.  */
  scratch = TMP_SALLOC_LIMBS (6 * n + 3);

  /* vm1, 2n+1 limbs */
#ifdef SMALLER_RECURSION
  mpn_mul_n (vm1, asm1, bsm1, n);
  cy = 0;
  if (asm1[n] != 0)
    cy = bsm1[n] + mpn_add_n (vm1 + n, vm1 + n, bsm1, n);
  if (bsm1[n] != 0)
    cy += mpn_add_n (vm1 + n, vm1 + n, asm1, n);
  vm1[2 * n] = cy;
#else
  mpn_mul_n (vm1, asm1, bsm1, n + 1);
#endif

  mpn_mul_n (v2, as2, bs2, n + 1);		/* v2, 2n+1 limbs */

  /* vinf, s+t limbs */
  if (s > t)  mpn_mul (vinf, a2, s, b2, t);
  else        mpn_mul (vinf, b2, t, a2, s);

  vinf0 = vinf[0];				/* v1 overlaps with this */

#ifdef SMALLER_RECURSION
  /* v1, 2n+1 limbs */
  mpn_mul_n (v1, as1, bs1, n);
  if (as1[n] == 1)
    {
      cy = bs1[n] + mpn_add_n (v1 + n, v1 + n, bs1, n);
    }
  else if (as1[n] != 0)
    {
#if HAVE_NATIVE_mpn_addlsh1_n
      cy = 2 * bs1[n] + mpn_addlsh1_n (v1 + n, v1 + n, bs1, n);
#else
      cy = 2 * bs1[n] + mpn_addmul_1 (v1 + n, bs1, n, CNST_LIMB(2));
#endif
    }
  else
    cy = 0;
  if (bs1[n] == 1)
    {
      cy += mpn_add_n (v1 + n, v1 + n, as1, n);
    }
  else if (bs1[n] != 0)
    {
#if HAVE_NATIVE_mpn_addlsh1_n
      cy += mpn_addlsh1_n (v1 + n, v1 + n, as1, n);
#else
      cy += mpn_addmul_1 (v1 + n, as1, n, CNST_LIMB(2));
#endif
    }
  v1[2 * n] = cy;
#else
  cy = vinf[1];
  mpn_mul_n (v1, as1, bs1, n + 1);
  vinf[1] = cy;
#endif

  mpn_mul_n (v0, ap, bp, n);			/* v0, 2n limbs */

  mpn_toom_interpolate_5pts (pp, v2, vm1, n, s + t, 1^vm1_neg, vinf0, scratch + 4 * n + 4);

  TMP_FREE;
}


#define CONCAT(name,M,N)  name ## M ## N

#define M 3
#define N 3
#define mpn_mul_toomMN CONCAT(mpn_mul_toom,3,3)

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
      if (err == 0 && test % 0x100 == 0)
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
  mp_size_t an, bn;
  mp_ptr ap, bp, refp, pp;
  double t;
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

#if 0
  if (!(2 * an > 3 * bn && an < 4 * bn))
    {
      fprintf (stderr, "Invalid value combination of an,bn\n");
      return 1;
    }
#endif

  ap = TMP_ALLOC_LIMBS (an);
  bp = TMP_ALLOC_LIMBS (bn);
  refp = TMP_ALLOC_LIMBS (an + bn);
  pp = TMP_ALLOC_LIMBS (an + bn);

  mpn_random (ap, an);
  mpn_random (bp, bn);
  TIME (t, mpn_mul_toomMN (pp, ap, an, bp, bn));
  printf ("mpn_mul_toom%d%d:   %f\n", M, N, t);
  TIME (t, mpn_mul (refp, ap, an, bp, bn));
  printf ("mpn_mul:          %f\n", t);
  TIME (t, mpn_mul_basecase (refp, ap, an, bp, bn));
  printf ("mpn_mul_basecase: %f\n", t);
  TMP_FREE;

  return 0;
}
#endif
