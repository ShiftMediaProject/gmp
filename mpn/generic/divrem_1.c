/* mpn_divrem_1(quot_ptr, qsize, dividend_ptr, dividend_size, divisor_limb) --
   Divide (DIVIDEND_PTR,,DIVIDEND_SIZE) by DIVISOR_LIMB.
   Write DIVIDEND_SIZE limbs of quotient at QUOT_PTR.
   Return the single-limb remainder.
   There are no constraints on the value of the divisor.

   QUOT_PTR and DIVIDEND_PTR might point to the same limb.

Copyright (C) 1996, 1998, 1999, 2000 Free Software Foundation, Inc.

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

mp_limb_t
#if __STDC__
mpn_divrem_1 (mp_ptr qp, mp_size_t qxn,
	      mp_srcptr np, mp_size_t nn,
	      mp_limb_t d)
#else
mpn_divrem_1 (qp, qxn, np, nn, d)
     mp_ptr qp;
     mp_size_t qxn;
     mp_srcptr np;
     mp_size_t nn;
     mp_limb_t d;
#endif
{
  mp_limb_t rlimb;
  mp_size_t i;

  /* Develop integer part of quotient.  */
  rlimb = mpn_divmod_1 (qp + qxn, np, nn, d);

  /* Develop fraction part of quotient.  This is not as fast as it should;
     the preinvert stuff from mpn_divmod_1 ought to be used here too.  */
  if (UDIV_NEEDS_NORMALIZATION)
    {
      int normalization_steps;

      count_leading_zeros (normalization_steps, d);
      if (normalization_steps != 0)
	{
	  d <<= normalization_steps;
	  rlimb <<= normalization_steps;

	  for (i = qxn - 1; i >= 0; i--)
	    udiv_qrnnd (qp[i], rlimb, rlimb, 0, d);

	  return rlimb >> normalization_steps;
	}
      else
	/* fall out */
	;
    }

  for (i = qxn - 1; i >= 0; i--)
    udiv_qrnnd (qp[i], rlimb, rlimb, 0, d);

  return rlimb;
}
