/* mpf_mul_2exp -- Multiply a float by 2^n.

Copyright (C) 1993, 1994, 1996, 2000 Free Software Foundation, Inc.

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

void
#if __STDC__
mpf_mul_2exp (mpf_ptr r, mpf_srcptr u, unsigned long int exp)
#else
mpf_mul_2exp (r, u, exp)
     mpf_ptr r;
     mpf_srcptr u;
     unsigned long int exp;
#endif
{
  mp_srcptr up;
  mp_ptr rp = r->_mp_d;
  mp_size_t usize;
  mp_size_t abs_usize;
  mp_size_t prec = r->_mp_prec;
  mp_exp_t uexp = u->_mp_exp;

  usize = u->_mp_size;

  if (usize == 0)
    {
      r->_mp_size = 0;
      r->_mp_exp = 0;
      return;
    }

  abs_usize = ABS (usize);
  up = u->_mp_d;

  if (exp % BITS_PER_MP_LIMB == 0)
    {
      prec++;			/* retain more precision here as we don't need
				   to account for carry-out here */
      if (abs_usize > prec)
	{
	  up += abs_usize - prec;
	  abs_usize = prec;
	}
      if (rp != up)
	MPN_COPY_INCR (rp, up, abs_usize);
      r->_mp_exp = uexp + exp / BITS_PER_MP_LIMB;
    }
  else
    {
      mp_limb_t cy_limb;
      mp_size_t adj;
      if (abs_usize > prec)
	{
	  up += abs_usize - prec;
	  abs_usize = prec;
	  /* Use mpn_rshift since mpn_lshift operates downwards, and we
	     therefore would clobber part of U before using that part, in case
	     R is the same variable as U.  */
	  cy_limb = mpn_rshift (rp + 1, up, abs_usize, (-exp) % BITS_PER_MP_LIMB);
	  rp[0] = cy_limb;
	  adj = rp[abs_usize] != 0;
	}
      else
	{
	  cy_limb = mpn_lshift (rp, up, abs_usize, exp % BITS_PER_MP_LIMB);
	  rp[abs_usize] = cy_limb;
	  adj = cy_limb != 0;
	}

      abs_usize += adj;
      r->_mp_exp = uexp + exp / BITS_PER_MP_LIMB + adj;
    }
  r->_mp_size = usize >= 0 ? abs_usize : -abs_usize;
}
