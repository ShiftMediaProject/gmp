/* mpn_mul -- Multiply two natural numbers.

   Contributed to the GNU project by Torbjorn Granlund.

Copyright 1991, 1993, 1994, 1996, 1997, 1999, 2000, 2001, 2002, 2003, 2005,
2006, 2007, 2009 Free Software Foundation, Inc.

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


#ifndef MUL_BASECASE_MAX_UN
#define MUL_BASECASE_MAX_UN 500
#endif

#define TOOM33_OK(an,bn) (6 + 2 * an < 3 * bn)
#define TOOM44_OK(an,bn) (12 + 3 * an < 4 * bn)

/* Multiply the natural numbers u (pointed to by UP, with UN limbs) and v
   (pointed to by VP, with VN limbs), and store the result at PRODP.  The
   result is UN + VN limbs.  Return the most significant limb of the result.

   NOTE: The space pointed to by PRODP is overwritten before finished with U
   and V, so overlap is an error.

   Argument constraints:
   1. UN >= VN.
   2. PRODP != UP and PRODP != VP, i.e. the destination must be distinct from
      the multiplier and the multiplicand.  */

mp_limb_t
mpn_mul (mp_ptr prodp,
	 mp_srcptr up, mp_size_t un,
	 mp_srcptr vp, mp_size_t vn)
{
  ASSERT (un >= vn);
  ASSERT (vn >= 1);
  ASSERT (! MPN_OVERLAP_P (prodp, un+vn, up, un));
  ASSERT (! MPN_OVERLAP_P (prodp, un+vn, vp, vn));

  if (up == vp && un == vn)
    {
      mpn_sqr_n (prodp, up, un);
    }
  else if (vn < MUL_TOOM22_THRESHOLD)
    { /* plain schoolbook multiplication */

      /* Unless un is very large, or else if have an applicable mpn_mul_N,
	 perform basecase multiply directly.  */
      if (un <= MUL_BASECASE_MAX_UN
#if HAVE_NATIVE_mpn_mul_2
	  || vn <= 2
#else
	  || vn == 1
#endif
	  )
	mpn_mul_basecase (prodp, up, un, vp, vn);
      else
	{
	  /* We have un >> MUL_BASECASE_MAX_UN > vn.  For better memory
	     locality, split up[] into MUL_BASECASE_MAX_UN pieces and multiply
	     these pieces with the vp[] operand.  After each such partial
	     multiplication (but the last) we copy the most significant vn
	     limbs into a temporary buffer since that part would otherwise be
	     overwritten by the next multiplication.  After the next
	     multiplication, we add it back.  This illustrates the situation:

                                                    -->vn<--
                                                      |  |<------- un ------->|
                                                         _____________________|
                                                        X                    /|
                                                      /XX__________________/  |
                                    _____________________                     |
                                   X                    /                     |
                                 /XX__________________/                       |
               _____________________                                          |
              /                    /                                          |
            /____________________/                                            |
	    ==================================================================

	    The parts marked with X are the parts whose sums are copied into
	    the temporary buffer.  */

	  mp_limb_t tp[MUL_TOOM22_THRESHOLD_LIMIT];
	  mp_limb_t cy;
          ASSERT (MUL_TOOM22_THRESHOLD <= MUL_TOOM22_THRESHOLD_LIMIT);

	  mpn_mul_basecase (prodp, up, MUL_BASECASE_MAX_UN, vp, vn);
	  prodp += MUL_BASECASE_MAX_UN;
	  MPN_COPY (tp, prodp, vn);		/* preserve high triangle */
	  up += MUL_BASECASE_MAX_UN;
	  un -= MUL_BASECASE_MAX_UN;
	  while (un > MUL_BASECASE_MAX_UN)
	    {
	      mpn_mul_basecase (prodp, up, MUL_BASECASE_MAX_UN, vp, vn);
	      cy = mpn_add_n (prodp, prodp, tp, vn); /* add back preserved triangle */
	      mpn_incr_u (prodp + vn, cy);
	      prodp += MUL_BASECASE_MAX_UN;
	      MPN_COPY (tp, prodp, vn);		/* preserve high triangle */
	      up += MUL_BASECASE_MAX_UN;
	      un -= MUL_BASECASE_MAX_UN;
	    }
	  if (un > vn)
	    {
	      mpn_mul_basecase (prodp, up, un, vp, vn);
	    }
	  else
	    {
	      ASSERT_ALWAYS (un > 0);
	      mpn_mul_basecase (prodp, vp, vn, up, un);
	    }
	  cy = mpn_add_n (prodp, prodp, tp, vn); /* add back preserved triangle */
	  mpn_incr_u (prodp + vn, cy);
	}
    }
  else if (BELOW_THRESHOLD (vn, MUL_TOOM33_THRESHOLD) ||
	   /* Also do larger unbalanced here, up to a (somewhat arbitrarily)
	      larger vn limit, unless toom33 can do this product directly.  */
	   (!TOOM33_OK (un, vn) && BELOW_THRESHOLD (vn, MUL_TOOM33_THRESHOLD * 3 / 2)))
    {
      /* Loop over toom42, then choose toom42, toom32, or toom22 */
      mp_ptr ws;
      mp_ptr scratch;
      TMP_DECL; TMP_MARK;

#define WSALL (4 * vn)
      ws = TMP_SALLOC_LIMBS (WSALL + 1);

#define ITCH ((un + vn) * 4 + 100)
      scratch = TMP_ALLOC_LIMBS (ITCH + 1);

      if (un >= 3 * vn)
	{
	  mp_limb_t cy;

	  mpn_toom42_mul (prodp, up, 2 * vn, vp, vn, scratch);
	  un -= 2 * vn;
	  up += 2 * vn;
	  prodp += 2 * vn;

	  while (un >= 3 * vn)
	    {
	      mpn_toom42_mul (ws, up, 2 * vn, vp, vn, scratch);
	      un -= 2 * vn;
	      up += 2 * vn;
	      cy = mpn_add_n (prodp, prodp, ws, vn);
	      MPN_COPY (prodp + vn, ws + vn, 2 * vn);
	      mpn_incr_u (prodp + vn, cy);
	      prodp += 2 * vn;
	    }

	  /* FIXME: Test these in opposite order, following the philosophy of
	     minimizing the relative overhead.  */
	  if (5 * un > 9 * vn)
	    {
	      mpn_toom42_mul (ws, up, un, vp, vn, scratch);
	      cy = mpn_add_n (prodp, prodp, ws, vn);
	      MPN_COPY (prodp + vn, ws + vn, un);
	      mpn_incr_u (prodp + vn, cy);
	    }
	  else if (9 * un > 10 * vn)
	    {
	      mpn_toom32_mul (ws, up, un, vp, vn, scratch);
	      cy = mpn_add_n (prodp, prodp, ws, vn);
	      MPN_COPY (prodp + vn, ws + vn, un);
	      mpn_incr_u (prodp + vn, cy);
	    }
	  else
	    {
	      mpn_toom22_mul (ws, up, un, vp, vn, scratch);
	      cy = mpn_add_n (prodp, prodp, ws, vn);
	      MPN_COPY (prodp + vn, ws + vn, un);
	      mpn_incr_u (prodp + vn, cy);
	    }
	}
      else
	{
	  /* FIXME: Test these in opposite order, following the philosophy of
	     minimizing the relative overhead.  */
	  if (5 * un > 9 * vn)
	    mpn_toom42_mul (prodp, up, un, vp, vn, scratch);
	  else if (9 * un > 10 * vn)
	    mpn_toom32_mul (prodp, up, un, vp, vn, scratch);
	  else
	    mpn_toom22_mul (prodp, up, un, vp, vn, scratch);
	}
      TMP_FREE;
    }
  else if (BELOW_THRESHOLD (vn, MUL_TOOM44_THRESHOLD))
    {
      TMP_DECL; TMP_MARK;
      if (TOOM33_OK (un, vn))
	{
	  /* Apply toom33 directly, since operands are balanced enough.  */
	  mp_ptr scratch;
	  scratch = TMP_SALLOC_LIMBS (mpn_toom33_mul_itch (un, vn));
	  mpn_toom33_mul (prodp, up, un, vp, vn, scratch);
	}
      else
	{
	  /* Apply toom33, recurse.  FUTURE: toom63, toom53, toom43, toom33 */
	  mp_ptr scratch, pp;		/* FIXME: use same area for these */
	  mp_limb_t cy;
	  scratch = TMP_SALLOC_LIMBS (mpn_toom33_mul_itch (vn, vn));
	  mpn_toom33_mul (prodp, up, vn, vp, vn, scratch);
	  prodp += vn;
	  up += vn;
	  un -= vn;
	  pp = TMP_SALLOC_LIMBS (2 * vn);
	  while (un >= vn)
	    {
	      mpn_toom33_mul (pp, up, vn, vp, vn, scratch);
	      cy = mpn_add_n (prodp, prodp, pp, vn);
	      MPN_COPY (prodp + vn, pp + vn, vn);
	      mpn_incr_u (prodp + vn, cy);
	      prodp += vn;
	      up += vn;
	      un -= vn;
	    }
	  if (un != 0)
	    {
	      mpn_mul (pp, vp, vn, up, un);
	      cy = mpn_add_n (prodp, prodp, pp, vn);
	      MPN_COPY (prodp + vn, pp + vn, un);
	      mpn_incr_u (prodp + vn, cy);
	    }
	}
      TMP_FREE;
    }
  else if (BELOW_THRESHOLD ((un + vn) >> 1, MUL_FFT_THRESHOLD) ||
	   BELOW_THRESHOLD (vn, MUL_FFT_THRESHOLD / 3)) /* FIXME */
    {
      TMP_DECL; TMP_MARK;
      if (TOOM44_OK (un, vn))
	{
	  /* Apply toom44 directly, since operands are balanced enough.  */
	  mp_ptr scratch;
	  scratch = TMP_ALLOC_LIMBS (mpn_toom44_mul_itch (un, vn));
	  mpn_toom44_mul (prodp, up, un, vp, vn, scratch);
	}
      else
	{
	  /* Apply toom44, recurse.  FUTURE: toom84, toom74, toom64, toom54, toom44 */
	  mp_ptr scratch, pp;		/* FIXME: use same area for these */
	  mp_limb_t cy;
	  scratch = TMP_ALLOC_LIMBS (mpn_toom44_mul_itch (vn, vn));
	  mpn_toom44_mul (prodp, up, vn, vp, vn, scratch);
	  prodp += vn;
	  up += vn;
	  un -= vn;
	  pp = TMP_SALLOC_LIMBS (2 * vn);
	  while (un >= vn)
	    {
	      mpn_toom44_mul (pp, up, vn, vp, vn, scratch);
	      cy = mpn_add_n (prodp, prodp, pp, vn);
	      MPN_COPY (prodp + vn, pp + vn, vn);
	      mpn_incr_u (prodp + vn, cy);
	      prodp += vn;
	      up += vn;
	      un -= vn;
	    }
	  if (un != 0)
	    {
	      mpn_mul (pp, vp, vn, up, un);
	      cy = mpn_add_n (prodp, prodp, pp, vn);
	      MPN_COPY (prodp + vn, pp + vn, un);
	      mpn_incr_u (prodp + vn, cy);
	    }
	}
      TMP_FREE;
    }
  else
    {
      mpn_mul_fft_full (prodp, up, un, vp, vn);
    }
  return prodp[un + vn - 1];
}
