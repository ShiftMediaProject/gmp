/* mpfr_init2 -- initialize a floating-point number with given precision

Copyright (C) 2001 Free Software Foundation, Inc.

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
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void
mpfr_init2 (mpfr_ptr x, mp_prec_t p)
{
  mp_prec_t xsize; 

  if (p==0) {
    fprintf(stderr, "*** cannot initialize mpfr with precision 0\n"); exit(1);
  }

  xsize = (p - 1)/BITS_PER_MP_LIMB + 1; 

  MPFR_PREC(x) = p;
  MPFR_MANT(x) = (mp_ptr) (*__gmp_allocate_func) (xsize * BYTES_PER_MP_LIMB);
  MPFR_SIZE(x) = xsize;
  MPFR_CLEAR_FLAGS(x); 
  MPFR_SET_ZERO(x); /* initializes to zero */
  MPFR_EXP(x) = 0; /* avoids uninitialized memory reads for zero */
}
