/* mpn_div_qr_2 -- Divide natural numbers, producing both remainder and
   quotient.  The divisor is two limbs.

   Contributed to the GNU project by Torbjorn Granlund and Niels Möller

   THIS FILE CONTAINS INTERNAL FUNCTIONS WITH MUTABLE INTERFACES.  IT IS
   ONLY SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS
   ALMOST GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP
   RELEASE.


Copyright 1993, 1994, 1995, 1996, 1999, 2000, 2001, 2002, 2011 Free Software
Foundation, Inc.

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
#include "longlong.h"


static mp_limb_t
mpn_div_qr_2_norm (mp_ptr qp, mp_ptr np, mp_size_t nn,
		   mp_limb_t d1, mp_limb_t d0, mp_limb_t di)
{
  mp_limb_t qh;
  mp_size_t i;
  mp_limb_t r1, r0;

  ASSERT (nn >= 2);
  ASSERT (d1 & GMP_NUMB_HIGHBIT);

  np += nn - 2;
  r1 = np[1];
  r0 = np[0];

  qh = 0;
  if (r1 >= d1 && (r1 > d1 || r0 >= d0))
    {
#if GMP_NAIL_BITS == 0
      sub_ddmmss (r1, r0, r1, r0, d1, d0);
#else
      r0 = r0 - d0;
      r1 = r1 - d1 - (r0 >> GMP_LIMB_BITS - 1);
      r0 &= GMP_NUMB_MASK;
#endif
      qh = 1;
    }

  for (i = nn - 2 - 1; i >= 0; i--)
    {
      mp_limb_t n0, q;
      n0 = np[-1];
      udiv_qr_3by2 (q, r1, r0, r1, r0, n0, d1, d0, di);
      np--;
      qp[i] = q;
    }

  np[1] = r1;
  np[0] = r0;

  return qh;
}

static mp_limb_t
mpn_div_qr_2_unnorm (mp_ptr qp, mp_ptr np, mp_size_t nn,
		     mp_limb_t d1, mp_limb_t d0, int shift, mp_limb_t di)
{
  mp_limb_t qh;
  mp_limb_t r2, r1, r0;
  mp_size_t i;

  ASSERT (nn >= 2);
  ASSERT (d1 & GMP_NUMB_HIGHBIT);
  ASSERT (shift > 0);

  r2 = np[nn-1] >> (GMP_LIMB_BITS - shift);
  r1 = (np[nn-1] << shift) | (np[nn-2] >> (GMP_LIMB_BITS - shift));
  r0 = np[nn-2] << shift;

  udiv_qr_3by2 (qh, r2, r1, r2, r1, r0, d1, d0, di);

  for (i = nn - 2 - 1; i >= 0; i--)
    {
      mp_limb_t q;
      r0 = np[i];
      r1 |= r0 >> (GMP_LIMB_BITS - shift);
      r0 <<= shift;
      udiv_qr_3by2 (q, r2, r1, r2, r1, r0, d1, d0, di);
      qp[i] = q;      
    }

  np[0] = (r1 >> shift) | (r2 << (GMP_LIMB_BITS - shift));
  np[1] = r2 >> shift;
  return qh;
}

/* Divide num {np,nn} by den {dp,2} and write the nn-2 least
   significant quotient limbs at qp and the 2 long remainder at np.
   Return the most significant limb of the quotient.

   Preconditions:
   1. qp must either not overlap with the input operands at all, or
      qp >= np + 2 must hold true.  (This means that it's possible to put
      the quotient in the high part of {np,nn}, right above the remainder.
   2. nn >= 2.  */

mp_limb_t
mpn_div_qr_2 (mp_ptr qp, mp_ptr np, mp_size_t nn,
	      mp_srcptr dp)
{
  mp_limb_t d1;
  mp_limb_t d0;
  gmp_pi1_t dinv;

  ASSERT (nn >= 2);
  ASSERT (! MPN_OVERLAP_P (qp, nn-2, np, nn) || qp >= np + 2);
  ASSERT_MPN (np, nn);
  ASSERT_MPN (dp, 2);

  d1 = dp[1]; d0 = dp[0];

  ASSERT (d1 > 0);

  if (UNLIKELY (d1 & GMP_NUMB_HIGHBIT))
    {
      gmp_pi1_t dinv;
      invert_pi1 (dinv, d1, d0);
      return mpn_div_qr_2_norm (qp, np, nn, d1, d0, dinv.inv32);
    }
  else
    {
      int shift;
      count_leading_zeros (shift, d1);
      d1 = (d1 << shift) | (d0 >> (GMP_LIMB_BITS - shift));
      d0 <<= shift;
      invert_pi1 (dinv, d1, d0);
      return mpn_div_qr_2_unnorm (qp, np, nn, d1, d0, shift, dinv.inv32);
    }
}
