/* mpn_divrem_classic -- Divide natural numbers, producing both remainder and
   quotient.

   THIS FILE CONTAINS INTERNAL FUNCTIONS WITH MUTABLE INTERFACES.  IT IS
   ONLY SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS
   ALMOST GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP
   RELEASE.


Copyright (C) 1993, 1994, 1995, 1996, 1997, 1999, 2000 Free Software
Foundation, Inc.

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

/* Divide num (NP/NSIZE) by den (DP/DSIZE) and write
   the NSIZE-DSIZE least significant quotient limbs at QP
   and the DSIZE long remainder at NP.  If QEXTRA_LIMBS is
   non-zero, generate that many fraction bits and append them after the
   other quotient limbs.
   Return the most significant limb of the quotient, this is always 0 or 1.

   Preconditions:
   0. NSIZE >= DSIZE.
   1. The most significant bit of the divisor must be set.
   2. QP must either not overlap with the input operands at all, or
      QP + DSIZE >= NP must hold true.  (This means that it's
      possible to put the quotient in the high part of NUM, right after the
      remainder in NUM.
   3. NSIZE >= DSIZE, even if QEXTRA_LIMBS is non-zero.
   4. DSIZE >= 2.  */


mp_limb_t
#if __STDC__
mpn_divrem_classic (mp_ptr qp, mp_size_t qxn,
	    mp_ptr np, mp_size_t nsize,
	    mp_srcptr dp, mp_size_t dsize)
#else
mpn_divrem_classic (qp, qxn, np, nsize, dp, dsize)
     mp_ptr qp;
     mp_size_t qxn;
     mp_ptr np;
     mp_size_t nsize;
     mp_srcptr dp;
     mp_size_t dsize;
#endif
{
  mp_limb_t most_significant_q_limb = 0;
  mp_size_t i;
  mp_limb_t dx, d1, n0;
  mp_limb_t dxinv;
  int have_preinv;

  np += nsize - dsize;
  dx = dp[dsize - 1];
  d1 = dp[dsize - 2];
  n0 = np[dsize - 1];

  if (n0 >= dx)
    {
      if (n0 > dx || mpn_cmp (np, dp, dsize - 1) >= 0)
	{
	  mpn_sub_n (np, np, dp, dsize);
	  n0 = np[dsize - 1];
	  most_significant_q_limb = 1;
	}
    }

  /* If multiplication is much faster than division, preinvert the
	   most significant divisor limb before entering the loop.  */
  if (UDIV_TIME > 2 * UMUL_TIME + 6)
    {
      have_preinv = 0;
      if ((UDIV_TIME - (2 * UMUL_TIME + 6)) * (nsize - dsize) > UDIV_TIME)
	{
	  invert_limb (dxinv, dx);
	  have_preinv = 1;
	}
    }

  for (i = qxn + nsize - dsize - 1; i >= 0; i--)
    {
      mp_limb_t q;
      mp_limb_t nx;
      mp_limb_t cy_limb;

      nx = np[dsize - 1];
      if (i >= qxn)
	{
	  np--;
	}
      else
	{
	  MPN_COPY_DECR (np + 1, np, dsize - 1);
	  np[0] = 0;
	}

      if (nx == dx)
	/* This might over-estimate q, but it's probably not worth
	   the extra code here to find out.  */
	q = ~(mp_limb_t) 0;
      else
	{
	  mp_limb_t r, p1, p0;

	  if (UDIV_TIME > 2 * UMUL_TIME + 6 && have_preinv)
	    {
	      mp_limb_t  nl = np[dsize - 1];  /* avoid gcc 2.7.2.3 bug */
	      udiv_qrnnd_preinv (q, r, nx, nl, dx, dxinv);
	    }
	  else
	    udiv_qrnnd (q, r, nx, np[dsize - 1], dx);
	  umul_ppmm (p1, p0, d1, q);

#if 0
	  while (p1 > r || (p1 == r && p0 > np[dsize - 2]))
	    {
	      q--;
	      r += dx;
	      if (r < dx)	/* I.e. "carry in previous addition?"  */
		break;
	      p1 -= p0 < d1;
	      p0 -= d1;
	    }
#else
	  q -= p1 > r || (p1 == r && p0 > np[dsize - 2]);
#endif
	}

      /* Possible optimization: We already have (q * nx) and (1 * n1)
	 after the calculation of q.  Taking advantage of that, we
	 could make this loop make two iterations less.  */

      cy_limb = mpn_submul_1 (np, dp, dsize, q);

      if (nx != cy_limb)
	{
	  mpn_add_n (np, np, dp, dsize);
	  q--;
	}

      qp[i] = q;
    }

  return most_significant_q_limb;
}
