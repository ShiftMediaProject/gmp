/* mpfr_set_f -- set a MPFR number from a GNU MPF number

Copyright (C) 1999-2000 Free Software Foundation.

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
mpfr_set_f(mpfr_ptr y, mpf_srcptr x, mp_rnd_t rnd_mode)
#else
mpfr_set_f(y, x, rnd_mode)
     mpfr_ptr y;
     mpf_srcptr x;
     mp_rnd_t rnd_mode;
#endif
{
  mp_limb_t *my, *mx, *tmp; unsigned long cnt, sx, sy;
  TMP_DECL(marker);

  if (SIZ(x) * MPFR_SIGN(y) < 0) MPFR_CHANGE_SIGN(y);

  MPFR_CLEAR_FLAGS(y);

  TMP_MARK(marker);
  sx = ABS(SIZ(x)); sy = MPFR_ABSSIZE(y);
  my = MPFR_MANT(y); mx = PTR(x);

  if (sx==0) { /* x is zero */
    MPFR_SET_ZERO(y); return;
  }

  count_leading_zeros(cnt, mx[sx - 1]);  

  if (sy < sx)
    {
      unsigned long xprec = sx * BITS_PER_MP_LIMB;

      tmp = (mp_limb_t*) TMP_ALLOC(xprec);
      if (cnt) mpn_lshift(tmp, mx, sx, cnt); 
      else MPN_COPY(tmp, mx, sx); 
      mpfr_round_raw(my, tmp, xprec, (SIZ(x)<0), MPFR_PREC(y), rnd_mode);  
    }
  else
    {
      if (cnt) mpn_lshift(my + sy - sx, mx, sx, cnt); 
      else MPN_COPY(my + sy - sx, mx, sy); 
      MPN_ZERO(my, sy - sx);
      /* no rounding necessary, since y has a larger mantissa */
    }
  
  MPFR_EXP(y) = EXP(x) * BITS_PER_MP_LIMB - cnt;

  TMP_FREE(marker);
}
