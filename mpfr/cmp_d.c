/* mpfr_cmp_d -- compare a floating-point number with a double

Copyright 2003 Free Software Foundation, Inc.

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

int 
mpfr_cmp_d (mpfr_srcptr b, double d)
{
  mpfr_t tmp; 
  int z; 

  MPFR_ASSERTN(!MPFR_IS_NAN(b));

  mpfr_init2 (tmp, IEEE_DBL_MANT_DIG);
  mpfr_set_d (tmp, d, GMP_RNDN); 

  z = mpfr_cmp (b, tmp); 
  mpfr_clear (tmp); 

  return z; 
}
