/* mpfr_mul_ui -- multiply a floating-point number by a machine integer

Copyright (C) 1999 Free Software Foundation.

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
#include "mpfr-impl.h"

void
#if __STDC__
mpfr_mul_ui(mpfr_ptr y, mpfr_srcptr x, unsigned long int u, mp_rnd_t rnd_mode)
#else
mpfr_mul_ui(y, x, u, rnd_mode)
     mpfr_ptr y;
     mpfr_srcptr x;
     unsigned long int u;
     mp_rnd_t rnd_mode;
#endif
{
  mp_limb_t carry, *my, *old_my, *my2; unsigned long c; 
  unsigned long xsize, ysize, cnt, dif, ex, sh; 
  TMP_DECL(marker);

  if (MPFR_IS_NAN(x))
    {
      MPFR_CLEAR_FLAGS(y);
      MPFR_SET_NAN(y);
      return;
    }

  if (MPFR_IS_INF(x)) 
    {
      MPFR_CLEAR_FLAGS(y);
      if (u) 
	{ 
	  MPFR_SET_INF(y); 
	  if (MPFR_SIGN(y) != MPFR_SIGN(x)) { MPFR_CHANGE_SIGN(y); }
	  return; 
	}
      else { MPFR_SET_NAN(y); return; }
    }

  MPFR_CLEAR_FLAGS(y); 

  TMP_MARK(marker);
  my = MPFR_MANT(y); ex = MPFR_EXP(x);  
  ysize = (MPFR_PREC(y)-1)/BITS_PER_MP_LIMB + 1;
  xsize = (MPFR_PREC(x)-1)/BITS_PER_MP_LIMB + 1;

  old_my = my; 

  if (ysize < xsize) {
      my = (mp_ptr) TMP_ALLOC (xsize * BYTES_PER_MP_LIMB);
      dif=0;
    }
  else dif=ysize-xsize;

  carry = mpn_mul_1(my+dif, MPFR_MANT(x), xsize, u);
  MPN_ZERO(my, dif);

  /* WARNING: count_leading_zeros is undefined for carry=0 */
  if (carry) count_leading_zeros(cnt, carry);
  else cnt=BITS_PER_MP_LIMB;

  /* Warning: the number of limbs used by x and the lower part
     of y may differ */
  sh = (MPFR_PREC(x)+BITS_PER_MP_LIMB-1)/BITS_PER_MP_LIMB
     - (MPFR_PREC(y)+cnt-1)/BITS_PER_MP_LIMB; 

  /* Warning: if all significant bits are in the carry, one has to 
     be careful */
  if (cnt + MPFR_PREC(y) < BITS_PER_MP_LIMB)
    {
      /* Quick 'n dirty */

      if (xsize > ysize) {
	my2 = (mp_ptr) TMP_ALLOC ((xsize + 1) * BYTES_PER_MP_LIMB);
	my2[xsize] = mpn_lshift(my2, my, xsize, cnt) 
	  | (carry << (cnt ? BITS_PER_MP_LIMB - cnt : 0));
      }
      else { 
	my2 = (mp_ptr) TMP_ALLOC ((ysize + 1) * BYTES_PER_MP_LIMB);
	my2[ysize] = mpn_lshift(my2, my, ysize, cnt)
	  | (carry << (cnt ? BITS_PER_MP_LIMB - cnt : 0)); 
      }      

      my = my2; ex += BITS_PER_MP_LIMB - cnt;
      carry = 0; cnt = BITS_PER_MP_LIMB;
    }

    c = mpfr_round_raw(my+sh, my, MPFR_PREC(x), (MPFR_SIGN(x)<0), 
		       MPFR_PREC(y)-BITS_PER_MP_LIMB+cnt, rnd_mode);

  /* If cnt = 1111111111111 and c = 1 we shall get depressed */
  if (c && (carry == (((mp_limb_t)1) << (cnt ? BITS_PER_MP_LIMB - cnt : 0)) 
	    - 1))
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
  MPFR_EXP(y) = ex + BITS_PER_MP_LIMB - cnt; 
  if (ysize < xsize) MPN_COPY(old_my, my, ysize);
  /* set sign */
  if (MPFR_SIGN(y) * MPFR_SIGN(x) < 0) MPFR_CHANGE_SIGN(y);
  TMP_FREE(marker);
}
