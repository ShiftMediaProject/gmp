/* mpn_mul -- Multiply two natural numbers.

   Contributed to the GNU project by Torbjorn Granlund.

Copyright 1991, 1993, 1994, 1996, 1997, 1999, 2000, 2001, 2002, 2003, 2005,
2006, 2007 Free Software Foundation, Inc.

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

  if (un == vn)
    {
      if (up == vp)
	mpn_sqr_n (prodp, up, un);
      else
	mpn_mul_n (prodp, up, vp, un);
      return prodp[2 * un - 1];
    }

  if (vn < MUL_KARATSUBA_THRESHOLD)
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

	  mp_limb_t tp[MUL_KARATSUBA_THRESHOLD_LIMIT];
	  mp_limb_t cy;
          ASSERT (MUL_KARATSUBA_THRESHOLD <= MUL_KARATSUBA_THRESHOLD_LIMIT);

	  mpn_mul_basecase (prodp, up, MUL_BASECASE_MAX_UN, vp, vn);
	  prodp += MUL_BASECASE_MAX_UN;
	  MPN_COPY (tp, prodp, vn);		/* preserve high triangle */
	  up += MUL_BASECASE_MAX_UN;
	  un -= MUL_BASECASE_MAX_UN;
	  while (un > MUL_BASECASE_MAX_UN)
	    {
	      mpn_mul_basecase (prodp, up, MUL_BASECASE_MAX_UN, vp, vn);
	      cy = mpn_add_n (prodp, prodp, tp, vn); /* add back preserved triangle */
	      mpn_incr_u (prodp + vn, cy);		/* safe? */
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
	  mpn_incr_u (prodp + vn, cy);		/* safe? */
	}
      return prodp[un + vn - 1];
    }

  if (ABOVE_THRESHOLD ((un + vn) >> 1, MUL_FFT_THRESHOLD) &&
      ABOVE_THRESHOLD (vn, MUL_FFT_THRESHOLD / 3)) /* FIXME */
    {
      mpn_mul_fft_full (prodp, up, un, vp, vn);
      return prodp[un + vn - 1];
    }

  if (un >= 3 * vn)
    {
      mp_ptr ws;
      mp_limb_t cy;
      TMP_DECL;

      mpn_mul_toom42 (prodp, up, 2 * vn, vp, vn);
      un -= 2 * vn;
      up += 2 * vn;
      prodp += 2 * vn;

      TMP_MARK;
      ws = TMP_SALLOC_LIMBS (4 * vn);		/* FIXME */

      while (un >= 3 * vn)
	{
	  mpn_mul_toom42 (ws, up, 2 * vn, vp, vn);
	  un -= 2 * vn;
	  up += 2 * vn;
	  cy = mpn_add_n (prodp, prodp, ws, vn);
	  MPN_COPY (prodp + vn, ws + vn, 2 * vn);
	  mpn_incr_u (prodp + vn, cy);
	  prodp += 2 * vn;
	}

      if (5 * un > 9 * vn)
	{
	  mpn_mul_toom42 (ws, up, un, vp, vn);
	  cy = mpn_add_n (prodp, prodp, ws, vn);
	  MPN_COPY (prodp + vn, ws + vn, un);
	  mpn_incr_u (prodp + vn, cy);
	}
      else if (9 * un > 10 * vn)
	{
	  mpn_mul_toom32 (ws, up, un, vp, vn);
	  cy = mpn_add_n (prodp, prodp, ws, vn);
	  MPN_COPY (prodp + vn, ws + vn, un);
	  mpn_incr_u (prodp + vn, cy);
	}
      else
	{
	  mp_ptr scratch = ws + (un + vn);
	  mpn_mul_toom22 (ws, up, un, vp, vn, scratch);
	  cy = mpn_add_n (prodp, prodp, ws, vn);
	  MPN_COPY (prodp + vn, ws + vn, un);
	  mpn_incr_u (prodp + vn, cy);
	}
      TMP_FREE;
      return prodp[un + vn - 1];
    }

  if (un * 5 > vn * 9)
    mpn_mul_toom42 (prodp, up, un, vp, vn);
  else if (9 * un > 10 * vn)
    mpn_mul_toom32 (prodp, up, un, vp, vn);
  else
    {
      mp_ptr scratch;
      TMP_DECL; TMP_MARK;
      scratch = TMP_SALLOC_LIMBS (un + vn);
      mpn_mul_toom22 (prodp, up, un, vp, vn, scratch);
      TMP_FREE;
    }

  return prodp[un + vn - 1];
}
