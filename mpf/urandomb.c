/* mpf_urandomb (rop, state, nbits) -- Generate a uniform pseudorandom
   real number between 0 (inclusive) and 1 (exclusive) of size NBITS,
   using STATE as the random state previously initialized by a call to
   gmp_randinit().

Copyright (C) 1999, 2000  Free Software Foundation, Inc.

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
mpf_urandomb (mpf_t rop, gmp_randstate_t rstate, unsigned long int nbits)
#else
mpf_urandomb (rop, rstate, nbits)
     mpf_t rop;
     gmp_randstate_t rstate;
     unsigned long int nbits;
#endif
{
  mp_ptr rp;
  mp_size_t nlimbs;
  mp_exp_t exp;

  rp = PTR (rop);
  nlimbs = (nbits + BITS_PER_MP_LIMB - 1) / BITS_PER_MP_LIMB;

  _gmp_rand (rp, rstate, nbits);

  /* If nbits isn't a multiple of BITS_PER_MP_LIMB, shift up.  */
  if (nlimbs != 0)
    {
      if (nbits % BITS_PER_MP_LIMB != 0)
	mpn_lshift (rp, rp, nlimbs,
		    BITS_PER_MP_LIMB - nbits % BITS_PER_MP_LIMB);
    }

  exp = 0;
  while (nlimbs != 0 && rp[nlimbs - 1] == 0)
    {
      nlimbs--;
      exp--;
    }
  EXP (rop) = exp;
  SIZ (rop) = nlimbs;

}
