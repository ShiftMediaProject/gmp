/* mpn_mod_34lsub1 -- remainder modulo 2^(BITS_PER_MP_LIMB*3/4)-1.

   THE FUNCTIONS IN THIS FILE ARE FOR INTERNAL USE ONLY.  THEY'RE ALMOST
   CERTAIN TO BE SUBJECT TO INCOMPATIBLE CHANGES OR DISAPPEAR COMPLETELY IN
   FUTURE GNU MP RELEASES.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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


/* Calculate a remainder from {p,n} divided by 2^(BITS_PER_MP_LIMB*3/4)-1.
   The remainder is not fully reduced, it's any limb value congruent to
   {p,n} modulo that divisor.

   This implementation is only correct when BITS_PER_MP_LIMB is a multiple
   of 4, but that suffices for all current uses.  */

#if BITS_PER_MP_LIMB % 4 == 0

#define B1  (BITS_PER_MP_LIMB / 4)
#define B2  (B1 * 2)
#define B3  (B1 * 3)

#define M1  ((CNST_LIMB(1) << B1) - 1)
#define M2  ((CNST_LIMB(1) << B2) - 1)
#define M3  ((CNST_LIMB(1) << B3) - 1)

#define LOW0(n)      ((n) & M3)
#define HIGH0(n)     ((n) >> B3)

#define LOW1(n)      (((n) & M2) << B1)
#define HIGH1(n)     ((n) >> B2)

#define LOW2(n)      (((n) & M1) << B2)
#define HIGH2(n)     ((n) >> B1)

#define PARTS0(n)    (LOW0(n) + HIGH0(n))
#define PARTS1(n)    (LOW1(n) + HIGH1(n))
#define PARTS2(n)    (LOW2(n) + HIGH2(n))

#define ADD(c,a,val)    \
  do {                  \
    mp_limb_t  l = val; \
    a += l;             \
    c += (a < l);       \
  } while (0)

mp_limb_t
mpn_mod_34lsub1 (mp_srcptr p, mp_size_t n)
{
  mp_limb_t  c0 = 0;
  mp_limb_t  c1 = 0;
  mp_limb_t  c2 = 0;
  mp_limb_t  a0, a1, a2;

  ASSERT (n >= 1);

  a0 = a1 = a2 = 0;
  c0 = c1 = c2 = 0;

  while ((n -= 3) >= 0)
    {
      ADD (c0, a0, p[0]);
      ADD (c1, a1, p[1]);
      ADD (c2, a2, p[2]);
      p += 3;
    }

  if (n != -3)
    {
      ADD (c0, a0, p[0]);
      if (n != -2)
	ADD (c1, a1, p[1]);
    }

  return
    PARTS0 (a0) + PARTS1 (a1) + PARTS2 (a2)
    + PARTS1 (c0) + PARTS2 (c1) + PARTS0 (c2);
}

#endif
