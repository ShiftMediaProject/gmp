/* mpfr_mul_2exp -- multiply a floating-point number by a power of two

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
mpfr_mul_2exp(mpfr_ptr y, mpfr_srcptr x, unsigned long int n, unsigned char rnd_mode)
#else
mpfr_mul_2exp(y, x, n, rnd_mode)
     mpfr_ptr y;
     mpfr_srcptr x;
     unsigned long int n; 
     unsigned char rnd_mode; 
#endif
{
  /* Important particular case */ 
  if (y != x) mpfr_set(y, x, rnd_mode);
  EXP(y) += n;
  return;
}

