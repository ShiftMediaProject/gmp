/* mpfr_swap (U, V) -- Swap U and V.

Copyright 2000, 2001 Free Software Foundation, Inc.

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
#include "mpfr.h"
#include "mpfr-impl.h"

void
mpfr_swap (mpfr_ptr u, mpfr_ptr v)
{
  mp_ptr up, vp;
  mp_size_t usize, vsize;
  mp_prec_t uprec, vprec;
  mp_exp_t  uexp, vexp;

  uprec = MPFR_PREC(u);
  vprec = MPFR_PREC(v);
  MPFR_PREC(v) = uprec;
  MPFR_PREC(u) = vprec;

  usize = MPFR_SIZE(u);
  vsize = MPFR_SIZE(v);
  MPFR_SIZE(v) = usize;
  MPFR_SIZE(u) = vsize;

  uexp = MPFR_EXP(u);
  vexp = MPFR_EXP(v);
  MPFR_EXP(v) = uexp;
  MPFR_EXP(u) = vexp;

  up = MPFR_MANT(u);
  vp = MPFR_MANT(v);
  MPFR_MANT(v) = up;
  MPFR_MANT(u) = vp;
}
