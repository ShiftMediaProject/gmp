/*  mpfr_ui_pow_ui -- compute the power beetween two machine integer

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_ui_pow_ui (mpfr_ptr x, unsigned long int y, unsigned long int n,
                mp_rnd_t rnd)
{
  long int err;
  unsigned long m;
  mpfr_t res;
  mp_prec_t prec;
  int inexact;

  MPFR_CLEAR_FLAGS(x);

  if (n == 0) /* y^0 = 1 for any y */
    return mpfr_set_ui (x, 1, rnd);

  if (y == 0) /* 0^n = 0 for any n > 0 */
    return mpfr_set_ui (x, 0, rnd);

  mpfr_save_emin_emax ();
  mpfr_init (res);

  prec = MPFR_PREC(x);

  do
    {
      int i;

      prec += 3;
      for (i = 0, m = n; m; i++, m >>= 1)
        prec++;
      mpfr_set_prec (res, prec);
      inexact = mpfr_set_ui (res, y, GMP_RNDU);
      err = 1;
      /* now 2^(i-1) <= n < 2^i */
      for (i-=2; i>=0; i--)
	{
	  if (mpfr_mul (res, res, res, GMP_RNDU))
	    inexact = 1;
	  err++;
	  if (n & (1 << i))
	    if (mpfr_mul_ui (res, res, y, GMP_RNDU))
	      inexact = 1;
	}
      err = prec - err;
      if (err < 0)
	err = 0;
    }
  while (inexact && (mpfr_can_round (res, err,
          MPFR_SIGN(res) > 0 ? GMP_RNDU : GMP_RNDD, rnd, MPFR_PREC(x)) == 0));

  if (mpfr_set (x, res, rnd))
    inexact = 1;

  mpfr_clear (res);

  MPFR_RESTORE_RET(inexact, x, rnd);
}
