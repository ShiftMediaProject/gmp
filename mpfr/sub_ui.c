/* mpfr_sub_ui -- subtract a floating-point number and a machine integer

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_sub_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int u, mp_rnd_t rnd_mode)
{
  if (u)  /* if u=0, do nothing */
  {
    mpfr_t uu;
    mp_limb_t up[1];
    unsigned long cnt;
    int inex;

    MPFR_INIT1(up, uu, BITS_PER_MP_LIMB, 1);
    count_leading_zeros(cnt, (mp_limb_t) u);
    *up = (mp_limb_t) u << cnt;
    MPFR_EXP(uu) = BITS_PER_MP_LIMB - cnt;

    /* Optimization note: Exponent operations may be removed
       if mpfr_sub works even when uu is out-of-range. */
    mpfr_save_emin_emax();
    inex = mpfr_sub(y, x, uu, rnd_mode);
    MPFR_RESTORE_RET(inex, y, rnd_mode);
  }
  else
    return mpfr_set (y, x, rnd_mode);
}
