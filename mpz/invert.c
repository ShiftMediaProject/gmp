/* mpz_invert (inv, x, n).  Find multiplicative inverse of X in Z(N).
   If X has an inverse, return non-zero and store inverse in INVERSE,
   otherwise, return 0 and put garbage in INVERSE.

Copyright (C) 1996 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"

int
#if __STDC__
mpz_invert (mpz_ptr inverse, mpz_srcptr x, mpz_srcptr n)
#else
mpz_invert (inverse, x, n)
     mpz_ptr inverse;
     mpz_srcptr x, n;
#endif
{
  mpz_t gcd;
  mp_size_t xsize, nsize, size;

  xsize = SIZ (x);
  nsize = SIZ (n);
  xsize = ABS (xsize);
  nsize = ABS (nsize);
  size = MAX (xsize, nsize) + 1;

  /* No inverse exists if the leftside operand is 0.  Likewise, no
     inverse exists if the mod operand is 1.  */
  if (xsize == 0 || (nsize == 1 && (PTR (n))[0] == 1))
    return 0;

  MPZ_TMP_INIT (gcd, size);
  mpz_gcdext (gcd, inverse, (mpz_ptr) 0, x, n);

  /* If no inverse existed, return with an indication of that.  */
  if (gcd->_mp_size != 1 || (gcd->_mp_d)[0] != 1)
    return 0;

  /* Make sure we return a positive inverse.  */
  if (SIZ (inverse) < 0)
    {
      if (SIZ (n) < 0)
	mpz_sub (inverse, inverse, n);
      else
	mpz_add (inverse, inverse, n);
    }
  return 1;
}
