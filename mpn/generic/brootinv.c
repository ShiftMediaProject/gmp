/* mpn_brootinv, compute r such that r^k * y = 1 (mod 2^b).

   Contributed to the GNU project by Martin Boij (as part of perfpow.c).

Copyright 2009, 2010, 2012 Free Software Foundation, Inc.

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

/* Compute r such that r^k * y = 1 (mod 2^b).

   Iterates
     r' <-- k^{-1} ((k+1) r - r^{k+1} y) (mod 2^b)
   using Hensel lifting, each time doubling the number of known bits in r.

   Works just for odd k.  Else the Hensel lifting degenerates.

   FIXME:
     (1) Simplify to do precision book-keeping in limbs rather than bits.

     (2) Rewrite iteration as
	   r' <-- r - k^{-1} r (r^k y - 1)
	 and take advantage of the zero low part of r^k y - 1.

     (3) Use wrap-around trick.

     (4) Use a small table to get starting value.

   If we prefer to get y^{1/k} rather than y^{-1/k}, we could also switch to
   the iteration

     r' <-- r - k^{-1} r (r^k y^{k-1} - 1)

   which converges to y^{1/n - 1}, and we then get y^{1/n} by a single mullo.
*/
void
mpn_brootinv (mp_ptr rp, mp_srcptr yp, mp_bitcnt_t bnb, mp_limb_t k, mp_ptr tp)
{
  mp_ptr tp2, tp3;
  mp_limb_t kinv, k2;
  mp_size_t bn, order[GMP_LIMB_BITS + 1];
  int i, d;

  ASSERT (bnb > 0);
  ASSERT ((k & 1) != 0);

  bn = 1 + (bnb - 1) / GMP_LIMB_BITS;

  tp2 = tp + bn;
  tp3 = tp + 2 * bn;
  k2 = k + 1;

  binvert_limb (kinv, k);

  d = 0;
  for (; bnb != 1; bnb = (bnb + 1) >> 1)
    order[d++] = 1 + (bnb - 1) / GMP_LIMB_BITS;

  rp[0] = 1;
  for (i = d - 1; i >= 0; i--)
    {
      bn = order[i];

      mpn_mul_1 (tp, rp, bn, k2);

      mpn_powlo (tp2, rp, &k2, 1, bn, tp3);
      mpn_mullo_n (rp, yp, tp2, bn);

      mpn_sub_n (tp2, tp, rp, bn);
      mpn_pi1_bdiv_q_1 (rp, tp2, bn, k, kinv, 0);
    }
}
