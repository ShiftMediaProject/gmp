/* _mpfr_ceil_exp2 - returns y >= 2^d

Copyright 1999, 2000, 2001, 2002 Free Software Foundation.

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

/* returns y >= 2^d, assuming that d <= 1024 */
double
_mpfr_ceil_exp2 (double d)
{
  long exp;
  union ieee_double_extract x;

  MPFR_ASSERTN(d <= 1024.0);
  exp = (long) d;
  if (d != (double) exp)
    exp++;
  /* now exp = ceil(d) */
  x.d = 1.0;
  x.s.exp = exp <= -1022 ? 1 : 1023 + exp;
  return x.d;
}
