/* mpn_divexact_by3 -- mpn division by 3, expecting no remainder. */

/*
Copyright (C) 2000 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/


#include "gmp.h"
#include "gmp-impl.h"


#define MP_LIMB_T_MAX  (~((mp_limb_t) 0))

/* Multiplicative inverse of 3, modulo 2^BITS_PER_MP_LIMB.
   0xAAAAAAAB for 32 bits, 0xAAAAAAAAAAAAAAAB for 64 bits. */
#define INVERSE_3      ((MP_LIMB_T_MAX / 3) * 2 + 1)


/* Divide src,size by 3 and store the quotient in dst,size.  If src,size
   isn't exactly divisible by 3 the result in dst,size won't be very useful.
   The return value is 0 if src,size was divisible by 3, or non-zero if not.

   This code will be better than the equivalent mpn_divmod_1 for any CPUs
   with a fast mul but slowish div.

   The c += is adding the high word of 3*l to c.  That high word will be 0,
   1 or 2.

   The quotient q and return value r satisfy r*b^n+a == 3*q, where a is the
   dividend, n is the size of a and q, and b is the limb size, 2^32 or 2^64.
   Clearly when r==0 this gives q==a/3, but the actual result when r!=0
   isn't documented at the moment.  */

mp_limb_t
#if __STDC__
mpn_divexact_by3 (mp_ptr dst, mp_srcptr src, mp_size_t size)
#else
mpn_divexact_by3 (dst, src, size)
     mp_ptr    dst;
     mp_srcptr src;
     mp_size_t size;
#endif
{
  mp_size_t  i;
  mp_limb_t  c, l;

  ASSERT (size >= 1);

  c = 0;
  i = 0;
  do
    {
      l = src[i] - c;
      c = (l > src[i]);

      l *= INVERSE_3;
      dst[i] = l;

      c += (l > MP_LIMB_T_MAX/3) + (l > (MP_LIMB_T_MAX/3)*2);
    }
  while (++i < size);

  return c;
}
