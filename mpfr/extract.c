/* mpfr_extract -- bit-extraction function for the binary splitting algorithm

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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

/* given 0 <= |p| < 1, this function extracts limbs of p and puts them in y.
   It is mainly designed for the "binary splitting" algorithm together with
   generic.c.

   More precisely, if B = 2^BITS_PER_MP_LIMB:
   - for i=0, y = floor(p * B)
   - for i>0, y = (p * B^(2^i)) mod B^(2^(i-1))
 */

void
mpfr_extract (mpz_ptr y, mpfr_srcptr p, unsigned int i)
{
  int two_i = 1 << i;
  int two_i_2 = i ? two_i / 2 : 1;
  mp_size_t size_p = MPFR_ABSSIZE(p);

  /* as 0 <= |p| < 1, we don't have to care with infinities, NaN, ... */
  
  _mpz_realloc (y, two_i_2);
  if (size_p < two_i)
    {
      MPN_ZERO (PTR(y), two_i_2);
      if (size_p >= two_i_2)
        MPN_COPY (PTR(y) + two_i - size_p, MPFR_MANT(p), size_p - two_i_2);
    }
  else
    MPN_COPY (PTR(y), MPFR_MANT(p) + size_p - two_i, two_i_2);

  MPN_NORMALIZE (PTR(y), two_i_2);
  SIZ(y) = (MPFR_ISNEG(p)) ? -two_i_2 : two_i_2;
}
