/* mpn_gcdext -- Extended Greatest Common Divisor.

Copyright (C) 1996 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#ifndef EXTEND
#define EXTEND 1
#endif

#if STAT
int arr[BITS_PER_MP_LIMB];
#endif


/* mpn_gcdext (GP, SP, SSIZE, UP, USIZE, VP, VSIZE)

   Compute the extended GCD of {UP,USIZE} and {VP,VSIZE} and store the
   greatest common divisor at GP, and the first cofactor at SP.  Write the
   size of the cofactor through the pointer SSIZE.  Return the size of the
   value at GP.  Note that SP might be a negative number; this is denoted
   by storing the negative of the size through SSIZE.

   {UP,USIZE} and {VP,VSIZE} are both clobbered.

   The space allocation for all four areas needs to be USIZE+1.

   Preconditions: 1) U >= V.
		  2) V > 0.
*/

/* Idea 1: After we have performed a full division, don't shift operands back,
	   but instead account for the extra factors-of-2 thus introduced.
   Idea 2: Simple generalization to use divide-and-conquer would give us an
	   algorithm that runs faster than O(n^2).
   Idea 3: The input numbers need less space as the computation progresses,
	   while the s0 and s1 variables need more space.  To save space, we
	   could make them share space, and have the latter variables grow
	   into the former.  */

#define swapptr(xp,yp) \
do { mp_ptr _swapptr_tmp = (xp); (xp) = (yp); (yp) = _swapptr_tmp; } while (0)

mp_size_t
#if EXTEND
#if __STDC__
mpn_gcdext (mp_ptr gp, mp_ptr s0p, mp_size_t *s0size,
	    mp_ptr up, mp_size_t size, mp_ptr vp, mp_size_t vsize)
#else
mpn_gcdext (gp, s0p, s0size, up, size, vp, vsize)
     mp_ptr gp;
     mp_ptr s0p;
     mp_size_t *s0size;
     mp_ptr up;
     mp_size_t size;
     mp_ptr vp;
     mp_size_t vsize;
#endif
#else
#if __STDC__
mpn_gcd (mp_ptr gp,
	 mp_ptr up, mp_size_t size, mp_ptr vp, mp_size_t vsize)
#else
mpn_gcd (gp, up, size, vp, vsize)
     mp_ptr gp;
     mp_ptr up;
     mp_size_t size;
     mp_ptr vp;
     mp_size_t vsize;
#endif
#endif
{
  mp_limb_t uh, vh;
  mp_limb_signed_t A, B, C, D;
  int cnt;
  mp_ptr tp, wp;
#if RECORD
  mp_limb_signed_t min = 0, max = 0;
#endif
#if EXTEND
  mp_ptr s1p;
  mp_ptr orig_s0p = s0p;
  mp_size_t ssize;
  int sign = 1;
  TMP_DECL (mark);

  TMP_MARK (mark);

  tp = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);
  wp = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);
  s1p = (mp_ptr) TMP_ALLOC ((size + 1) * BYTES_PER_MP_LIMB);

  MPN_ZERO (s0p, size);
  MPN_ZERO (s1p, size);

  s0p[0] = 1;
  s1p[0] = 0;
  ssize = 1;
#endif

  if (size > vsize)
    {
      /* Normalize V (and shift up U the same amount).  */
      count_leading_zeros (cnt, vp[vsize - 1]);
      if (cnt != 0)
	{
	  mp_limb_t cy;
	  mpn_lshift (vp, vp, vsize, cnt);
	  cy = mpn_lshift (up, up, size, cnt);
	  up[size] = cy;
	  size += cy != 0;
	}

      mpn_divmod (up + vsize, up, size, vp, vsize);
#if EXTEND
      /* This is really what it boils down to in this case... */
      s0p[0] = 0;
      s1p[0] = 1;
      sign = -sign;
#endif
      size = vsize;
      if (cnt != 0)
	{
	  mpn_rshift (up, up, size, cnt);
	  mpn_rshift (vp, vp, size, cnt);
	}
      swapptr (up, vp);
    }

  for (;;)
    {
      /* Figure out exact size of V.  */
      vsize = size;
      MPN_NORMALIZE (vp, vsize);
      if (vsize <= 1)
	break;

      /* Make UH be the most significant limb of U, and make VH be
	 corresponding bits from V.  */
      uh = up[size - 1];
      vh = vp[size - 1];
      count_leading_zeros (cnt, uh);
      if (cnt != 0)
	{
	  uh = (uh << cnt) | (up[size - 2] >> (BITS_PER_MP_LIMB - cnt));
	  vh = (vh << cnt) | (vp[size - 2] >> (BITS_PER_MP_LIMB - cnt));
	}

      A = 1;
      B = 0;
      C = 0;
      D = 1;

      for (;;)
	{
	  mp_limb_signed_t q, T;
	  if (vh + C == 0 || vh + D == 0)
	    break;

	  q = (uh + A) / (vh + C);
	  if (q != (uh + B) / (vh + D))
	    break;

	  T = A - q * C;
	  A = C;
	  C = T;
	  T = B - q * D;
	  B = D;
	  D = T;
	  T = uh - q * vh;
	  uh = vh;
	  vh = T;
#if EXTEND
	  sign = -sign;
#endif
	}

#if RECORD
      min = MIN (A, min);  min = MIN (B, min);
      min = MIN (C, min);  min = MIN (D, min);
      max = MAX (A, max);  max = MAX (B, max);
      max = MAX (C, max);  max = MAX (D, max);
#endif

      if (B == 0)
	{
	  mp_limb_t qh;
	  mp_size_t i;

	  /* This is quite rare.  I.e., optimize something else!  */

	  /* Normalize V (and shift up U the same amount).  */
	  count_leading_zeros (cnt, vp[vsize - 1]);
	  if (cnt != 0)
	    {
	      mp_limb_t cy;
	      mpn_lshift (vp, vp, vsize, cnt);
	      cy = mpn_lshift (up, up, size, cnt);
	      up[size] = cy;
	      size += cy != 0;
	    }

	  qh = mpn_divmod (up + vsize, up, size, vp, vsize);
#if EXTEND
	  MPN_COPY (tp, s0p, ssize);
	  {
	    mp_size_t qsize;

	    qsize = size - vsize; /* size of stored quotient from division */
	    if (ssize < qsize)
	      {
		MPN_ZERO (tp + ssize, qsize - ssize);
		MPN_ZERO (s1p + ssize, qsize); /* zero s1 too */
		for (i = 0; i < ssize; i++)
		  {
		    mp_limb_t cy;
		    cy = mpn_addmul_1 (tp + i, up + vsize, qsize, s1p[i]);
		    tp[qsize + i] = cy;
		  }
		if (qh != 0)
		  {
		    mp_limb_t cy;
		    cy = mpn_add_n (tp + qsize, tp + qsize, s1p, ssize);
		    if (cy != 0)
		      abort ();
		  }
	      }
	    else
	      {
		MPN_ZERO (s1p + ssize, qsize); /* zero s1 too */
		for (i = 0; i < qsize; i++)
		  {
		    mp_limb_t cy;
		    cy = mpn_addmul_1 (tp + i, s1p, ssize, up[vsize + i]);
		    tp[ssize + i] = cy;
		  }
		if (qh != 0)
		  {
		    mp_limb_t cy;
		    cy = mpn_add_n (tp + qsize, tp + qsize, s1p, ssize);
		    if (cy != 0)
		      {
			tp[qsize + ssize] = cy;
			s1p[qsize + ssize] = 0;
			ssize++;
		      }
		  }
	      }
	    ssize += qsize;
	    ssize -= tp[ssize - 1] == 0;
	  }

	  sign = -sign;
	  swapptr (s0p, s1p);
	  swapptr (s1p, tp);
#endif
	  size = vsize;
	  if (cnt != 0)
	    {
	      mpn_rshift (up, up, size, cnt);
	      mpn_rshift (vp, vp, size, cnt);
	    }
	  swapptr (up, vp);
	}
      else
	{
#if EXTEND
	  mp_size_t tsize, wsize;
#endif
	  /* T = U*A + V*B
	     W = U*C + V*D
	     U = T
	     V = W	   */

#if STAT
	  { mp_limb_t x;
	    x = ABS (A) | ABS (B) | ABS (C) | ABS (D);
	    count_leading_zeros (cnt, x);
	    arr[BITS_PER_MP_LIMB - cnt]++; }
#endif
	  if (A == 0)
	    {
	      /* B == 1 */
	      MPN_COPY (tp, vp, size);
	    }
	  else
	    {
	      if (A < 0)
		{
		  mpn_mul_1 (tp, vp, size, B);
		  mpn_submul_1 (tp, up, size, -A);
		}
	      else
		{
		  mpn_mul_1 (tp, up, size, A);
		  mpn_submul_1 (tp, vp, size, -B);
		}
	    }
	  if (C < 0)
	    {
	      mpn_mul_1 (wp, vp, size, D);
	      mpn_submul_1 (wp, up, size, -C);
	    }
	  else
	    {
	      mpn_mul_1 (wp, up, size, C);
	      mpn_submul_1 (wp, vp, size, -D);
	    }

	  swapptr (tp, up);
	  swapptr (wp, vp);

#if EXTEND
	  if (A == 0)
	    {
	      /* B == 1 */
	      MPN_COPY (tp, s1p, ssize);
	      tsize = ssize;
	      tp[ssize] = 0;	/* must zero since wp might spill below */
	    }
	  else
	    {
	      mp_limb_t cy;
	      if (A < 0)
		{
		  cy = mpn_mul_1 (tp, s1p, ssize, B);
		  cy += mpn_addmul_1 (tp, s0p, ssize, -A);
		}
	      else
		{
		  cy = mpn_mul_1 (tp, s0p, ssize, A);
		  cy += mpn_addmul_1 (tp, s1p, ssize, -B);
		}
	      tp[ssize] = cy;
	      tsize = ssize + (cy != 0);
	    }

	  {
	    mp_limb_t cy;
	    if (C < 0)
	      {
		cy = mpn_mul_1 (wp, s1p, ssize, D);
		cy += mpn_addmul_1 (wp, s0p, ssize, -C);
	      }
	    else
	      {
		cy = mpn_mul_1 (wp, s0p, ssize, C);
		cy += mpn_addmul_1 (wp, s1p, ssize, -D);
	      }
	    wp[ssize] = cy;
	    wsize = ssize + (cy != 0);
	  }
	  ssize = MAX (wsize, tsize);

	  swapptr (tp, s0p);
	  swapptr (wp, s1p);
#endif
#if 0	/* Is it a win to remove multiple zeros here? */
	  MPN_NORMALIZE (up, size);
#else
	  size -= up[size - 1] == 0;
#endif
	}
    }

#if RECORD
  printf ("min: %ld\n", min);
  printf ("max: %ld\n", max);
#endif

  if (vsize == 0)
    {
      if (gp != up)
	MPN_COPY (gp, up, size);
#if EXTEND
      MPN_NORMALIZE (s0p, ssize);
      if (orig_s0p != s0p)
	MPN_COPY (orig_s0p, s0p, ssize);
      *s0size = sign >= 0 ? ssize : -ssize;
#endif
      TMP_FREE (mark);
      return size;
    }
  else
    {
      mp_limb_t vl, ul, t;
#if EXTEND
      mp_size_t qsize, i;
#endif
      vl = vp[0];
#if EXTEND
      t = mpn_divmod_1 (wp, up, size, vl);

      MPN_COPY (tp, s0p, ssize);

      qsize = size - (wp[size - 1] == 0); /* size of quotient from division */
      if (ssize < qsize)
	{
	  MPN_ZERO (tp + ssize, qsize - ssize);
	  MPN_ZERO (s1p + ssize, qsize); /* zero s1 too */
	  for (i = 0; i < ssize; i++)
	    {
	      mp_limb_t cy;
	      cy = mpn_addmul_1 (tp + i, wp, qsize, s1p[i]);
	      tp[qsize + i] = cy;
	    }
	}
      else
	{
	  MPN_ZERO (s1p + ssize, qsize); /* zero s1 too */
	  for (i = 0; i < qsize; i++)
	    {
	      mp_limb_t cy;
	      cy = mpn_addmul_1 (tp + i, s1p, ssize, wp[i]);
	      tp[ssize + i] = cy;
	    }
	}
      ssize += qsize;
      ssize -= tp[ssize - 1] == 0;

      sign = -sign;
      swapptr (s0p, s1p);
      swapptr (s1p, tp);
#else
      t = mpn_mod_1 (up, size, vl);
#endif
      ul = vl;
      vl = t;
      while (vl != 0)
	{
	  mp_limb_t t;
#if EXTEND
	  mp_limb_t q;
	  q = ul / vl;
	  t = ul - q * vl;

	  MPN_COPY (tp, s0p, ssize);

	  MPN_ZERO (s1p + ssize, 1); /* zero s1 too */
	  for (i = 0; i < 1; i++)
	    {
	      mp_limb_t cy;
	      cy = mpn_addmul_1 (tp + i, s1p, ssize, q);
	      tp[ssize + i] = cy;
	    }

	  ssize += 1;
	  ssize -= tp[ssize - 1] == 0;

	  sign = -sign;
	  swapptr (s0p, s1p);
	  swapptr (s1p, tp);
#else
	  t = ul % vl;
#endif
	  ul = vl;
	  vl = t;
	}
      gp[0] = ul;
#if EXTEND
      MPN_NORMALIZE (s0p, ssize);
      if (orig_s0p != s0p)
	MPN_COPY (orig_s0p, s0p, ssize);
      *s0size = sign >= 0 ? ssize : -ssize;
#endif
      TMP_FREE (mark);
      return 1;
    }
}
