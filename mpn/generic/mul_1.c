/* mpn_mul_1 -- Multiply a limb vector with a single limb and
   store the product in a second limb vector.

Copyright 1991, 1992, 1993, 1994, 1996, 2000 Free Software Foundation,
Inc.

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


/* For reference, "incr" style overlaps are wanted in mpf_mul_ui, where an
   mpf_mul_ui(x,x,y) if SIZ(x)>PREC(x) (either SIZ(x)==PREC(x)+1 as occurs
   normally, or due to an mpf_set_prec_raw).  */

mp_limb_t
mpn_mul_1 (mp_ptr res_ptr,
	   mp_srcptr s1_ptr,
	   mp_size_t s1_size,
	   mp_limb_t s2_limb)
{
  register mp_limb_t cy_limb;
  register mp_size_t j;
  register mp_limb_t prod_high, prod_low;

  ASSERT (s1_size >= 1);
  ASSERT (MPN_SAME_OR_INCR_P (res_ptr, s1_ptr, s1_size));

  /* The loop counter and index J goes from -S1_SIZE to -1.  This way
     the loop becomes faster.  */
  j = -s1_size;

  /* Offset the base pointers to compensate for the negative indices.  */
  s1_ptr -= j;
  res_ptr -= j;

  cy_limb = 0;
  do
    {
      umul_ppmm (prod_high, prod_low, s1_ptr[j], s2_limb);

      prod_low += cy_limb;
      cy_limb = (prod_low < cy_limb) + prod_high;

      res_ptr[j] = prod_low;
    }
  while (++j != 0);

  return cy_limb;
}
