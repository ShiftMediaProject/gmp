/* mpn_scan0 -- Scan from a given bit position for the next clear bit.

Copyright 1994, 1996, 2001, 2002 Free Software Foundation, Inc.

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

/* Design issues:
   1. What if starting_bit is not within U?  Caller's problem?
   2. Bit index should be 'unsigned'?

   Argument constraints:
   1. U must sooner or later have a limb with a clear bit.
 */

unsigned long int
mpn_scan0 (register mp_srcptr up,
	   register unsigned long int starting_bit)
{
  mp_size_t starting_word;
  mp_limb_t alimb;
  int cnt;
  mp_srcptr p;

  /* Start at the word implied by STARTING_BIT.  */
  starting_word = starting_bit / GMP_NUMB_BITS;
  p = up + starting_word;
  alimb = *p++ ^ GMP_NUMB_MASK;

  /* Mask off any bits before STARTING_BIT in the first limb.  */
  alimb &= - (mp_limb_t) 1 << (starting_bit % GMP_NUMB_BITS);

  while (alimb == 0)
    alimb = *p++ ^ GMP_NUMB_MASK;

  count_trailing_zeros (cnt, alimb);
  return (p - up - 1) * GMP_NUMB_BITS + cnt;
}
