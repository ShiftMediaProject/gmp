/* gcd_subdiv_step.c.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2003, 2004, 2005, 2008, 2010 Free Software Foundation, Inc.

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
   b is small, or the difference is small. Perform one subtraction
   followed by one division. Returns zero if the gcd is found.
   Otherwise, compute the reduced a and b, and return the new size. */

/* About the hook functions.

     hook->update (ctx, qp, qn, d)

   is called when one of the numbers, or a multiple of it, is
   subtracted from the other. d == 0 means q a is subtracted from b, d
   == 1 means that q b is subtracted from a.

     hook->done (ctx, gp, gn, d)

   is called when the gcd is found. d == 0 if the last reduction
   subtracted a from b, d == 1 if it subtracted b from a, and d == 2
   if it is unknown which was the most recent reduction. */

mp_size_t
mpn_gcd_subdiv_step (mp_ptr ap, mp_ptr bp, mp_size_t n,
		     const struct gcd_subdiv_step_hook *hook, void *ctx,
		     mp_ptr tp)
{
  mp_size_t an, bn;

  int swapped;
  
  ASSERT (n > 0);
  ASSERT (ap[n-1] > 0 || bp[n-1] > 0);

  an = bn = n;
  MPN_NORMALIZE (ap, an);
  MPN_NORMALIZE (bp, bn);

  swapped = 0;

  /* Arrange so that a < b, subtract b -= a, and maintain
     normalization. */
  if (an == bn)
    {
      int c;
      MPN_CMP (c, ap, bp, an);
      if (UNLIKELY (c == 0))
	{
	  /* For gcdext, return the smallest of the two cofactors. */
	  hook->done (ctx, ap, an, 2);
	  return 0;
	}
      else if (c > 0)
	{
	  MP_PTR_SWAP (ap, bp);
	  swapped ^= 1;
	}
    }
  else
    {
      if (an > bn)
	{
	  MPN_PTR_SWAP (ap, an, bp, bn);
	  swapped ^= 1;
	}
      if (an == 0)
	{
	  hook->done (ctx, bp, bn, swapped ^ 1);
	  return 0;
	}
    }
  ASSERT_NOCARRY (mpn_sub (bp, bp, bn, ap, an));
  MPN_NORMALIZE (bp, bn);
  ASSERT (bn > 0);

  if (hook->update)
    {
      static const mp_limb_t one = CNST_LIMB(1);
      hook->update(ctx, &one, 1, swapped);
    }

  /* Arrange so that a < b, and divide b = q a + r */
  if (an == bn)
    {
      int c;
      MPN_CMP (c, ap, bp, an);
      if (UNLIKELY (c == 0))
	{
	  hook->done (ctx, bp, bn, swapped);
	  return 0;	  
	}
      else if (c > 0)
	{
	  MP_PTR_SWAP (ap, bp);
	  swapped ^= 1;
	}
    }
  else if (an > bn)
    {
      MPN_PTR_SWAP (ap, an, bp, bn);
      swapped ^= 1;
    }

  mpn_tdiv_qr (tp, bp, 0, bp, bn, ap, an);

  /* FIXME: For jacobi, we need to call update before calling done.
     While for gcdext, calling update in this case would do useless
     work. */
  if (mpn_zero_p (bp, an))
    {
      hook->done (ctx, ap, an, swapped);
      return 0;
    }

  if (hook->update)
    hook->update(ctx, tp, bn - an + 1, swapped);
  
  return an;
}
