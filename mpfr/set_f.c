/* mpfr_set_f -- set a MPFR number from a GNU MPF number

Copyright (C) 1999-2000 PolKA project, Inria Lorraine and Loria

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
mpfr_set_f(mpfr_ptr y, mpf_srcptr x, char rnd_mode)
#else
mpfr_set_f(y, x, rnd_mode)
     mpfr_ptr y;
     mpf_srcptr x;
     char rnd_mode; 
#endif
{
  mp_limb_t *my, *mx, *tmp; unsigned long cnt, sx, sy;
  TMP_DECL(marker);

  sx = ABS(SIZ(x)); sy = ABSSIZE(y);
  my = MANT(y); mx = MANT(x);

  if (sx==0) { /* x is zero */
    SET_ZERO(y); return;
  }

  count_leading_zeros(cnt, mx[sx - 1]);  

  if (SIZ(x)*SIGN(y)<0) CHANGE_SIGN(y);

  if (sy < sx)
    {
      unsigned long xprec = sx * BITS_PER_MP_LIMB;

      tmp = (mp_limb_t*) TMP_ALLOC(xprec);
      if (cnt) mpn_lshift(tmp, mx, sx, cnt); 
      else MPN_COPY(tmp, mx, sx); 
      mpfr_round_raw(my, tmp, xprec, (SIZ(x)<0), PREC(y), rnd_mode);  
    }
  else
    {
      if (cnt) mpn_lshift(my + sy - sx, mx, sx, cnt); 
      else MPN_COPY(my + sy - sx, mx, sy); 
      MPN_ZERO(my, sy - sx);
      /* no rounding necessary, since y has a larger mantissa */
    }
  
  EXP(y) = EXP(x) * BITS_PER_MP_LIMB - cnt;

  TMP_FREE(marker);
}
