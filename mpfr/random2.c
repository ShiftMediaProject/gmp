/* mpfr_random2 -- Generate a positive random mpfr_t of specified size, with
   long runs of consecutive ones and zeros in the binary representation.
   Intended for testing.

Copyright 1999, 2001, 2002, 2003 Free Software Foundation, Inc.
(Copied from the GNU MP Library.)

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

void
mpfr_random2 (mpfr_ptr x, mp_size_t size, mp_exp_t exp)
{
  mp_size_t xn;
  unsigned long cnt;
  mp_ptr xp;
  mp_size_t prec;
  mp_limb_t elimb;

  MPFR_CLEAR_FLAGS(x);
  MPFR_SET_POS(x);
  xn = ABS (size);
  prec = (MPFR_PREC(x) - 1) / GMP_NUMB_BITS;
  xp = MPFR_MANT(x);

  if (xn == 0)
    {
      MPFR_SET_ZERO(x);
      return;
    }

  if (xn > prec + 1)
    xn = prec + 1;

  /* General random mantissa.  */
  mpn_random2 (xp, xn);

  /* Set mandatory most significant bit.  */
  xp[xn - 1] |= MPFR_LIMB_HIGHBIT;

  /* Generate random exponent.  */
  _gmp_rand (&elimb, RANDS, GMP_NUMB_BITS);
  exp = ABS (exp);
  MPFR_SET_EXP (x, elimb % (2 * exp + 1) - exp);

  /* Mask off non significant bits in the low limb.  */
  cnt = xn * GMP_NUMB_BITS - MPFR_PREC(x);
  xp[0] &= ~((MP_LIMB_T_ONE << cnt) - 1);
}
