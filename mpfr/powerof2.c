/* mpfr_powerof2_raw -- test whether a floating-point number is a power of 2

Copyright 2002 Free Software Foundation.

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
#include "mpfr.h"
#include "mpfr-impl.h"

/* This is an internal function and one assumes that x is a real number. */

int
mpfr_powerof2_raw (mpfr_srcptr x)
{
  mp_limb_t *xp;
  mp_size_t xn;

  MPFR_ASSERTN(MPFR_IS_FP(x));
  xp = MPFR_MANT(x);
  xn = (MPFR_PREC(x) - 1) / BITS_PER_MP_LIMB;
  if (NOT_POW2(xp[xn]))
    return 0;
  while (xn > 0)
    if (xp[--xn] != 0)
      return 0;
  return 1;
}
