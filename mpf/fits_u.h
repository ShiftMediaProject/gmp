/* mpf_fits_u*_p -- test whether an mpf fits a C unsigned type.

Copyright 2001 Free Software Foundation, Inc.

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


/* Notice this is equivalent to mpz_set_f + mpz_fits_u*_p.  */

int
FUNCTION (mpf_srcptr f)
{
  mp_size_t  fn, i;
  mp_srcptr  fp;
  mp_exp_t   exp;
  mp_limb_t  fl;

  fn = SIZ(f);
  if (fn <= 0)
    return fn == 0;  /* zero fits, negatives don't */

  exp = EXP(f);
  fp = PTR(f);

  if (exp == 1)
    {
      fn -= 1;			/* decrement to point at first fraction limb */
      fl = fp[fn];
    }
#if GMP_NAIL_BITS != 0
  else if (exp == 2 && MAXIMUM > GMP_NUMB_MAX)
    {
      fn -= 1;
      fl = fp[fn];
      if ((fl >> GMP_NAIL_BITS) != 0)
	return 0;
      fl = (fl << GMP_NUMB_BITS);
      if (fn >= 1)
	{
	  fn -= 1;		/* decrement to point at first fraction limb */
	  fl |= fp[fn];
	}
    }
#endif
  else
    return 0;

  /* any fraction limbs must be zero */
  for (i = fn - 1; i >= 0; i--)
    if (fp[i] != 0)
      return 0;

  return fl <= MAXIMUM;
}
