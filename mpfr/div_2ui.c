/* mpfr_div_2ui -- divide a floating-point number by a power of two

Copyright 1999, 2001 Free Software Foundation, Inc.

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
mpfr_div_2ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int n, mp_rnd_t rnd_mode)
{
  int inexact;

  inexact = y != x ? mpfr_set (y, x, rnd_mode) : 0;

  if (MPFR_IS_FP(y) && MPFR_NOTZERO(y))
    {
      /* n will have to be casted to long to make sure that the addition
         and subtraction below (for overflow detection) are signed */
      while (n > LONG_MAX)
        {
          int inex2;

          n -= LONG_MAX;
          inex2 = mpfr_div_2ui(y, y, LONG_MAX, rnd_mode);
          if (inex2)
            return inex2; /* underflow */
        }

      /* MPFR_EMAX_MAX - (long) n is signed and doesn't lead to an overflow;
         the first test useful so that the real test can't lead to an
         overflow. */
      if (__mpfr_emin > MPFR_EMAX_MAX - (long) n ||
          MPFR_EXP(y) < __mpfr_emin + (long) n)
        return mpfr_set_underflow (y, rnd_mode, MPFR_SIGN(y));

      MPFR_EXP(y) -= (long) n;
    }

  return inexact;
}
