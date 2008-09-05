/* gcd_subdiv_step.c.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2003, 2004, 2005, 2008 Free Software Foundation, Inc.

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

/* Used when mpn_hgcd or mpn_hgcd2 has failed. Then either one of a or
   b is very small, or the difference is very small. Perform one
   subtraction followed by one division. If the gcd is found, stores
   it in gp and *gn, and returns zero. Otherwise, compute the reduced
   a and b, and return the new size. */

/* FIXME: Check when the smaller number is a single limb, and invoke
 * mpn_gcd_1. */
mp_size_t
mpn_gcd_subdiv_step (mp_ptr gp, mp_size_t *gn,
		      mp_ptr ap, mp_ptr bp, mp_size_t n, mp_ptr tp)
{
  mp_size_t an, bn;

  ASSERT (n > 0);
  ASSERT (ap[n-1] > 0 || bp[n-1] > 0);

  /* First, make sure that an >= bn, and subtract an -= bn */
  for (an = n; an > 0; an--)
    if (ap[an-1] != bp[an-1])
      break;

  if (an == 0)
    {
      /* Done */
      MPN_COPY (gp, ap, n);
      *gn = n;
      return 0;
    }

  if (ap[an-1] < bp[an-1])
    MP_PTR_SWAP (ap, bp);

  bn = n;
  MPN_NORMALIZE (bp, bn);
  if (bn == 0)
    {
      MPN_COPY (gp, ap, n);
      *gn = n;
      return 0;
    }

  ASSERT_NOCARRY (mpn_sub_n (ap, ap, bp, an));
  MPN_NORMALIZE (ap, an);

  ASSERT (an > 0);
	  
  if (an < bn)
    MPN_PTR_SWAP (ap, an, bp, bn);
  else if (an == bn)
    {
      int c;
      MPN_CMP (c, ap, bp, an);
      if (c < 0)
	MP_PTR_SWAP (ap, bp);
    }

  mpn_tdiv_qr (tp + bn, tp, 0, ap, an, bp, bn);

  /* Normalizing seems to be the simplest way to test if the remainder
     is zero. */
  an = bn;
  MPN_NORMALIZE (tp, an);
  if (an == 0)
    {
      MPN_COPY (gp, bp, bn);
      *gn = bn;
      return 0;
    }

  MPN_COPY (ap, tp, bn);

  return bn;
}
