/* mpn_mul -- Multiply two natural numbers.

Copyright (C) 1991, 1993, 1994, 1996 Free Software Foundation, Inc.

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

/* Multiply the natural numbers u (pointed to by UP, with USIZE limbs) and v
   (pointed to by VP, with VSIZE limbs), and store the result at PRODP.  The
   result is USIZE + VSIZE limbs.  Return the most significant limb of the
   result.

   NOTE: The space pointed to by PRODP is overwritten before finished
   with U and V, so overlap is an error.

   Argument constraints:
   1. USIZE >= VSIZE.
   2. PRODP != UP and PRODP != VP, i.e. the destination
      must be distinct from the multiplier and the multiplicand.  */

mp_limb_t
#if __STDC__
mpn_mul (mp_ptr prodp,
	 mp_srcptr up, mp_size_t usize,
	 mp_srcptr vp, mp_size_t vsize)
#else
mpn_mul (prodp, up, usize, vp, vsize)
     mp_ptr prodp;
     mp_srcptr up;
     mp_size_t usize;
     mp_srcptr vp;
     mp_size_t vsize;
#endif
{
  mp_ptr prod_endp = prodp + usize + vsize - 1;
  TMP_DECL (marker);

  if (up == vp && usize == vsize)
    {
      if (vsize < KARATSUBA_SQR_THRESHOLD)
	{
	  if (vsize == 0)
	    return 0;
	  __gmpn_sqr_basecase (prodp, vp, vsize);
	}
      else
	{
	  mp_ptr tspace;
	  TMP_MARK (marker);
	  tspace = (mp_ptr) TMP_ALLOC (2 * (vsize + BITS_PER_MP_LIMB) * BYTES_PER_MP_LIMB);
	  __gmpn_sqr (prodp, vp, vsize, tspace);
	  TMP_FREE (marker);
	}
      return *prod_endp;
    }

  if (vsize < KARATSUBA_MUL_THRESHOLD)
    {
      __gmpn_mul_basecase (prodp, up, usize, vp, vsize);
      return *prod_endp;
    }

  TMP_MARK (marker);

  /* The way of multiplying numbers leads to poor performance when the size of
     U is Fib(n) and the size of V is Fib(n-1)...  */
  {
    mp_ptr tspace;
    tspace = (mp_ptr) TMP_ALLOC (2 * (vsize + BITS_PER_MP_LIMB) * BYTES_PER_MP_LIMB);
    __gmpn_mul_n (prodp, up, vp, vsize, tspace);

    prodp += vsize;
    up += vsize;
    usize -= vsize;
    if (usize >= vsize)
      {
	mp_ptr tp = (mp_ptr) TMP_ALLOC (2 * vsize * BYTES_PER_MP_LIMB);
	do
	  {
	    mp_limb_t cy;
	    __gmpn_mul_n (tp, up, vp, vsize, tspace);
	    cy = mpn_add_n (prodp, prodp, tp, vsize);
	    mpn_add_1 (prodp + vsize, tp + vsize, vsize, cy);
	    prodp += vsize;
	    up += vsize;
	    usize -= vsize;
	  }
	while (usize >= vsize);
      }

    /* True: usize < vsize.  */

    /* For simplicity: Recurse.  */

    if (usize != 0)
      {
	mp_limb_t cy;
	mpn_mul (tspace, vp, vsize, up, usize);
	cy = mpn_add_n (prodp, prodp, tspace, vsize);
	mpn_add_1 (prodp + vsize, tspace + vsize, usize, cy);
      }
  }

  TMP_FREE (marker);
  return *prod_endp;
}
