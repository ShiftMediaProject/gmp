/* mpfr_sqrt_ui -- square root of a machine integer

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
mpfr_sqrt_ui (mpfr_ptr r, unsigned long u, mp_rnd_t rnd_mode)
{
  if (u)
    {
      mpfr_t uu;
      mp_limb_t up[1];
      unsigned long cnt;
      int inex;

      MPFR_INIT1(up, uu, BITS_PER_MP_LIMB, 1);
      count_leading_zeros (cnt, (mp_limb_t) u);
      *up = (mp_limb_t) u << cnt;
      MPFR_EXP(uu) = BITS_PER_MP_LIMB - cnt;

      mpfr_save_emin_emax();
      inex = mpfr_sqrt(r, uu, rnd_mode);
      MPFR_RESTORE_RET(inex, r, rnd_mode);
    }
  else /* sqrt(0) = 0 */
    {
      MPFR_CLEAR_FLAGS(r);
      MPFR_SET_ZERO(r);
      MPFR_SET_POS(r);
      MPFR_RET(0);
    }
}
