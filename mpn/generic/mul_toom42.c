/* mpn_mul_toom42 -- Multiply {ap,an} and {bp,bn} where an is nominally twice
   as large as bn.  Or more accurately, 3bn < 2an < 8bn.  This function should
   probably not be used when 3an > 8bn, for efficiency reasons.

   Contributed by Torbjorn Granlund.  Additional improvement by Marco Bodrato.

   The idea of using asymmetric operands was suggested by Marco Bodrato and
   Alberto Zanoni.

   THE FUNCTION IN THIS FILE IS INTERNAL WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH IT THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT IT WILL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2006 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option) any
later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
the GNU MP Library; see the file COPYING.  If not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA. */

#include "gmp.h"
#include "gmp-impl.h"

void mpn_toom3_interpolate (mp_ptr, mp_ptr, mp_ptr, mp_size_t, mp_size_t, int,
			    mp_limb_t, mp_ptr);

static inline int
mpn_zero_p (mp_srcptr ap, mp_size_t n)
{
  mp_size_t i;
  for (i = n - 1; i >= 0; i--)
    {
      if (ap[i] != 0)
	return 0;
    }
  return 1;
}

/*
  Evaluate in: -1, 0, +1, +2, +inf

  <-s-><--n--><--n--><--n-->
   ___ ______ ______ ______
  |a3_|___a2_|___a1_|___a0_|
	       |_b1_|___b0_|
	       <-t--><--n-->

  v0  =  a0             * b0       #   A(0)*B(0)
  v1  = (a0+ a1+ a2+ a3)*(b0+ b1)  #   A(1)*B(1)
  vm1 = (a0- a1+ a2- a3)*(b0- b1)  #  A(-1)*B(-1)
  v2  = (a0+2a1+4a2+8a3)*(b0+2b1)  #   A(2)*B(2)
  vinf=              a3 *     b1   # A(inf)*B(inf)
*/

void
mpn_mul_toom42 (mp_ptr pp,
		mp_srcptr ap, mp_size_t an,
		mp_srcptr bp, mp_size_t bn)
{
  mp_size_t n, s, t;
  int vm1_neg;
  mp_limb_t cy, vinf0, vinf1;
  mp_ptr a0_a2, a1_a3;
  mp_ptr as1, asm1, as2;
  mp_ptr bs1, bsm1, bs2;
  mp_ptr scratch;
  TMP_DECL;

#define a0  ap
#define a1  (ap + n)
#define a2  (ap + 2*n)
#define a3  (ap + 3*n)
#define b0  bp
#define b1  (bp + n)

  n = an >= 2 * bn ? (an + 3) >> 2 : (bn + 1) >> 1;

  s = an - 3 * n;
  t = bn - n;

  ASSERT (0 < s && s <= n);
  ASSERT (0 < t && t <= n);

  /* FIXME: All these 6 allocations should be removed and the operands should
     reuse the pp area, and scratch area (allocated below).  Some thought is
     needed on operand placement and operand order to achieve best locality. */

  TMP_MARK;

  as1 = TMP_SALLOC_LIMBS (n + 1);
  asm1 = TMP_SALLOC_LIMBS (n + 1);
  as2 = TMP_SALLOC_LIMBS (n + 1);

  bs1 = TMP_SALLOC_LIMBS (n + 1);
  bsm1 = TMP_SALLOC_LIMBS (n + 1);
  bs2 = TMP_SALLOC_LIMBS (n + 1);

  a0_a2 = pp;
  a1_a3 = pp + n + 1;

  /* Compute as1 and asm1.  */
  a0_a2[n] = mpn_add_n (a0_a2, a0, a2, n);
  a1_a3[n] = mpn_add (a1_a3, a1, n, a3, s);
#if HAVE_NATIVE_mpn_addsub_n
  if (mpn_cmp (a0_a2, a1_a3, n + 1) < 0)
    {
      mpn_addsub_n (as1, asm1, a1_a3, a0_a2, n + 1);
      vm1_neg = 1;
    }
  else
    {
      mpn_addsub_n (as1, asm1, a0_a2, a1_a3, n + 1);
      vm1_neg = 0;
    }
#else
  mpn_add_n (as1, a0_a2, a1_a3, n + 1);
  if (mpn_cmp (a0_a2, a1_a3, n + 1) < 0)
    {
      mpn_sub_n (asm1, a1_a3, a0_a2, n + 1);
      vm1_neg = 1;
    }
  else
    {
      mpn_sub_n (asm1, a0_a2, a1_a3, n + 1);
      vm1_neg = 0;
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
  bsm1[n] = 0;			/* pad to make bsm1 as long as asm1 */

  /* Compute bs2, recycling bs1. bs2=bs1+b1  */
  mpn_add (bs2, bs1, n + 1, b1, t);

#define v0    pp				/* 2n */
#define v1    (pp+2*n)				/* 2n+1 */
#define vinf  (pp+4*n)				/* s+t */
#define vm1   scratch				/* 2n+2 */
#define v2    (scratch + 2 * n + 2)		/* 2n+2 */

  /* We could trim this to 4n+4 if HAVE_NATIVE_mpn_sublsh1_n, since
     mpn_toom3_interpolate only needs scratch otherwise.  */
  scratch = TMP_SALLOC_LIMBS (6 * n + 4);

  mpn_mul_n (vm1, asm1, bsm1, n + 1);		/* vm1, 2n+1 limbs */

  mpn_mul_n (v2, as2, bs2, n + 1);		/* v2,  2n+1 limbs */

  if (s > t)  mpn_mul (vinf, a3, s, b1, t);
  else        mpn_mul (vinf, b1, t, a3, s);

  vinf0 = vinf[0];				/* v1 overlaps with this */
  vinf1 = vinf[1];				/* v1 pseudo-overlaps with this */
  mpn_mul_n (v1, as1, bs1, n + 1);
  vinf[1] = vinf1;

  mpn_mul_n (v0, ap, bp, n);			/* v0,  2n   limbs */

  mpn_toom3_interpolate (pp, v2, vm1, n, s + t, 1^vm1_neg, vinf0, scratch + 4 * n + 4);

  TMP_FREE;
}


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
  mp_size_t n, s, t, an, bn;
  mp_ptr ap, bp, refp, pp;
  int test;
  int maxn;
  int norandom;
  int err = 0;
  TMP_DECL;
  TMP_MARK;

  an = 4 * SIZE;
  bn = 2 * SIZE;
  norandom = 0;

  if (argc >= 2)
    {
      maxn = strtol (argv[1], 0, 0);
      an = 4 * maxn;
      bn = 2 * maxn;
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
  pp = TMP_ALLOC_LIMBS (an + bn);

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
	  t = random () % n + 1;
	  an = 3 * n + s;
	  bn = n + t;
	}
      mpn_random2 (ap, an);
      mpn_random2 (bp, bn);
      mpn_random2 (pp, an+bn);
      mpn_mul_toom42 (pp, ap, an, bp, bn);
      mpn_mul (refp, ap, an, bp, bn);
      if (mpn_cmp (refp, pp, an + bn) != 0)
	{
	  printf ("ERROR in test %d\n", test);
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

  if (!(2 * an > 3 * bn && an < 4 * bn))
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
  TIME (t, mpn_mul_toom42 (pp, ap, an, bp, bn));
  printf ("mpn_mul_toom42: %f\n", t);
  TIME (t, mpn_mul (refp, ap, an, bp, bn));
  printf ("mpn_mul:        %f\n", t);
  TMP_FREE;

  return 0;
}
#endif
