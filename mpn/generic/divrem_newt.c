/* Division using Newton's method.

Copyright (C) 1994, 1997 Free Software Foundation, Inc.

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

/*
  IDEAS:

  1) The wp space in mpn_divrem_newton is necessary because we are required
  (for compatibility) to allow qp to point into the np space.  Maybe it is
  possible to get rid of the wp space by avoiding to read from np after the
  first write to qp?

  2) We are conservative in choosing precisions in mpn_reciprocal.  It ought to
  be possible to be more aggressive by peeking at the ignored low halves of the
  products!

  3) The loop in mpn_reciprocal calls many mpn procedures.  Some of the
  mpn_addmul_1 calls could be changed into simpler ones, since they multiply by
  1 or 2.  We could also save time by adding new low-level procedures, for
  combined left-shift-and-add and left-shift-and-subtract.

  4) The current scheme for computing fractional quotient limbs is stupid.
  Improve the loops in mpn_divrem_newton to handle the situation without the
  current kludge!
*/

/* Don't make this smaller than 2, as the code won't handle that.  */
#ifndef RECIPROC_STARTPREC
#define RECIPROC_STARTPREC 8
#endif

mp_size_t
#if __STDC__
mpn_reciprocal (mp_ptr ip, mp_srcptr vp, mp_size_t vn)
#else
mpn_reciprocal (ip, vp, vn)
     mp_ptr ip;
     mp_srcptr vp;
     mp_size_t vn;
#endif
{
  mp_size_t xi;
  int n_iterations, i;
  mp_size_t tns[BITS_PER_MP_LIMB];
  mp_ptr xp, tp, x2p;
  mp_size_t xn, x2n;
  mp_size_t prec;
  TMP_DECL (marker);

  TMP_MARK (marker);
  xp = (mp_ptr) TMP_ALLOC (2 * vn * BYTES_PER_MP_LIMB);
  tp = (mp_ptr) TMP_ALLOC (2 * vn * BYTES_PER_MP_LIMB);
  x2p = (mp_ptr) TMP_ALLOC (2 * vn * BYTES_PER_MP_LIMB);

  /* If V starts with 0b100000..., we've got to handle that specially for
     correct operation of the Newton code.  */
  if ((vp[vn - 1] << 1) == 0)
    {
      for (i = vn - 2; i >= 0; i--)
	if (vp[i] != 0) goto found_nz;

      /* If we come here, all bits were 0 (but the most significant).  */
      for (i = 0; i < vn; i++)
	ip[i] = ~ (mp_limb_t) 0;
      TMP_FREE (marker);
      return vn;

    found_nz:
      xn = vn - i;
      xn = MAX (xn, RECIPROC_STARTPREC + 1);
      xn = MIN (xn, vn + 1);
    }
  else
    xn = 1 + MIN (vn, RECIPROC_STARTPREC);

  /* Compute starting approximation.  */
  tp[2 * xn - 1] = 1;
  MPN_ZERO (tp, 2 * xn - 1);
  mpn_divrem_classic (xp, (mp_size_t) 0, tp, 2 * xn, vp + vn - xn, xn);
  xn--;

  prec = xn;

  /* Determine the successive precisions to use in the iteration.  We minimize
     the precisions, from the highest (i.e. last iteration) to the lowest
     (i.e. first iteration).  */
  n_iterations = 0;
  for (xi = vn; xi > xn; xi = (xi + 2) >> 1)
    {
      tns[n_iterations] = xi;
      n_iterations++;
    }

  /* Main iteration loop.  */
  for (i = n_iterations - 1; i >= 0; i--)
    {
      mp_limb_t cy1, cy2;
      mp_ptr x2_high_p;
      mp_srcptr t_high_p;
      mp_size_t oprec;

      oprec = prec;
      prec = tns[i];

      /* x**2 */
      /* Need all bits of result.  Only lower half differ from the value
	 obtained in previous iteration.  */
      mpn_mul_n (x2p, xp, xp, oprec);
      x2n = 2 * oprec;
      cy2 = 1 + mpn_addmul_1 (x2p + oprec, xp, oprec, (mp_limb_t) 2);

      /* 2x */
      cy1 = 2 + mpn_lshift (tp + prec - oprec, xp, oprec, 1);
      MPN_ZERO (tp, prec - oprec);

      /* v * x**2 */
      /* Need only upper half of result.  Only lower half (of needed part)
	 differ from the value obtained in the previous iteration.  */
      t_high_p = vp + vn - prec; /* most significant PREC limbs */
      x2_high_p = x2p + x2n - prec;
      mpn_mul_n (xp, x2_high_p, t_high_p, prec);
      cy2 = mpn_addmul_1 (xp + prec, t_high_p, prec, cy2);
      xn = prec + prec;

      /* X now has more limbs than we care about.  Ignore all but the PREC most
	 significant ones.  */
      mpn_sub_n (xp, tp, xp + prec, prec);
      mpn_decr_u (xp, (mp_limb_t) 1); /* compensate for chopping */
    }

  MPN_COPY (ip, xp, vn);
  TMP_FREE (marker);
  return prec;
}

mp_limb_t
#if __STDC__
mpn_divrem_newton (mp_ptr qp, mp_size_t qxn,
		   mp_ptr np, mp_size_t nn,
		   mp_srcptr dp, mp_size_t dn)
#else
mpn_divrem_newton (qp, qxn, np, nn, dp, dn)
     mp_ptr qp;
     mp_size_t qxn;
     mp_ptr np;
     mp_size_t nn;
     mp_srcptr dp;
     mp_size_t dn;
#endif
{
  mp_ptr ip, tp, wp;
  mp_size_t qn, in;
  mp_limb_t most_significant_q_limb;
  int np_realloc_flag;
  mp_ptr np_new, np_orig;
  mp_size_t nn_new, nn_orig;
  TMP_DECL (marker);

  TMP_MARK (marker);
  np_realloc_flag = 0;
  if (qxn != 0)
    {
      /* This works, but wastes both memory and cycles.  But it is quite tricky
	 to generalize the code below to handle a non-zero qxn...  */
      nn_new = nn + qxn;
      np_new = (mp_ptr) TMP_ALLOC (nn_new * BYTES_PER_MP_LIMB);
      MPN_ZERO (np_new, qxn);
      MPN_COPY (np_new + qxn, np, nn);
      nn_orig = nn;
      np_orig = np;
      nn = nn_new;
      np = np_new;
      np_realloc_flag = 1;
    }

  /* See if the quotient will have nn-dn or nn-dn+1 limbs.
     Set most_significant_q_limb in the latter case.  */
  most_significant_q_limb = 0;
  if (mpn_cmp (np + nn - dn, dp, dn) >= 0)
    {
      mpn_sub_n (np + nn - dn, np + nn - dn, dp, dn);
      most_significant_q_limb = 1;
    }

  if (nn == dn)
    {
      TMP_FREE (marker);
      return most_significant_q_limb;
    }

  tp = (mp_ptr) TMP_ALLOC (2 * dn * BYTES_PER_MP_LIMB);
  wp = (mp_ptr) TMP_ALLOC (dn * BYTES_PER_MP_LIMB);

  /* Choose precision of reciprocal.  */
  in = (nn < 2 * dn) ? nn - dn : dn;
  ip = (mp_ptr) TMP_ALLOC (in * BYTES_PER_MP_LIMB);

  /* Compute reciprocal of divisor to chosen precision.  */
  MPN_COPY (tp, dp + dn - in, in);
  if ((dp[dn - in - 1] >> (BITS_PER_MP_LIMB - 1)) != 0)
    {
      /* Round the divisor upwards before computing the reciprocal.
	 We otherwise risk to get quotient that are 2 too large.  */
      mp_limb_t cy;
      cy = mpn_add_1 (tp, tp, in, (mp_limb_t) 1);
      if (cy)
	mpn_decr_u (tp, (mp_limb_t) 1);
    }
  mpn_reciprocal (ip, tp, in);

  np += nn - 2 * dn;
  qp += nn - 2 * dn;

  /* Compute quotient limbs until there are less than dn remaining to be
     computed.  */
  while (nn >= 2 * dn)
    {
      mp_limb_t cy;
      mpn_mul_n (tp, np + dn, ip, dn);
      mpn_add_n (wp, tp + dn, np + dn, dn);	/* prel quotient */
      mpn_mul_n (tp, wp, dp, dn);
      cy = mpn_sub_n (np, np, tp, 2 * dn);	/* prel remainder */
      if (cy)
	{
	  mp_limb_t cy;
	  /* Estimated quotient was too large.  */
	  /* adjust remainder */
	  cy = mpn_add_n (np, np, dp, dn);
	  mpn_incr_u (np + dn, cy);
	  /* adjust quotient */
	  mpn_decr_u (wp, (mp_limb_t) 1);
	}
      else while (np[dn] != 0 || mpn_cmp (np, dp, dn) >= 0)
	{
	  mp_limb_t cy;
	  /* Estimated quotient was too small.  */
	  /* adjust remainder */
	  cy = mpn_sub_n (np, np, dp, dn);
	  mpn_decr_u (np + dn, cy);
	  /* adjust quotient */
	  mpn_incr_u (wp, (mp_limb_t) 1);
	}
      /* Copy quotient in place.  */
      MPN_COPY (qp, wp, dn);
      qp -= dn;
      np -= dn;
      nn -= dn;
    }

  /* True: dn <= nn < 2*dn */
  /* Compute the last (less than dn) quotient limbs.  */
  qn = nn - dn;
  if (qn != 0)
    {
      mp_limb_t cy;
      qp += 2 * dn - nn;
      np += 2 * dn - nn;
      mpn_mul_n (tp, ip + in - qn, np + dn, qn);
      mpn_add_n (wp, tp + qn, np + dn, qn);	/* prel quotient */
      mpn_mul (tp, dp, dn, wp, qn);
      cy = mpn_sub_n (np, np, tp, nn);		/* prel remainder */
      if (cy)
	{
	  do
	    {
	      /* adjust remainder */
	      cy = mpn_add_n (np, np, dp, dn);
	      cy = !mpn_add_1 (np + dn, np + dn, qn, cy);
	      /* adjust quotient */
	      mpn_decr_u (wp, (mp_limb_t) 1);
	    }
	  while (cy);
	}
      else while (np[dn] != 0 || mpn_cmp (np, dp, dn) >= 0)
	{
	  mp_limb_t cy;
	  /* adjust remainder */
	  cy = mpn_sub_n (np, np, dp, dn);
	  mpn_decr_u (np + dn, cy);
	  /* adjust quotient */
	  mpn_incr_u (wp, (mp_limb_t) 1);
	}
      MPN_COPY (qp, wp, qn);
    }

  if (np_realloc_flag)
    MPN_COPY (np_orig, np_new, nn_orig);

  TMP_FREE (marker);
  return most_significant_q_limb;
}
