/* mpfr_reldiff -- compute relative difference of two floating-point numbers.

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
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* reldiff(b, c) = abs(b-c)/b */
void 
mpfr_reldiff (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, mp_rnd_t rnd_mode)
{
  mpfr_t b_copy;

  if (MPFR_IS_NAN(b) || MPFR_IS_NAN(c))
    { MPFR_CLEAR_FLAGS(a); MPFR_SET_NAN(a); return; }
  if (MPFR_IS_INF(b)) 
    { 
      if (MPFR_IS_INF(c) && (MPFR_SIGN(c) == MPFR_SIGN(b)))
	{ MPFR_CLEAR_FLAGS(a); MPFR_SET_ZERO(a); return; }
      else
	{ MPFR_CLEAR_FLAGS(a); MPFR_SET_NAN(a); return; }
    }

  if (MPFR_IS_INF(c)) 
    {
      MPFR_SET_SAME_SIGN(a, b);
      MPFR_CLEAR_FLAGS(a);
      MPFR_SET_INF(a);
      return;
    }

  if (MPFR_IS_ZERO(b)) /* reldiff = abs(c)/c = sign(c) */
    mpfr_set_ui(a, MPFR_SIGN(c), rnd_mode);
  else
    {
      if (a == b)
        {
          mpfr_init2 (b_copy, MPFR_PREC(b));
          mpfr_set (b_copy, b, GMP_RNDN);
        }

      mpfr_sub (a, b, c, rnd_mode);
      mpfr_abs (a, a, rnd_mode); /* for compatibility with MPF */
      mpfr_div (a, a, (a == b) ? b_copy : b, rnd_mode);
      
      if (a == b)
        mpfr_clear (b_copy);
    }
}

