/* int mpz_fits_X_p (mpz_t z) -- test whether z fits signed type X.

Copyright 1997, 2000, 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"


int
FUNCTION (mpz_srcptr z)
{
  mp_size_t n = SIZ(z);
  mp_ptr p = PTR(z);

  return (n == 0
          || (n == 1  && p[0] <= MAXIMUM)
          || (n == -1 && p[0] <= - (mp_limb_t) MINIMUM)
#if GMP_NAIL_BITS != 0	/* too broad, better compare MAXIMUM to GMP_NUMB_MAX */
	  || (n == 2 && p[1] <= (MAXIMUM >> GMP_NUMB_BITS))
	  || (n == -2 && p[1] <= (- (mp_limb_t) MINIMUM) >> GMP_NUMB_BITS)
#endif
	  );
}
