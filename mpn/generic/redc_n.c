/* mpn_redc_n.  Set cp[] <- up[]/R^n mod mp[].  Clobber up[].
   mp[] is n limbs; up[] is 2n limbs, the inverse ip[] is n limbs.

   THIS IS AN INTERNAL FUNCTION WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH THIS FUNCTION THROUGH DOCUMENTED INTERFACES.

Copyright (C) 2000, 2001, 2002, 2004, 2008 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include "gmp.h"
#include "gmp-impl.h"

void
mpn_redc_n (mp_ptr rp, mp_ptr tp, mp_srcptr mp, mp_size_t n, mp_srcptr ip)
{
  mp_ptr xp, yp;
  mp_limb_t cy;
  TMP_DECL;
  TMP_MARK;

  xp = TMP_ALLOC_LIMBS (3 * n);
  yp = xp + n;

  mpn_mullow_n (xp, tp, ip, n);
  mpn_mul_n (yp, xp, mp, n);
  cy = mpn_sub_n (rp, tp + n, yp + n, n);
  if (cy != 0)
    mpn_add_n (rp, rp, mp, n);

  TMP_FREE;
}
