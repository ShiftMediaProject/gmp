/* mpn_modexact_1c_odd -- Nx1 modexact style remainder.

   THE FUNCTIONS IN THIS FILE ARE FOR INTERNAL USE ONLY.  THEY'RE ALMOST
   CERTAIN TO BE SUBJECT TO INCOMPATIBLE CHANGES OR DISAPPEAR COMPLETELY IN
   FUTURE GNU MP RELEASES.  */
 
/*
Copyright 2000, 2001 Free Software Foundation, Inc.

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
MA 02111-1307, USA.
*/

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/* Calculate r, satisfying 0<=r<divisor and

           r*b^k + a - c == q*d

   where b=2^BITS_PER_MP_LIMB, a is {src,size}, k is either size or size-1
   (the caller won't know which), and q is a quotient (which is discarded).
   d must be odd, c can be any limb value.

   This slightly strange function suits divisibility and congruence testing
   since if r=0 then a==c mod d, or GCDs since the factors of 2 in b^k can
   be discarded leaving -r == (a-c) mod d, and similarly the Jacobi symbol
   (with the factor of -1 on r accounted for).


   r is like the remainder returned by mpn_divexact_by3c, and is the sort of
   remainder a hypothetical mpn_divexact_1 might return.  Like
   mpn_divexact_by3c, r represents a borrow, since effectively quotient
   limbs are chosen so that subtracting that multiple of divisor from src at
   each step will produce a zero limb.

   A long calculation can be done piece by piece from low to high by passing
   the return value from one part as the carry parameter to the next part.
   The effective final k becomes anything between size and size-n, if n
   pieces are used.


   A similar sort of routine could be constructed based on adding multiples
   of d at each limb, much like redc in mpz_powm does.  Subtracting however
   has a small advantage that there's never a borrow into h, whereas using
   an addition to cancel out s would put a carry into h according to
   src[i]-c ==0 or !=0.


   In terms of efficiency, this function is similar to a mul-by-inverse
   mpn_mod_1.  Both consist essentially of two multiplies and are best
   suited to CPUs with low latency multipliers (in comparison to a divide
   instruction.)  Modexact has a few less supplementary operations, only
   needs low part and high part multiplies, and has fewer working quantities
   (helping CPUs with few registers).


   The special case for size==1 is so that the high<divisor test at the end
   can assume c<d, that being established only if the main loop runs at
   least once.  There's also a decent chance one % is faster than a
   modlimb_invert, though that will depend on the processor.

   A CPU specific implementation might want to omit either the size==1 code
   or the high<divisor test.  mpn/x86/k6/mode1o.asm for instance finds
   neither useful.  */


mp_limb_t
mpn_modexact_1c_odd (mp_srcptr src, mp_size_t size, mp_limb_t d, mp_limb_t c)
{
  mp_limb_t  s, h, l, inverse, dummy;
  mp_size_t  i;

  ASSERT (size >= 1);
  ASSERT (d & 1);

  if (size == 1)
    {
      s = src[0];
      if (s > c)
        {
          l = s-c;
          h = l % d;
          if (h != 0)
            h = d - h;
        }
      else
        {
          l = c-s;
          h = l % d;
        }
      return h;
    }

  modlimb_invert (inverse, d);

  i = 0;
  do
    {
      s = src[i];
      l = s - c;
      c = (l > s);
      l *= inverse;
      umul_ppmm (h, dummy, l, d);
      c += h;
    }
  while (++i < size-1);

  s = src[i];
  if (s <= d)
    {
      /* With high<d the final step can be a subtract and addback. */
      l = c - s;
      if (l > c)
        l += d;

      ASSERT (l < d);
      return l;
    }
  else
    {
      /* Can't skip a divide, just do the loop code once more. */
      l = s - c;
      c = (l > s);
      l *= inverse;
      umul_ppmm (h, dummy, l, d);
      c += h;

      ASSERT (c < d);
      return c;
    }
}



#if 0

/* The following is an alternate form that might shave one cycle on a
   superscalar processor since it takes c+=h off the dependent chain,
   leaving just a low product, high product, and a subtract.

   This is for CPU specific implementations to consider.  A special case for
   high<divisor and/or size==1 can be added if desired.

   Notice that c is only ever 0 or 1, since if s-c produces a borrow then
   x=0xFF..FF and x-h cannot produce a borrow.  The c=(x>s) could become
   c=(x==0xFF..FF) too, if that helped.  */

mp_limb_t
mpn_modexact_1c_odd (mp_srcptr src, mp_size_t size, mp_limb_t d, mp_limb_t h)
{
  mp_limb_t  s, x, y, inverse, dummy;
  mp_limb_t  c = 0;
  mp_size_t  i;

  ASSERT (size >= 1);
  ASSERT (d & 1);

  modlimb_invert (inverse, d);

  for (i = 0; i < size; i++)
    {
      ASSERT (c==0 || c==1);

      s = src[i];
      x = s - c;
      c = (x > s);

      y = x - h;
      c += (y > x);

      y *= inverse;
      umul_ppmm (h, dummy, y, d);
    }

  h += c;
  ASSERT (h < d);
  return h;
}

#endif
