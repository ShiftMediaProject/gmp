/* double mpq_get_d (mpq_t src) -- mpq to double, rounding towards zero.

Copyright 1995, 1996, 2001, 2002, 2003 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/* All that's required is to get the high 53 bits of the quotient num/den,
   rounded towards zero.  More than 53 bits is fine, any excess is ignored
   by mpn_get_d.

   Enhancements:

   With some shifts all we need is 1 or 2 limbs of quotient, ie. 1 or 2
   steps of mpn_sb_divrem_mn, could call that directly instead of going via
   mpn_divmod.

   Use a quotient-only division, when it exists.  Since only 1 or 2 limbs of
   quotient are needed, they could be estimated from the high limbs of num
   and den, followed by a mul+cmp to see if 1 too big, without using any
   temporary memory.  */

double
mpq_get_d (const MP_RAT *src)
{
  double res;
  mp_ptr np, dp;
  mp_ptr rp;
  mp_size_t nsize = src->_mp_num._mp_size;
  mp_size_t dsize = src->_mp_den._mp_size;
  mp_size_t qsize, rsize;
  mp_size_t sign_quotient = nsize ^ dsize;
  mp_limb_t qlimb;
#define N_QLIMBS (1 + (sizeof (double) + BYTES_PER_MP_LIMB-1) / BYTES_PER_MP_LIMB)
  mp_limb_t qarr[N_QLIMBS + 1];
  mp_ptr qp = qarr;
  TMP_DECL (marker);

  if (nsize == 0)
    return 0.0;

  TMP_MARK (marker);
  nsize = ABS (nsize);
  dsize = ABS (dsize);
  np = src->_mp_num._mp_d;
  dp = src->_mp_den._mp_d;

  rsize = dsize + N_QLIMBS;
  rp = (mp_ptr) TMP_ALLOC ((rsize + 1) * BYTES_PER_MP_LIMB);

  /* Normalize the denominator, i.e. make its most significant bit set by
     shifting it NORMALIZATION_STEPS bits to the left.  Also shift the
     numerator the same number of steps (to keep the quotient the same!).  */
  if ((dp[dsize - 1] & GMP_NUMB_HIGHBIT) == 0)
    {
      mp_ptr tp;
      mp_limb_t nlimb;
      unsigned normalization_steps;

      count_leading_zeros (normalization_steps, dp[dsize - 1]);
      normalization_steps -= GMP_NAIL_BITS;

      /* Shift up the denominator setting the most significant bit of
	 the most significant limb.  Use temporary storage not to clobber
	 the original contents of the denominator.  */
      tp = (mp_ptr) TMP_ALLOC (dsize * BYTES_PER_MP_LIMB);
      mpn_lshift (tp, dp, dsize, normalization_steps);
      dp = tp;

      if (rsize > nsize)
	{
	  MPN_ZERO (rp, rsize - nsize);
	  nlimb = mpn_lshift (rp + (rsize - nsize),
			      np, nsize, normalization_steps);
	}
      else
	{
	  nlimb = mpn_lshift (rp, np + (nsize - rsize),
			      rsize, normalization_steps);
	}
      if (nlimb != 0)
	{
	  rp[rsize] = nlimb;
	  rsize++;
	}
    }
  else
    {
      if (rsize > nsize)
	{
	  MPN_ZERO (rp, rsize - nsize);
	  MPN_COPY (rp + (rsize - nsize), np, nsize);
	}
      else
	{
	  MPN_COPY (rp, np + (nsize - rsize), rsize);
	}
    }

  qlimb = mpn_divmod (qp, rp, rsize, dp, dsize);
  qsize = rsize - dsize;
  if (qlimb)
    {
      qp[qsize] = qlimb;
      qsize++;
    }

  MPN_NORMALIZE (qp, qsize);
  res = mpn_get_d (qp, qsize, sign_quotient,
                   (long) (nsize - dsize - N_QLIMBS) * GMP_NUMB_BITS);
  TMP_FREE (marker);
  return res;
}
