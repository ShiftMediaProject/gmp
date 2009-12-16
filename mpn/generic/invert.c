/* invert.c -- Compute floor((B^{2n}-1)/U).

   Contributed to the GNU project by Marco Bodrato.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY WILL CHANGE OR DISAPPEAR IN A FUTURE GMP RELEASE.

Copyright (C) 2007, 2009 Free Software Foundation, Inc.

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

#include <stdlib.h>		/* for NULL */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

#ifndef INV_APPR_THRESHOLD
#define INV_APPR_THRESHOLD (INV_NEWTON_THRESHOLD)
#endif


void
mpn_invert (mp_ptr ip, mp_srcptr dp, mp_size_t n, mp_ptr scratch)
{
  ASSERT (n > 0);
  ASSERT (dp[n-1] & GMP_LIMB_HIGHBIT);
  ASSERT (! MPN_OVERLAP_P (ip, n, dp, n));
  ASSERT (! MPN_OVERLAP_P (ip, n, scratch, mpn_invertappr_itch(n)));
  ASSERT (! MPN_OVERLAP_P (dp, n, scratch, mpn_invertappr_itch(n)));

  if (n == 1)
    invert_limb (*ip, *dp);
  else {
    TMP_DECL;

    TMP_MARK;
    if (scratch == NULL)
      scratch = TMP_ALLOC_LIMBS (mpn_invert_itch (n));

    if (BELOW_THRESHOLD (n, INV_APPR_THRESHOLD)) {
      /* Maximum scratch needed by this branch: 3*n + 2 */
      mp_size_t i;
      mp_ptr xp;

      xp = scratch + n + 2;				/* 2 * n limbs */
      for (i = n - 1; i >= 0; i--)
	xp[i] = ~CNST_LIMB(0);
      mpn_com_n (xp + n, dp, n);
      mpn_tdiv_qr (scratch, ip, 0, xp, 2 * n, dp, n);
      MPN_COPY (ip, scratch, n);
    } else { /* Use approximated inverse; correct the result if needed. */
      mp_limb_t cy;

      cy = mpn_ni_invertappr (ip, dp, n, scratch);

      if (cy) {
	/* Code to detect and correct the "off by one" approximation. */
	mpn_mul_n (scratch, ip, dp, n);
	ASSERT_NOCARRY (mpn_add_n (scratch + n, scratch + n, dp, n));
	if ( ! mpn_add (scratch, scratch, 2*n, dp, n)) {
	  MPN_INCR_U (ip, n, 1);
	}
      }
    }
    TMP_FREE;
  }
}
