/* mpfr_set_prec_raw -- reset the precision of a floating-point number

Copyright (C) 2000 Free Software Foundation.

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
#include "mpfr.h"
#include "mpfr-impl.h"

void
#if __STDC__
mpfr_set_prec_raw (mpfr_ptr x, mp_prec_t p)
#else
mpfr_set_prec_raw (x, p)
     mpfr_ptr x;
     mp_prec_t p;
#endif
{
  if (p==0) {
    fprintf(stderr, "*** cannot set precision to 0 bits\n"); exit(1);
  }

  if (p > MPFR_ABSSIZE(x) * BITS_PER_MP_LIMB) {
    fprintf(stderr, "*** precision too large for allocated space\n");
    exit(1);
  }

  MPFR_PREC(x) = p;
}


