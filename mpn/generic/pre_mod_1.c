/* mpn_preinv_mod_1 (dividend_ptr, dividend_size, divisor_limb,
		       divisor_limb_inverted) --
   Divide (DIVIDEND_PTR,,DIVIDEND_SIZE) by the normalized DIVISOR_LIMB.
   DIVISOR_LIMB_INVERTED should be 2^(2*BITS_PER_MP_LIMB) / DIVISOR_LIMB +
   - 2^BITS_PER_MP_LIMB.
   Return the single-limb remainder.

Copyright 1991, 1993, 1994, 2000, 2001, 2002 Free Software Foundation, Inc.

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


/* This function used to be documented, but is now considered obsolete.  It
   continues to exist for binary compatibility, even when not required
   internally.  */

mp_limb_t
mpn_preinv_mod_1 (mp_srcptr dividend_ptr, mp_size_t dividend_size,
		  mp_limb_t divisor_limb, mp_limb_t divisor_limb_inverted)
{
  mp_size_t i;
  mp_limb_t n0, r;
  mp_limb_t dummy;

  ASSERT (dividend_size >= 1);
  ASSERT (divisor_limb & GMP_LIMB_HIGHBIT);

  r = dividend_ptr[dividend_size-1];
  if (r >= divisor_limb)
    r -= divisor_limb;

  for (i = dividend_size - 2; i >= 0; i--)
    {
      n0 = dividend_ptr[i];
      udiv_qrnnd_preinv (dummy, r, r, n0, divisor_limb, divisor_limb_inverted);
    }
  return r;
}
