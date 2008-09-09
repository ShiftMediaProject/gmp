/* mpn_gcdext -- Extended Greatest Common Divisor.

Copyright 1996, 1998, 2000, 2001, 2002, 2003, 2004, 2005, 2008 Free Software
Foundation, Inc.

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

/* Temporary storage: an + 1 for initial division, independent of
   everything else. 2*(n+1) for u. n+1 for the matrix-vector
   multiplications (if hgcd2 succeeds). If hgcd fails, n+1 limbs are
   needed for the division, with most n for the quotient, and n+1 for
   the product q u0.

   In all, max(an + 1, 4n + 3). */

mp_size_t
mpn_gcdext_lehmer_itch (mp_size_t an, mp_size_t bn)
{
  return MAX(an+1, 4*bn + 3);
}

mp_size_t
mpn_gcdext_lehmer (mp_ptr gp, mp_ptr up, mp_size_t *usize,
		   mp_ptr ap, mp_size_t an,
		   mp_ptr bp, mp_size_t n,
		   mp_ptr tp)
{
  mp_size_t ualloc = n + 1;

  /* Keeps track of the second row of the reduction matrix
   *
   *   M = (v0, v1 ; u0, u1)
   *
   * which correspond to the first column of the inverse
   *
   *   M^{-1} = (u1, -v1; -u0, v0)
   */

  mp_size_t un;
  mp_ptr u0;
  mp_ptr u1;
  int swapped;

  if (an > n)
    {
      /* Initial division */
      mpn_tdiv_qr (tp + n, tp, 0, ap, an, bp, n);
      an = n;
      MPN_NORMALIZE(tp, an);
      if (an == 0)
	{
	  MPN_COPY (gp, bp, n);
	  *usize = 0;
	  return n;
	}
      else
	MPN_COPY(ap, tp, n);
    }
  MPN_ZERO (tp, 2*ualloc);
  u0 = tp; tp += ualloc;
  u1 = tp; tp += ualloc;

  u1[0] = 1; un = 1;

  swapped = 0;

  while (n >= 2)
    {
      struct hgcd_matrix1 M;
      mp_limb_t ah, al, bh, bl;
      mp_limb_t mask;

      mask = ap[n-1] | bp[n-1];
      ASSERT (mask > 0);

      if (mask & GMP_NUMB_HIGHBIT)
	{
	  ah = ap[n-1]; al = ap[n-2];
	  bh = bp[n-1]; bl = bp[n-2];
	}
      else if (n == 2)
	{
	  /* We use the full inputs without truncation, so we can
	     safely shift left. */
	  int shift;

	  count_leading_zeros (shift, mask);
	  ah = MPN_EXTRACT_NUMB (shift, ap[1], ap[0]);
	  al = ap[0] << shift;
	  bh = MPN_EXTRACT_NUMB (shift, bp[1], bp[0]);
	  bl = bp[0] << shift;	  
	}
      else
	{
	  int shift;

	  count_leading_zeros (shift, mask);
	  ah = MPN_EXTRACT_NUMB (shift, ap[n-1], ap[n-2]);
	  al = MPN_EXTRACT_NUMB (shift, ap[n-2], ap[n-3]);
	  bh = MPN_EXTRACT_NUMB (shift, bp[n-1], bp[n-2]);
	  bl = MPN_EXTRACT_NUMB (shift, bp[n-2], bp[n-3]);
	}

      /* Try an mpn_nhgcd2 step */
      if (mpn_hgcd2 (ah, al, bh, bl, &M))
	{
	  n = mpn_hgcd_mul_matrix1_inverse_vector (&M, n, ap, bp, tp);
	  un = mpn_hgcd_mul_matrix1_vector(&M, un, u0, u1, tp);
	}
      else
	{
	  /* mpn_hgcd2 has failed. Then either one of a or b is very
	     small, or the difference is very small. Perform one
	     subtraction followed by one division. */

	  /* FIXME: Split into a separate function
	   * mpn_gcdext_subdiv_step? Current code is based on
	   * mpn_gcd_subdiv_step, swapping u0 and u1 whenever a and b
	   * are swapped, and updating u0, u1 whenever a or b is
	   * reduced. */
	  mp_size_t an, bn;
	  mp_size_t qn;
	  mp_size_t u0n;

	  ASSERT (n > 0);
	  ASSERT (ap[n-1] > 0 || bp[n-1] > 0);

	  an = bn = n;
	  MPN_NORMALIZE (ap, an);
	  MPN_NORMALIZE (bp, bn);

	  if (UNLIKELY (an == 0))
	    {
	    return_b:
	      MPN_COPY (gp, bp, bn);
	      MPN_NORMALIZE (u0, un);
	      MPN_COPY (up, u0, un);
	      if (!swapped)
		un = -un;
	      *usize = un;
	      return bn;
	    }
	  else if (UNLIKELY (bn == 0))
	    {
	    return_a:
	      MPN_COPY (gp, ap, an);
	      MPN_NORMALIZE (u1, un);
	      MPN_COPY (up, u1, un);
	      if (swapped)
		un = -un;
	      *usize = un;
	      return an;
	    }

	  /* Arrange so that a > b, subtract an -= bn, and maintain
	     normalization. */
	  if (an < bn)
	    {
	      MPN_PTR_SWAP (ap, an, bp, bn);
	      MP_PTR_SWAP (u0, u1);
	      swapped ^= 1;
	    }
	  else if (an == bn)
	    {
	      int c;
	      MPN_CMP (c, ap, bp, an);
	      if (UNLIKELY (c == 0))
		goto return_a;
	      else if (c < 0)
		{
		  MP_PTR_SWAP (ap, bp);
		  MP_PTR_SWAP (u0, u1);
		  swapped ^= 1;
		}
	    }
	  /* Reduce a -= b, u1 += u0 */
	  ASSERT_NOCARRY (mpn_sub (ap, ap, an, bp, bn));
	  MPN_NORMALIZE (ap, an);
	  ASSERT (an > 0);

	  u1[un] = mpn_add_n (u1, u1, u0, un);
	  un += (u1[un] > 0);

	  /* Arrange so that a > b, and divide a = q b + r */
	  if (an < bn)
	    {
	      MPN_PTR_SWAP (ap, an, bp, bn);
	      MP_PTR_SWAP (u0, u1);
	      swapped ^= 1;
	    }
	  else if (an == bn)
	    {
	      int c;
	      MPN_CMP (c, ap, bp, an);
	      if (UNLIKELY (c == 0))
		goto return_a;
	      else if (c < 0)
		{
		  MP_PTR_SWAP (ap, bp);
		  MP_PTR_SWAP (u0, u1);
		  swapped ^= 1;
		}
	    }

	  /* Reduce a -= q b, u1 += q u0 */
	  qn = an - bn + 1;
	  mpn_tdiv_qr (tp, tp + qn, 0, ap, an, bp, bn);

	  /* Normalizing seems to be the simplest way to test if the
	     remainder is zero. FIXME: Use mpn_zero_p function or
	     macro. See toom functions. */
	  an = bn;
	  MPN_NORMALIZE (tp + qn, an);
	  if (an == 0)
	    goto return_b;

	  MPN_COPY (ap, tp + qn, bn);
	  n = bn;

	  /* Update u1 += q u0 */
	  qn -= (tp[qn -1] == 0);
	  u0n = un;
	  MPN_NORMALIZE (u0, u0n);
	  ASSERT (u0n + qn <= ualloc);

	  if (qn > u0n)
	    mpn_mul (tp + qn, tp, qn, u0, u0n);
	  else
	    mpn_mul (tp + qn, u0, u0n, tp, qn);

	  if (qn + u0n > un)
	    {
	      ASSERT_NOCARRY (mpn_add (u1, tp + qn, qn + u0n, u1, un));
	      un = qn + u0n;
	      un -= (u1[un-1] == 0);
	    }
	  else
	    {
	      u1[un] = mpn_add (u1, u1, un, tp + qn, qn + u0n);
	      un += (u1[un] > 0);
	    }
	  ASSERT (un < ualloc);
	}
    }
  if (ap[0] == 0)
    {
      gp[0] = bp[0];

      MPN_NORMALIZE (u0, un);
      MPN_COPY (up, u0, un);
      if (!swapped)
	un = -un;
      *usize = un;
      return 1;
    }
  else if (bp[0] == 0)
    {
      gp[0] = ap[0];

      MPN_NORMALIZE (u1, un);
      MPN_COPY (up, u1, un);
      if (!swapped)
	un = un;
      *usize = un;
      return 1;
    }
  else
    {
      mp_limb_t uh, vh;
      mp_limb_t u;
      mp_limb_t v;

      gp[0] = mpn_gcdext_1 (&u, &v, ap[0], bp[0]);

      /* Set up = u u1 + v u0. Keep track of size, un grows by one or
	 two limbs. */
      uh = mpn_mul_1 (up, u1, un, u);
      vh = mpn_addmul_1 (up, u0, un, v);

      if ( (uh | vh) > 0)
	{
	  mp_limb_t cy;
	  uh += vh;
	  up[un++] = uh;
	  if (uh < vh)
	    up[un++] = 1;
	}
      if (swapped)
	un = -un;

      *usize = un;
      return 1;
    }
}
