/* comparison predicates

Copyright 2002, 2003 Free Software Foundation.

This file is part of the MPFR Library.
Contributed by the Spaces project (LORIA/LIP6).

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

/* Note: these functions currently use mpfr_cmp; they could have their
   own code to be faster. */

/*                          =     <     >     unordered
 * mpfr_greater_p           0     0     1     0
 * mpfr_greaterequal_p      1     0     1     0
 * mpfr_less_p              0     1     0     0
 * mpfr_lessequal_p         1     1     0     0
 * mpfr_lessgreater_p       0     1     1     0
 * mpfr_equal_p             1     0     0     0
 * mpfr_unordered_p         0     0     0     1
 */

int
mpfr_greater_p (mpfr_srcptr x, mpfr_srcptr y)
{
  return MPFR_IS_NAN(x) || MPFR_IS_NAN(y) ? 0 : (mpfr_cmp (x, y) > 0);
}

int
mpfr_greaterequal_p (mpfr_srcptr x, mpfr_srcptr y)
{
  return MPFR_IS_NAN(x) || MPFR_IS_NAN(y) ? 0 : (mpfr_cmp (x, y) >= 0);
}

int
mpfr_less_p (mpfr_srcptr x, mpfr_srcptr y)
{
  return MPFR_IS_NAN(x) || MPFR_IS_NAN(y) ? 0 : (mpfr_cmp (x, y) < 0);
}

int
mpfr_lessequal_p (mpfr_srcptr x, mpfr_srcptr y)
{
  return MPFR_IS_NAN(x) || MPFR_IS_NAN(y) ? 0 : (mpfr_cmp (x, y) <= 0);
}

int
mpfr_lessgreater_p (mpfr_srcptr x, mpfr_srcptr y)
{
  return MPFR_IS_NAN(x) || MPFR_IS_NAN(y) ? 0 : (mpfr_cmp (x, y) != 0);
}

int
mpfr_equal_p (mpfr_srcptr x, mpfr_srcptr y)
{
  return MPFR_IS_NAN(x) || MPFR_IS_NAN(y) ? 0 : (mpfr_cmp (x, y) == 0);
}

int
mpfr_unordered_p (mpfr_srcptr x, mpfr_srcptr y)
{
  return MPFR_IS_NAN(x) || MPFR_IS_NAN(y);
}
