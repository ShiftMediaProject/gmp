/* mpfr_set_f -- set a MPFR number from a GNU MPF number

Copyright 1999, 2000, 2001 Free Software Foundation, Inc.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_set_f (mpfr_ptr y, mpf_srcptr x, mp_rnd_t rnd_mode)
{
  mp_limb_t *my, *mx, *tmp;
  unsigned long cnt, sx, sy;
  int inexact;
  TMP_DECL(marker);

  if (SIZ(x) * MPFR_SIGN(y) < 0)
    MPFR_CHANGE_SIGN (y);

  MPFR_CLEAR_FLAGS (y);

  sx = ABS(SIZ(x)); /* number of limbs of the mantissa of x */
  sy = 1 + (MPFR_PREC(y) - 1) / BITS_PER_MP_LIMB;
  my = MPFR_MANT(y);
  mx = PTR(x);

  if (sx == 0) /* x is zero */
    {
      MPFR_SET_ZERO(y);
      return 0; /* 0 is exact */
    }

  count_leading_zeros(cnt, mx[sx - 1]);  

  if (sy <= sx) /* we may have to round even when sy = sx */
    {
      unsigned long xprec = sx * BITS_PER_MP_LIMB;

      TMP_MARK(marker);
      tmp = (mp_limb_t*) TMP_ALLOC(xprec);
      if (cnt)
	mpn_lshift(tmp, mx, sx, cnt);
      else
	MPN_COPY(tmp, mx, sx);
      mpfr_round_raw (my, tmp, xprec, (SIZ(x)<0), MPFR_PREC(y), rnd_mode,
		      &inexact);
      TMP_FREE(marker);
    }
  else
    {
      if (cnt)
	mpn_lshift(my + sy - sx, mx, sx, cnt); 
      else
	MPN_COPY(my + sy - sx, mx, sy); 
      MPN_ZERO(my, sy - sx);
      /* no rounding necessary, since y has a larger mantissa */
      inexact = 0;
    }
  
  MPFR_EXP(y) = EXP(x) * BITS_PER_MP_LIMB - cnt;

  return inexact;
}
