/* mpfr_ui_pow -- power of n function n^x

Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

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

#include <limits.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_ui_pow (mpfr_ptr y, unsigned long int n, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  mpfr_t t;
  int inexact;

  mpfr_save_emin_emax();
  mpfr_init2 (t, sizeof(n) * CHAR_BIT);
  inexact = mpfr_set_ui (t, n, GMP_RNDN);
  MPFR_ASSERTN (!inexact);
  inexact = mpfr_pow (y, t, x, rnd_mode);
  mpfr_clear (t);
  mpfr_restore_emin_emax ();
  return mpfr_check_range (y, inexact, rnd_mode);
}
