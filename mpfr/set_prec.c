/* mpfr_set_prec -- reset the precision of a floating-point number

Copyright (C) 1999 Free Software Foundation.

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
mpfr_set_prec (mpfr_ptr x, mp_prec_t p)
#else
mpfr_set_prec (x, p)
     mpfr_ptr x;
     mp_prec_t p;
#endif
{
  mp_prec_t xsize;

  if (p==0) {
    printf("*** cannot set precision to 0 bits\n"); exit(1);
  }

  xsize = (p - 1)/BITS_PER_MP_LIMB + 1; /* new limb size */

  if (xsize > MPFR_ABSSIZE(x)) {
    MPFR_MANT(x) = (mp_ptr) (*__gmp_reallocate_func) (MPFR_MANT(x), 
		 MPFR_ABSSIZE(x)*BYTES_PER_MP_LIMB, xsize * BYTES_PER_MP_LIMB);
    MPFR_SIZE(x) = xsize; /* new number of allocated limbs */
  }

  MPFR_PREC(x) = p;
}

mp_prec_t
#if __STDC__
mpfr_get_prec (mpfr_srcptr x)
#else
mpfr_get_prec (x)
     mpfr_srcptr x;
#endif
{
  return MPFR_PREC(x);
}
