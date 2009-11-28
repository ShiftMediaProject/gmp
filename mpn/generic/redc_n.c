/* mpn_redc_n.  Set cp[] <- up[]/R^n mod mp[].  Clobber up[].
   mp[] is n limbs; up[] is 2n limbs, the inverse ip[] is n limbs.

   THIS IS AN INTERNAL FUNCTION WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH THIS FUNCTION THROUGH DOCUMENTED INTERFACES.

Copyright (C) 2009 Free Software Foundation, Inc.

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

/*
  TODO

  * We assume mpn_mulmod_bnm1 is always faster than plain mpn_mul_n (or a
    future mpn_mulhi) for the range we will be called.  Follow up that
    assumption.

  * Call mpn_mul_fft directly for large-enough operands.  It computes mod B^n+1
    (while mpn_mulmod_bnm1 of course computed mod B^n-1), so the "unwrap" code
    will be very similar to the current unwrap code.

  * Decrease scratch usage.

  * Call mpn_mulmod_bnm1_itch.
*/

void
mpn_redc_n (mp_ptr rp, mp_ptr tp, mp_srcptr mp, mp_size_t n, mp_srcptr ip)
{
  mp_ptr xp, yp, scratch_out;
  mp_limb_t cy;
  mp_size_t rn;
  TMP_DECL;
  TMP_MARK;

  xp = TMP_ALLOC_LIMBS (3 * n);
  yp = xp + n;

  mpn_mullow_n (xp, tp, ip, n);

  rn = mpn_mulmod_bnm1_next_size (n);
  scratch_out = TMP_ALLOC_LIMBS (2 * rn + 100);		/* FIXME */

  mpn_mulmod_bnm1 (yp, rn, xp, n, mp, n, scratch_out);

  ASSERT_ALWAYS (2 * n > rn);				/* could handle this */

  cy = mpn_sub_n (yp + rn, yp, tp, 2*n - rn);		/* undo wrap around */
  MPN_DECR_U (yp + 2*n - rn, rn, cy);

  cy = mpn_sub_n (rp, tp + n, yp + n, n);
  if (cy != 0)
    mpn_add_n (rp, rp, mp, n);

  TMP_FREE;
}
