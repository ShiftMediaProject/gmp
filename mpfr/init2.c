/* mpfr_init2 -- initialize a floating-point number with given precision

Copyright 2001 Free Software Foundation, Inc.

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

void
mpfr_init2 (mpfr_ptr x, mp_prec_t p)
{
  mp_size_t xsize;

  /* p=1 is not allowed since the rounding to nearest even rule requires at
     least two bits of mantissa: the neighbours of 3/2 are 1*2^0 and 1*2^1,
     which both have an odd mantissa */
  MPFR_ASSERTN(p >= MPFR_PREC_MIN && p <= MPFR_PREC_MAX);

  xsize = (mp_size_t) ((p - 1) / BITS_PER_MP_LIMB) + 1;

  MPFR_PREC(x) = p;
  MPFR_MANT(x) = (mp_ptr)
    (*__gmp_allocate_func) ((size_t) xsize * BYTES_PER_MP_LIMB);
  MPFR_SIZE(x) = xsize;
  MPFR_SET_NAN(x); /* initializes to NaN */
}
