/* mpfr_set_prec_raw -- reset the precision of a floating-point number

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

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

void
mpfr_set_prec_raw (mpfr_ptr x, mp_prec_t p)
{
  MPFR_ASSERTN(p >= MPFR_PREC_MIN && p <= MPFR_PREC_MAX);

  if (p > (mp_prec_t) MPFR_ABSSIZE(x) * BITS_PER_MP_LIMB)
    {
      fprintf (stderr, "*** precision too large for allocated space\n");
      exit (1);
    }

  MPFR_PREC(x) = p;
}
