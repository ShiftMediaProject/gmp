/* mpn/generic/gcd.c accel GCD find_a() made available for measuring.  */

/*
Copyright 1999, 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#include "speed.h"


/* This code is copied from mpn/generic/gcd.c, there's no global entrypoint
   for it, being inlined. */


static
#if ! defined (__i386__)
__gmp_inline			/* don't inline this for the x86 */
#endif
mp_limb_t
#if __STDC__
find_a (mp_srcptr cp)
#else
find_a (cp)
     mp_srcptr cp;
#endif
{
  unsigned long int leading_zero_bits = 0;

  mp_limb_t n1_l = cp[0];	/* N1 == n1_h * 2^BITS_PER_MP_LIMB + n1_l.  */
  mp_limb_t n1_h = cp[1];

  mp_limb_t n2_l = -n1_l;	/* N2 == n2_h * 2^BITS_PER_MP_LIMB + n2_l.  */
  mp_limb_t n2_h = ~n1_h;

  /* Main loop.  */
  while (n2_h)			/* While N2 >= 2^BITS_PER_MP_LIMB.  */
    {
      /* N1 <-- N1 % N2.  */
      if ((MP_LIMB_T_HIGHBIT >> leading_zero_bits & n2_h) == 0)
	{
	  unsigned long int i;
	  count_leading_zeros (i, n2_h);
	  i -= leading_zero_bits, leading_zero_bits += i;
	  n2_h = n2_h<<i | n2_l>>(BITS_PER_MP_LIMB - i), n2_l <<= i;
	  do
	    {
	      if (n1_h > n2_h || (n1_h == n2_h && n1_l >= n2_l))
		n1_h -= n2_h + (n1_l < n2_l), n1_l -= n2_l;
	      n2_l = n2_l>>1 | n2_h<<(BITS_PER_MP_LIMB - 1), n2_h >>= 1;
	      i -= 1;
	    }
	  while (i);
	}
      if (n1_h > n2_h || (n1_h == n2_h && n1_l >= n2_l))
	n1_h -= n2_h + (n1_l < n2_l), n1_l -= n2_l;

      MP_LIMB_T_SWAP (n1_h, n2_h);
      MP_LIMB_T_SWAP (n1_l, n2_l);
    }

  return n2_l;
}


double
speed_find_a (struct speed_params *s)
{
  SPEED_ROUTINE_MPN_GCD_FINDA (find_a)
}
