/* mpn_mu_bdiv_q(qp,np,nn,dp,dn,tp) -- Compute {np,nn} / {dp,dn} mod B^nn.
   storing the result in {qp,nn}.  Overlap allowed between Q and N; all other
   overlap disallowed.

   Contributed to the GNU project by Torbjorn Granlund.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY WILL CHANGE OR DISAPPEAR IN A FUTURE GMP RELEASE.

Copyright 2005, 2006, 2007, 2009 Free Software Foundation, Inc.

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


/* We use the "misunderstanding algorithm" (MU), discovered by Paul Zimmermann
   and Torbjorn Granlund when Torbjorn misunderstood Paul's explanation of
   Jebelean's bidirectional exact division algorithm.

   The idea of this algorithm is to compute a smaller inverted value than used
   in the standard Barrett algorithm, and thus save time in the Newton
   iterations, and pay just a small price when using the inverted value for
   developing quotient bits.

   Written by Torbjorn Granlund.  Paul Zimmermann suggested the use of the
   "wrap around" trick.
*/

#include "gmp.h"
#include "gmp-impl.h"


/* N = {np,nn}
   D = {dp,dn}

   Requirements: N >= D
		 D >= 1
		 D odd
		 dn >= 2
		 nn >= 2
		 scratch space as determined by mpn_mu_bdiv_q_itch(nn,dn).

   Write quotient to Q = {qp,nn}.

   FIXME: When iterating, perhaps do the small step before loop, not after.
   FIXME: Try to avoid the scalar divisions when computing inverse size.
   FIXME: Trim allocation for (qn > dn) case, 3*dn might be possible.  In
	  particular, when dn==in, tp and rp could use the same space.
   FIXME: Trim final quotient calculation to qn limbs of precision.
*/
void
mpn_mu_bdiv_q (mp_ptr qp,
	       mp_srcptr np, mp_size_t nn,
	       mp_srcptr dp, mp_size_t dn,
	       mp_ptr scratch)
{
  mp_ptr ip;
  mp_ptr rp;
  mp_size_t qn;
  mp_size_t in;

  qn = nn;

  ASSERT (dn >= 2);
  ASSERT (qn >= 2);

  if (qn > dn)
    {
      mp_size_t b;
      mp_ptr tp;
      mp_limb_t cy;
      int k;
      mp_size_t m, wn;
      mp_size_t i;

      /* |_______________________|   dividend
			|________|   divisor  */

      /* Compute an inverse size that is a nice partition of the quotient.  */
      b = (qn - 1) / dn + 1;	/* ceil(qn/dn), number of blocks */
      in = (qn - 1) / b + 1;	/* ceil(qn/b) = ceil(qn / ceil(qn/dn)) */

      /* Some notes on allocation:

	 When in = dn, R dies when mpn_mullo returns, if in < dn the low in
	 limbs of R dies at that point.  We could save memory by letting T live
	 just under R, and let the upper part of T expand into R. These changes
	 should reduce itch to perhaps 3dn.
       */

      ip = scratch;		/* in limbs */
      rp = scratch + in;	/* MAX(binvert_itch(in),dn) limbs */
      tp = scratch + in + dn;	/* MAX(next_size(dn,k),dn+in) limbs */

      mpn_binvert (ip, dp, in, rp);

      cy = 0;

      MPN_COPY (rp, np, dn);
      np += dn;
      mpn_mullo_n (qp, rp, ip, in);
      qn -= in;

#if WANT_FFT
      if (ABOVE_THRESHOLD (dn, MUL_FFT_MODF_THRESHOLD))
	{
	  k = mpn_fft_best_k (dn, 0);
	  m = mpn_fft_next_size (dn, k);
	  wn = dn + in - m;			/* number of wrapped limbs */
	  ASSERT_ALWAYS (wn >= 0);		/* could handle this below */
	}
#endif

      while (qn > in)
	{
#if WANT_FFT
	  if (ABOVE_THRESHOLD (dn, MUL_FFT_MODF_THRESHOLD))
	    {
	      /* The two multiplicands are dn and 'in' limbs, with dn >= in.
		 The relevant part of the result will typically partially wrap,
		 and that part will come out as subtracted to the right.  The
		 unwrapped part, m-in limbs at the high end of tp, is the lower
		 part of the sought product.  The wrapped part, at the low end
		 of tp, will be subtracted from the low part of the partial
		 remainder; we undo that operation with another subtraction. */
	      int c0;

	      c0 = mpn_mul_fft (tp, m, dp, dn, qp, in, k);
	      ASSERT_ALWAYS (c0 == 0);

	      c0 = mpn_sub_n (tp + m, rp, tp, wn);

	      for (i = wn; c0 != 0 && i < in; i++)
		c0 = tp[i] == GMP_NUMB_MASK;
	      mpn_incr_u (tp + in, c0);
	    }
	  else
#endif
	    mpn_mul (tp, dp, dn, qp, in);	/* mulhi, need tp[dn+in-1...in] */
	  qp += in;
	  if (dn != in)
	    {
	      /* Subtract tp[dn-1...in] from partial remainder.  */
	      cy += mpn_sub_n (rp, rp + in, tp + in, dn - in);
	      if (cy == 2)
		{
		  mpn_incr_u (tp + dn, 1);
		  cy = 1;
		}
	    }
	  /* Subtract tp[dn+in-1...dn] from dividend.  */
	  cy = mpn_sub_nc (rp + dn - in, np, tp + dn, in, cy);
	  np += in;
	  mpn_mullo_n (qp, rp, ip, in);
	  qn -= in;
	}

      /* Generate last qn limbs.
	 FIXME: It should be possible to limit precision here, since qn is
	 typically somewhat smaller than dn.  No big gains expected.  */
#if WANT_FFT
      if (ABOVE_THRESHOLD (dn, MUL_FFT_MODF_THRESHOLD))
	{
	  int c0;

	  c0 = mpn_mul_fft (tp, m, dp, dn, qp, in, k);
	  ASSERT_ALWAYS (c0 == 0);

	  c0 = mpn_sub_n (tp + m, rp, tp, wn);

	  for (i = wn; c0 != 0 && i < in; i++)
	    c0 = tp[i] == GMP_NUMB_MASK;
	  mpn_incr_u (tp + in, c0);
	}
      else
#endif
	mpn_mul (tp, dp, dn, qp, in);		/* mulhi, need tp[qn+in-1...in] */
      qp += in;
      if (dn != in)
	{
	  cy += mpn_sub_n (rp, rp + in, tp + in, dn - in);
	  if (cy == 2)
	    {
	      mpn_incr_u (tp + dn, 1);
	      cy = 1;
	    }
	}

      mpn_sub_nc (rp + dn - in, np, tp + dn, qn - (dn - in), cy);
      mpn_mullo_n (qp, rp, ip, qn);
    }
  else
    {
      /* |_______________________|   dividend
		|________________|   divisor  */

      /* Compute half-sized inverse.  */
      in = qn - (qn >> 1);

      ip = scratch;		/* in limbs */
      rp = scratch + in;	/* MAX(binvert_itch(in),next_size(qn,k),qn+in) limbs */

      mpn_binvert (ip, dp, in, rp);

      mpn_mullo_n (qp, np, ip, in);		/* low `in' quotient limbs */
#if WANT_FFT
      if (ABOVE_THRESHOLD (qn, MUL_FFT_MODF_THRESHOLD))
	{
	  int k;
	  mp_size_t m;
	  int c0;

	  k = mpn_fft_best_k (qn, 0);
	  m = mpn_fft_next_size (qn, k);
	  c0 = mpn_mul_fft (rp, m, dp, qn, qp, in, k);
	  ASSERT_ALWAYS (c0 == 0);
	  if (mpn_cmp (np, rp, in) < 0)
	    mpn_incr_u (rp + in, 1);
	}
      else
#endif
	mpn_mul (rp, dp, qn, qp, in);		/* mulhigh */

      mpn_sub_n (rp, np + in, rp + in, qn - in);
      mpn_mullo_n (qp + in, rp, ip, qn - in);	/* high qn-in quotient limbs */
    }
}

mp_size_t
mpn_mu_bdiv_q_itch (mp_size_t nn, mp_size_t dn)
{
  mp_size_t qn, in, m, itch_invert;
  mp_size_t b;

  qn = nn;

  if (qn > dn)
    {
      b = (qn - 1) / dn + 1;	/* ceil(qn/dn), number of blocks */
      in = (qn - 1) / b + 1;	/* ceil(qn/b) = ceil(qn / ceil(qn/dn)) */
#if WANT_FFT
      if (ABOVE_THRESHOLD (dn, MUL_FFT_MODF_THRESHOLD))
        {
          m = mpn_fft_next_size (dn, mpn_fft_best_k (dn, 0));
          m = MAX (dn + in, m);
        }
      else
#endif
	m = dn + in;
      itch_invert = mpn_binvert_itch (in);
      return in + MAX (itch_invert, dn + m);
    }
  else
    {
      in = qn - (qn >> 1);
#if WANT_FFT
      if (ABOVE_THRESHOLD (qn, MUL_FFT_MODF_THRESHOLD))
        {
          m = mpn_fft_next_size (qn, mpn_fft_best_k (qn, 0));
          m = MAX (qn + in, m);
        }
      else
#endif
        m = qn + in;
      itch_invert = mpn_binvert_itch (in);
      return in + MAX (itch_invert, m);
    }
}
