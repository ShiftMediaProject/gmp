/* mpn_mul_basecase -- Internal routine to multiply two natural numbers
   of length m and n.

   THIS IS AN INTERNAL FUNCTION WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH THIS FUNCTION THROUGH DOCUMENTED INTERFACES.


Copyright (C) 1991, 1992, 1993, 1994, 1996, 1997, 2000 Free Software
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

/* Handle simple cases with traditional multiplication.

   This is the most critical code of multiplication.  All multiplies rely on
   this, both small and huge.  Small ones arrive here immediately.  Huge ones
   arrive here as this is the base case for Karatsuba's recursive algorithm.  */

void
#if __STDC__
mpn_mul_basecase (mp_ptr prodp,
		     mp_srcptr up, mp_size_t usize,
		     mp_srcptr vp, mp_size_t vsize)
#else
mpn_mul_basecase (prodp, up, usize, vp, vsize)
     mp_ptr prodp;
     mp_srcptr up;
     mp_size_t usize;
     mp_srcptr vp;
     mp_size_t vsize;
#endif
{
  mp_size_t i;

  /* Multiply by the first limb in V separately, as the result can be
     stored (not added) to PROD.  We also avoid a loop for zeroing.  */
  prodp[usize] = mpn_mul_1 (prodp, up, usize, vp[0]);
  prodp++;

  /* For each iteration in the loop, multiply U with one limb from V, and add
     the result to PROD.  */
  for (i = 1; i < vsize; i++)
    {
      prodp[usize] = mpn_addmul_1 (prodp, up, usize, vp[i]);
      prodp++;
    }
}
