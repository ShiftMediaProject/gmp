/* mpn_mul_toom32 -- Multiply {ap,an} and {bp,bn} where an is nominally four
   times as large as bn.  Or more accurately, bn < an < 3bn.  This function
   should probably not be used when 2an > 5bn, for efficiency reasons.

   Contributed by Torbjorn Granlund.

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


/*
  Things to work on:

  1. Trim allocation.  The allocations for as1, asm1, bs1, and bsm1 could be
     avoided by instead reusing the pp area and the scratch allocation.

  2. Apply optimizations also to mul_toom42.c.
*/

#include "gmp.h"
#include "gmp-impl.h"

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
  Evaluate in: -1, 0, +1, +inf

  <-s-><--n--><--n-->
   ___ ______ ______
  |a2_|___a1_|___a0_|
	|_b1_|___b0_|
	<-t--><--n-->

  v0  =  a0         * b0       #   A(0)*B(0)
  v1  = (a0+ a1+ a2)*(b0+ b1)  #   A(1)*B(1)      ah  <= 2  bh <= 1
  vm1 = (a0- a1+ a2)*(b0- b1)  #  A(-1)*B(-1)    |ah| <= 1  bh = 0
  vinf=          a2 *     b1   # A(inf)*B(inf)
*/

void
mpn_mul_toom32 (mp_ptr pp,
		mp_srcptr ap, mp_size_t an,
		mp_srcptr bp, mp_size_t bn)
{
  mp_size_t n, s, t;
  int vm1_neg;
#if HAVE_NATIVE_mpn_add_nc
  mp_limb_t cy;
#else
  mp_limb_t cy, cy2;
#endif
  mp_ptr a0_a2;
  mp_ptr as1, asm1;
  mp_ptr bs1, bsm1;
  mp_ptr scratch;
  TMP_DECL;

#define a0  ap
#define a1  (ap + n)
#define a2  (ap + 2 * n)
#define b0  bp
#define b1  (bp + n)

  n = 2 * an >= 3 * bn ? (an + 2) / (unsigned) 3 : (bn + 1) >> 1;

  s = an - 2 * n;
  t = bn - n;

  ASSERT (0 < s && s <= n);
  ASSERT (0 < t && t <= n);

  TMP_MARK;

  as1 = TMP_SALLOC_LIMBS (n + 1);
  asm1 = TMP_SALLOC_LIMBS (n + 1);

  bs1 = TMP_SALLOC_LIMBS (n + 1);
  bsm1 = TMP_SALLOC_LIMBS (n);

  a0_a2 = pp;

  /* Compute as1 and asm1.  */
  a0_a2[n] = mpn_add (a0_a2, a0, n, a2, s);
#if HAVE_NATIVE_mpn_addsub_n
  if (a0_a2[n] == 0 && mpn_cmp (a0_a2, a1, n) < 0)
    {
      cy = mpn_addsub_n (as1, asm1, a1, a0_a2, n);
      as1[n] = cy >> 1;
      asm1[n] = 0;
      vm1_neg = 1;
    }
  else
    {
      cy = mpn_addsub_n (as1, asm1, a0_a2, a1, n);
      as1[n] = a0_a2[n] + (cy >> 1);
      asm1[n] = a0_a2[n] - (cy & 1);
      vm1_neg = 0;
    }
#else
  as1[n] = a0_a2[n] + mpn_add_n (as1, a0_a2, a1, n);

  if (a0_a2[n] == 0 && mpn_cmp (a0_a2, a1, n) < 0)
    {
      mpn_sub_n (asm1, a1, a0_a2, n);
      asm1[n] = 0;
      vm1_neg = 1;
    }
  else
    {
      cy = mpn_sub_n (asm1, a0_a2, a1, n);
      asm1[n] = a0_a2[n] - cy;
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

  scratch = TMP_SALLOC_LIMBS (4 * n + 2);

  /* vm1, 2n+1 limbs */
  mpn_mul_n (vm1, asm1, bsm1, n);
  vm1[2 * n] = 0;
  if (asm1[n] != 0)
    vm1[2 * n] = mpn_add_n (vm1 + n, vm1 + n, bsm1, n);

  /* vinf, s+t limbs */
  if (s > t)  mpn_mul (vinf, a2, s, b1, t);
  else        mpn_mul (vinf, b1, t, a2, s);

  /* v1, 2n+1 limbs */
  mpn_mul_n (v1, as1, bs1, n);
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
  v1[2 * n] = cy;

  if (bs1[n] != 0)
    v1[2 * n] += mpn_add_n (v1 + n, v1 + n, as1, n);

  mpn_mul_n (v0, ap, bp, n);                    /* v0, 2n limbs */

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

  mpn_sub_n (v1, v1, vm1, 2 * n + 1);
  v1[2 * n] -= mpn_sub_n (v1, v1, v0, 2 * n);

  /*
    pp[] prior to operations:
     |_H vinf|_L vinf|_______|_______|_______|

    summation scheme for remaining operations:
     |_______|_______|_______|_______|_______|
     |_Hvinf_|_Lvinf_|       |_H v0__|_L v0__|
		     | H vm1 | L vm1 |
		     |-H vinf|-L vinf|
	     | H v1  | L v1  |
  */

  mpn_sub (vm1, vm1, 2 * n + 1, vinf, s + t);
#if HAVE_NATIVE_mpn_add_nc
  cy = mpn_add_n (pp + n, pp + n, vm1, n);
  cy = mpn_add_nc (pp + 2 * n, v1, vm1 + n, n, cy);
  cy = mpn_add_nc (pp + 3 * n, pp + 3 * n, v1 + n, n, cy);
  mpn_incr_u (pp + 3 * n, vm1[2 * n]);
  if (LIKELY (n != s + t))  /* FIXME: Limit operand range to avoid condition */
    mpn_incr_u (pp + 4 * n, cy + v1[2 * n]);
#else
  cy2 = mpn_add_n (pp + n, pp + n, vm1, n);
  cy = mpn_add_n (pp + 2 * n, v1, vm1 + n, n);
  mpn_incr_u (pp + 2 * n, cy2);
  mpn_incr_u (pp + 3 * n, cy + vm1[2 * n]);
  cy = mpn_add_n (pp + 3 * n, pp + 3 * n, v1 + n,  n);
  if (LIKELY (n != s + t))  /* FIXME: Limit operand range to avoid condition */
    mpn_incr_u (pp + 4 * n, cy + v1[2 * n]);
#endif

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

  an = 3 * SIZE;
  bn = 2 * SIZE;
  norandom = 0;

  if (argc >= 2)
    {
      maxn = strtol (argv[1], 0, 0);
      an = 3 * maxn;
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
	  an = 2 * n + s;
	  bn = n + t;
	}
      mpn_random2 (ap, an);
      mpn_random2 (bp, bn);
      mpn_random2 (pp, an+bn);
      mpn_mul_toom32 (pp, ap, an, bp, bn);
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

  if (!(an > bn && an < 3 * bn))
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
  TIME (t, mpn_mul_toom32 (pp, ap, an, bp, bn));
  printf ("mpn_mul_toom32: %f\n", t);
  TIME (t, mpn_mul (refp, ap, an, bp, bn));
  printf ("mpn_mul:        %f\n", t);
  TMP_FREE;

  return 0;
}
#endif
