/* mpfr_mul_ui -- multiply a floating-point number by a machine integer

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"

void
#if __STDC__
mpfr_mul_ui(mpfr_ptr y, mpfr_srcptr x, unsigned long u, unsigned char RND_MODE)
#else
mpfr_mul_ui(y, x, u, RND_MODE)
     mpfr_ptr y; 
     mpfr_srcptr x; 
     unsigned long u; 
     unsigned char RND_MODE; 
#endif
{
  mp_limb_t carry, *my, *old_my; unsigned long c; 
  unsigned long xsize, ysize, cnt, dif; 
  TMP_DECL(marker);

  TMP_MARK(marker);
  my = MANT(y); 
  ysize = (PREC(y)-1)/BITS_PER_MP_LIMB + 1;
  xsize = (PREC(x)-1)/BITS_PER_MP_LIMB + 1;

  if (ysize < xsize) {
      old_my = my; 
      my = (mp_ptr) TMP_ALLOC (xsize * BYTES_PER_MP_LIMB);
      dif=0;
    }
  else dif=ysize-xsize;

  carry = mpn_mul_1(my+dif, MANT(x), xsize, u);
  MPN_ZERO(my, dif);

  /* WARNING: count_leading_zeros is undefined for carry=0 */
  if (carry) count_leading_zeros(cnt, carry);
  else cnt=BITS_PER_MP_LIMB;
      
  c = mpfr_round_raw(my, my, PREC(x), (SIGN(x)<0), 
		     PREC(y)-BITS_PER_MP_LIMB+cnt, RND_MODE);
  
  /* If cnt = 1111111111111 and c = 1 we shall get depressed */
  if (c && (carry == (((mp_limb_t)1) << (BITS_PER_MP_LIMB - cnt)) - 1))
    {
      cnt--; 
      mpn_rshift(my, my, ysize, BITS_PER_MP_LIMB - cnt); 
      my[ysize - 1] |= ((mp_limb_t) 1) << (BITS_PER_MP_LIMB - 1);
    }
  else
    {
      /* Warning: mpn_rshift is undefined for shift=0 */
      if (cnt!=BITS_PER_MP_LIMB)
	mpn_rshift(my, my, ysize, BITS_PER_MP_LIMB - cnt); 
      my[ysize - 1] |= (carry << cnt); 
    }
  EXP(y) = EXP(x) + BITS_PER_MP_LIMB - cnt; 
  if (ysize < xsize) MPN_COPY(old_my, my, ysize);
  /* set sign */
  if (SIGN(y) != SIGN(x)) CHANGE_SIGN(y);
  TMP_FREE(marker);
}
