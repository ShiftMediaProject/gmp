/* mpfr_init -- initialize a floating-point number

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
mpfr_init2 (mpfr_t x, unsigned long int p)
#else
mpfr_init2 (x, p)
     mpfr_t x;
     unsigned long int p;
#endif
{
  unsigned long xsize; 

  if (p==0) {
    printf("*** cannot initialize mpfr with precision 0\n"); exit(1);
  }

  xsize = (p - 1)/BITS_PER_MP_LIMB + 1; 

  x -> _mp_prec = p;
  x -> _mp_d = (mp_ptr) (*_mp_allocate_func) 
    (xsize * BYTES_PER_MP_LIMB);
  x -> _mp_size = xsize;
  x -> _mp_exp = 0; /* avoids uninitialized memory reads for zero */
}
