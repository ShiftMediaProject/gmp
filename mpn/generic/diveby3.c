/* mpn_divexact_by3c -- mpn exact division by 3.

Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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


/* The algorithm here is basically the same as mpn_divexact_1, as described
   in the manual.  Namely at each step q = (src[i]-c)*inverse, and new c =
   borrow(src[i]-c) + high(divisor*q).  But because the divisor is just 3,
   high(divisor*q) can be determined with two comparisons instead of a
   multiply.

   The "c += ..."s add the high limb of 3*l to c.  That high limb will be 0,
   1 or 2.  Doing two separate "+="s seems to give better code on gcc (as of
   2.95.2 at least).

   It will be noted that the new c is formed by adding three values each 0
   or 1.  But the total is only 0, 1 or 2.  When the subtraction src[i]-c
   causes a borrow, that leaves a limb value of either 0xFF...FF or
   0xFF...FE.  The multiply by MODLIMB_INVERSE_3 gives 0x55...55 or
   0xAA...AA respectively, and in those cases high(3*q) is only 0 or 1
   respectively, hence a total of no more than 2.

   Alternatives:

   This implementation has each multiply on the dependent chain, due to
   "l=s-c".  See below for alternative code which avoids that.  */

mp_limb_t
mpn_divexact_by3c (mp_ptr dst, mp_srcptr src, mp_size_t size, mp_limb_t c)
{
  mp_size_t  i;

  ASSERT (size >= 1);
  ASSERT (c == 0 || c == 1 || c == 2);
  ASSERT (MPN_SAME_OR_SEPARATE_P (dst, src, size));

  i = 0;
  do
    {
      mp_limb_t  l, s;

      s = src[i];
      SUBC_LIMB (c, l, s, c);

      l = (l * MODLIMB_INVERSE_3) & GMP_NUMB_MASK;
      dst[i] = l;

      c += (l >= GMP_NUMB_CEIL_MAX_DIV3);
      c += (l >= GMP_NUMB_CEIL_2MAX_DIV3);
    }
  while (++i < size);

  ASSERT (c == 0 || c == 1 || c == 2);
  return c;
}


#if 0

/* The following alternative code re-arranges the quotient calculation from
   (src[i]-c)*inverse to instead

       q = src[i]*inverse - c*inverse

   thereby allowing src[i]*inverse to be scheduled back as far as desired,
   making full use of multiplier throughput and leaving just some carry
   handing on the dependent chain.

   The carry handling consists of determining the c for the next iteration.
   This is the same as described above, namely look for any borrow from
   src[i]-c, and at the high of 3*q.

   high(3*q) is done with two comparisons as above (in c2 and c3).  The
   borrow from src[i]-c is incorporated into those by noting that if there's
   a carry then then we have src[i]-c == 0xFF..FF or 0xFF..FE, in turn
   giving q = 0x55..55 or 0xAA..AA.  Adding 1 to either of those q values is
   enough to make high(3*q) come out 1 bigger, as required.

   l = -c*inverse is calculated at the same time as c, since for most chips
   it can be more conveniently derived from separate c1/c2/c3 values than
   from a combined c equal to 0, 1 or 2.

   The net effect is that with good pipelining this loop should be able to
   run at perhaps 4 cycles/limb, depending on available execute resources
   etc.

   Usage:

   This code is not used by default, since we really can't rely on the
   compiler generating a good software pipeline, nor on such an approach
   even being worthwhile on all CPUs.

   Itanium is one chip where this algorithm helps though, see
   mpn/ia64/diveby3.asm.  */

mp_limb_t
mpn_divexact_by3c (mp_ptr dst, mp_srcptr src, mp_size_t size, mp_limb_t c)
{
  mp_limb_t  s, sm, l, q, qx, c1, c2, c3;
  mp_size_t  i;

  ASSERT (size >= 1);
  ASSERT (c == 0 || c == 1 || c == 2);
  ASSERT (MPN_SAME_OR_SEPARATE_P (dst, src, size));

  l = (c == 0 ? 0 : c == 1 ? -MODLIMB_INVERSE_3 : -2*MODLIMB_INVERSE_3);

  for (i = 0; i < size; i++)
    {
      s = src[i];
      sm = s * MODLIMB_INVERSE_3;

      q = l + sm;
      c1 = (s < c);

      dst[i] = q;
      qx = q + c1;

      c2 = (qx > MP_LIMB_T_MAX/3);
      c3 = (qx > (MP_LIMB_T_MAX/3)*2);

      c = c2 + c3;
      l = (-c2 & -MODLIMB_INVERSE_3) + (-c3 & -MODLIMB_INVERSE_3);
    }

  return c;
}

#endif
