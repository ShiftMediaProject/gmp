/* mpfr_set_prec -- reset the precision of a floating-point number

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

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

void
#if __STDC__
mpfr_set_prec (mpfr_t x, unsigned long int p)
#else
mpfr_set_prec (x, p)
     mpfr_t x;
     unsigned long int p;
#endif
{
  unsigned long xsize;

  if (p==0) {
    printf("*** cannot set precision to 0 bits\n"); exit(1);
  }

  xsize = (p - 1)/BITS_PER_MP_LIMB + 1; /* new limb size */

  if (xsize > ABSSIZE(x)) {
    x -> _mp_d = (mp_ptr) (*_mp_reallocate_func) 
      (x -> _mp_d, ABSSIZE(x)*BYTES_PER_MP_LIMB, xsize * BYTES_PER_MP_LIMB);
    SIZE(x) = xsize; /* new number of allocated limbs */
  }

  x -> _mp_prec = p;
}

unsigned long int
#if __STDC__
mpfr_get_prec (mpfr_t x)
#else
mpfr_get_prec (x)
     mpfr_t x;
#endif
{
  return x -> _mp_prec;
}
