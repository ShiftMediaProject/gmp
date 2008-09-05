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

static mp_size_t
gcdext_lehmer (mp_ptr gp, mp_ptr up, mp_size_t *usize,
	       mp_srcptr ap, mp_size_t an,
	       mp_srcptr bp, mp_size_t n,
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
  
  u0 = tp; tp += n + 1;
  u1 = tp; tp += n + 1;

  u0[0] = 0; u1[0] = 1; un = 1;

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
  while (n >= 2)
    {
      struct hgcd_matrix1 M;
      mp_limb_t ah, al, bh, bl;
      mp_limb_t mask;

      mask = ap[n-1] | bp[n-1];
      ASSERT (mask > 0);

      if (n == 2)
	{
	  /* We use the full inputs without truncation, so we can
	     safely shift left. */
	  int shift;

	  count_leading_zeros (shift, mask);
	  ah = MPN_EXTRACT_LIMB (shift, ap[1], ap[0]);
	  al = ap[0] << shift;
	  bh = MPN_EXTRACT_LIMB (shift, bp[1], bp[0]);
	  bl = bp[0] << shift;	  
	}
      else if (mask & GMP_NUMB_HIGHBIT)
	{
	  ah = ap[n-1]; al = ap[n-2];
	  bh = bp[n-1]; bl = bp[n-2];
	}
      else
	{
	  int shift;

	  count_leading_zeros (shift, mask);
	  ah = MPN_EXTRACT_LIMB (shift, ap[n-1], ap[n-2]);
	  al = MPN_EXTRACT_LIMB (shift, ap[n-2], ap[n-3]);
	  bh = MPN_EXTRACT_LIMB (shift, bp[n-1], bp[n-2]);
	  bl = MPN_EXTRACT_LIMB (shift, bp[n-2], bp[n-3]);
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

	  /* First, make sure that an >= bn, and subtract an -= bn */
	  for (an = n; an > 0; an--)
	    if (ap[an-1] != bp[an-1])
	      break;

	  if (an == 0)
	    {
	      /* Done */
	      MPN_COPY (gp, ap, n);
	      MPN_NORMALIZE(u0, un);
	      MPN_COPY (up, u0, un);
	      *gn = n;
	      return -un;
	    }

	  if (ap[an-1] < bp[an-1])
	    {
	      MP_PTR_SWAP (ap, bp);
	      MP_PTR_SWAP (u0, u1);
	    }
	  bn = n; /* FIXME: Can b be non-normalized here??? */
	  MPN_NORMALIZE (bp, bn);
	  if (bn == 0)
	    {
	      MPN_COPY (gp, ap, n);
	      MPN_NORMALIZE (u2, un);
	      MPN_COPY (up, u2, un);
	      *gn = n;
	      return un;
	    }

	  /* Reduce a -= b, u1 += u0 */
	  ASSERT_NOCARRY (mpn_sub_n (ap, ap, bp, an));
	  MPN_NORMALIZE (ap, an);
	  ASSERT (an > 0);

	  u1[un] = mpn_add_n (u1, u1, u0, un);
	  u0[un] = 0;
	  un += (u1[un] > 0);

	  if (an < bn)
	    {
	      MPN_PTR_SWAP (ap, an, bp, bn);
	      MP_PTR_SWAP (u0, u1);
	    }
	  else if (an == bn)
	    {
	      int c;
	      MPN_CMP (c, ap, bp, an);
	      if (c < 0)
		{
		  MP_PTR_SWAP (ap, bp);
		  MP_PTR_SWAP (u0, u1);
		}
	    }

	  /* Reduce a -= q b, u1 += q u0 */
	  qn = an - bn + 1;
	  mpn_tdiv_qr (tp, tp + qn, 0, ap, an, bp, bn);

	  /* Normalizing seems to be the simplest way to test if the remainder
	     is zero. */
	  an = bn;
	  MPN_NORMALIZE (tp + qn, an);
	  if (an == 0)
	    {
	      MPN_COPY (gp, bp, bn);
	      MPN_NORMALIZE (u0, un);
	      MPN_COPY (up, u0, un);
	      *gn = bn;
	      return -un;
	    }

	  MPN_COPY (ap, tp + qn, bn);
	  
	  /* Update u1 += q u0 */
	  qn -= (tp[qn -1] == 0);
	  u0n = un;
	  MPN_NORMALIZE (u0, u0n);
	  if (qn > u0n)
	    mpn_mul (tp + qn, tp, qn, u0, u0n);
	  else
	    mpn_mul (tp + qn, u0, u0n, tp, qn);

	  if (qn + u0n > un)
	    {
	      ASSERT_NOCARRY (mpn_add (u1, tp + qn, qn + u0n, u1, un));
	      MPN_ZERO (u0 + un, qn + u0n - un);
	      un = qn + u0n;
	      un -= (u1[un-1] == 0);
	    }
	  else
	    {
	      u1[un] = mpn_add (u1, u1, un, tp + qn, qn + u0n);
	      u0[un] = 0;
	      un += (u1[un] > 0);
	    }
	}
    }
  if (ap[0] == 0)
    {
      gp[0] = bp[0];
      *gn = 1;
      MPN_NORMALIZE (u0, un);
      MPN_COPY (up, u0, un);
      return -un;
    }
  else if (bp[0] == 0)
    {
      gp[0] = ap[0];
      *gn = 1;
      MPN_NORMALIZE (u1, un);
      MPN_COPY (up, u1, un);
      return un;
    }
  else
    {
      mp_limb_t uh, vh;
      mp_limb_t u;
      mp_limb_t v;

      gp[0] = mpn_gcdext_1 (&u, &v, ap[0], bp[0]);
      *gn = 1;

      /* Set up = u u1 + v u0. Keep track of size, un grows by one or
	 two limbs. */
      uh = mpn_mul_1 (up, u1, un, u);
      vh = mpn_addmul_1 (up, u0, un, v);

      if ( (vh | uh) > 0)
	{
	  mp_limb_t cy;
	  uh += vh;
	  up[un++] = uh;
	  if (uh < vh)
	    up[un++] = 1;
	}
      
      return un;
    }
}
