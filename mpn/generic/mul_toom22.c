/* mpn_mul_toom22 -- Multiply {ap,an} and {bp,bn} where an >= bn.  Or more
   accurately, bn <= an < 2bn.  This function should probably not be used when
   3an > 4bn, for efficiency reasons.

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
  Evaluate in: -1, 0, +inf

  <-s-><--n-->
   ____ ______
  |_a1_|___a0_|
   |b1_|___b0_|
   <-t-><--n-->

  v0  =  a0     * b0       #   A(0)*B(0)
  vm1 = (a0- a1)*(b0- b1)  #  A(-1)*B(-1)
  vinf=      a1 *     b1   # A(inf)*B(inf)
*/

void
mpn_mul_toom22 (mp_ptr pp,
		mp_srcptr ap, mp_size_t an,
		mp_srcptr bp, mp_size_t bn,
		mp_ptr scratch)
{
  mp_size_t n, s, t;
  int vm1_neg;
  mp_limb_t cy, cy2;
  mp_ptr asm1;
  mp_ptr bsm1;

#define a0  ap
#define a1  (ap + n)
#define b0  bp
#define b1  (bp + n)

  n = (an + 1) >> 1;

  s = an - n;
  t = bn - n;

  ASSERT (0 < s && s <= n);
  ASSERT (0 < t && t <= s);

  asm1 = pp;
  bsm1 = pp + n;

  vm1_neg = 0;

  /* Compute asm1.  */
  if (s == n)
    {
      if (mpn_cmp (a0, a1, n) < 0)
	{
	  mpn_sub_n (asm1, a1, a0, n);
	  vm1_neg = 1;
	}
      else
	{
	  mpn_sub_n (asm1, a0, a1, n);
	}
    }
  else
    {
      if (mpn_zero_p (a0 + s, n - s) && mpn_cmp (a0, a1, s) < 0)
	{
	  mpn_sub_n (asm1, a1, a0, s);
	  MPN_ZERO (asm1 + s, n - s);
	  vm1_neg = 1;
	}
      else
	{
	  mpn_sub (asm1, a0, n, a1, s);
	}
    }

  /* Compute bsm1.  */
  if (t == n)
    {
      if (mpn_cmp (b0, b1, n) < 0)
	{
	  mpn_sub_n (bsm1, b1, b0, n);
	  vm1_neg ^= 1;
	}
      else
	{
	  mpn_sub_n (bsm1, b0, b1, n);
	}
    }
  else
    {
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

#define v0	pp				/* 2n */
#define vinf	(pp + 2 * n)			/* s+t */
#define vm1	scratch				/* 2n */
#define tp	(scratch + 2*n)

  /* vm1, 2n limbs */
  mpn_mul_n (vm1, asm1, bsm1, n);

  /* vinf, s+t limbs */
  mpn_mul (vinf, a1, s, b1, t);

  /* v0, 2n limbs */
  mpn_mul_n (v0, ap, bp, n);

  /* H(v0) + L(vinf) */
  cy = mpn_add_n (tp, v0 + n, vinf, n);

  /* L(v0) + H(v0) */
  cy2 = cy + mpn_add_n (pp + n, tp, v0, n);

  /* L(vinf) + H(vinf) */
  cy += mpn_add (pp + 2 * n, tp, n, vinf + n, s + t - n);

  if (vm1_neg)
    cy += mpn_add_n (pp + n, pp + n, vm1, 2 * n);
  else
    cy -= mpn_sub_n (pp + n, pp + n, vm1, 2 * n);

  ASSERT (cy  <= 2);
  ASSERT (cy2 <= 2);

  mpn_incr_u (pp + 2 * n, cy2);
  mpn_incr_u (pp + 3 * n, cy);
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
  mp_ptr ap, bp, refp, pp, scratch;
  int test;
  int maxn;
  int norandom;
  int err = 0;
  TMP_DECL;
  TMP_MARK;

  an = 2 * SIZE;
  bn = 2 * SIZE;
  norandom = 0;

  if (argc >= 2)
    {
      maxn = strtol (argv[1], 0, 0);
      an = 2 * maxn;
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
  scratch = TMP_SALLOC_LIMBS (an * bn);

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
	  t = random () % s + 1;
	  an = n + s;
	  bn = n + t;
	}
      mpn_random2 (ap, an);
      mpn_random2 (bp, bn);
      mpn_random2 (pp, an+bn);
      mpn_mul_toom22 (pp, ap, an, bp, bn, scratch);
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
  mp_ptr ap, bp, refp, pp, scratch;
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

  if (!(an >= bn && an < 2 * bn))
    {
      fprintf (stderr, "Invalid value combination of an,bn\n");
      return 1;
    }

  ap = TMP_ALLOC_LIMBS (an);
  bp = TMP_ALLOC_LIMBS (bn);
  refp = TMP_ALLOC_LIMBS (an + bn);
  pp = TMP_ALLOC_LIMBS (an + bn);
  scratch = TMP_SALLOC_LIMBS (an * bn);

  mpn_random (ap, an);
  mpn_random (bp, bn);
  TIME (t, mpn_mul_toom22 (pp, ap, an, bp, bn, scratch));
  printf ("mpn_mul_toom22: %f\n", t);
  TIME (t, mpn_mul (refp, ap, an, bp, bn));
  printf ("mpn_mul:        %f\n", t);
  TMP_FREE;

  return 0;
}
#endif
