/* mpn_brandom -- Generate random numbers with relatively long strings
   of ones and zeroes.  Suitable for border testing.

Copyright (C) 1992, 1993, 1994, 1996, 2000 Free Software Foundation, Inc.

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

/* It's a bit tricky to get this right, so please test the code well
   if you hack with it.  Some early versions of the function produced
   random numbers with the leading limb == 0, and some versions never
   made the most significant bit set.  */

#define BITS_PER_CHUNK 4

void
mpn_brandom (rp, rstate, nbits)
     mp_ptr rp;
     gmp_rand_state rstate;
     mp_size_t nbits;
{
  int nb;
  int bit_pos;
  mp_size_t limb_pos;
  mp_limb_t ran, ranm;
  mp_limb_t acc;
  mp_size_t n;

  bit_pos = nbits % BITS_PER_MP_LIMB;
  limb_pos = nbits / BITS_PER_MP_LIMB;
  if (bit_pos == 0)
    {
      bit_pos = BITS_PER_MP_LIMB;
      limb_pos--;
    }

  acc = 0;
  while (limb_pos >= 0)
    {
      mpn_rawrandom (&ranm, rstate, BITS_PER_CHUNK + 1);
      ran = ranm;
      nb = (ran >> 1) + 1;
      if ((ran & 1) != 0)
	{
	  /* Generate a string of ones.  */
	  if (nb > bit_pos)
	    {
	      rp[limb_pos--] = acc | ((((mp_limb_t) 1) << bit_pos) - 1);
	      bit_pos += BITS_PER_MP_LIMB;
	      bit_pos -= nb;
	      acc = (~(mp_limb_t) 0) << bit_pos;
	    }
	  else
	    {
	      bit_pos -= nb;
	      acc |= ((((mp_limb_t) 1) << nb) - 1) << bit_pos;
	    }
	}
      else
	{
	  /* Generate a string of zeroes.  */
	  if (nb > bit_pos)
	    {
	      rp[limb_pos--] = acc;
	      acc = 0;
	      bit_pos += BITS_PER_MP_LIMB;
	    }
	  bit_pos -= nb;
	}
    }
}
