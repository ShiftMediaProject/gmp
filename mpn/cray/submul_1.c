/* mpn_addmul_1 for Cray PVP.  This code would run on any machine with 64-bit
   limbs, but it is optimized for Cray's fast 24/48 bit multiply, and wouldn't
   run well on other machines.

Copyright (C) 1996 Free Software Foundation, Inc.

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
MA 02111-1307, USA.  */


#include "gmp.h"
#include "gmp-impl.h"

mp_limb_t
mpn_addmul_1 (mp_ptr rp, mp_srcptr up, mp_size_t n, mp_limb_t limb)
{
  mp_ptr p0, p1, tp;
  mp_limb_t cy_limb;
  TMP_DECL (marker);
  TMP_MARK (marker);

  p1 = TMP_ALLOC (n * BYTES_PER_MP_LIMB);
  p0 = TMP_ALLOC (n * BYTES_PER_MP_LIMB);
  tp = TMP_ALLOC (n * BYTES_PER_MP_LIMB);

  GMPN_MULWW (p1, p0, up, &n, &limb);
  cy_limb = mpn_sub_n (tp, rp, p0, n);
  rp[0] = tp[0];
  cy_limb += mpn_sub_n (rp + 1, tp + 1, p1, n - 1);
  cy_limb += p1[n - 1];

  TMP_FREE (marker);
  return cy_limb;
}
