/* Test file for mpfr_pow and mpfr_pow_ui.

Copyright (C) 2000 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "mpfr.h"

void check_pow_ui (void);

void check_pow_ui ()
{
  mpfr_t a, b;

  mpfr_init2 (a, 53);
  mpfr_init2 (b, 53);

  /* check in-place operations */
  mpfr_set_d (b, 0.6926773, GMP_RNDN);
  mpfr_pow_ui (a, b, 10, GMP_RNDN);
  mpfr_pow_ui (b, b, 10, GMP_RNDN);
  if (mpfr_cmp (a, b)) {
    fprintf (stderr, "Error for mpfr_pow_ui (b, b, ...)\n"); exit (1);
  }

  /* check large exponents */
  mpfr_set_d (b, 1, GMP_RNDN);
  mpfr_pow_ui (a, b, (unsigned long) 4294967295UL, GMP_RNDN);

  mpfr_set_d (a, -1.0/0.0, GMP_RNDN);
  mpfr_pow_ui (a, a, 4049053855UL, GMP_RNDN);
  if (mpfr_get_d (a) != -1.0/0.0) {
    fprintf (stderr, "Error for (-Inf)^4049053855\n"); exit (1);
  }

  mpfr_set_d (a, -1.0/0.0, GMP_RNDN);
  mpfr_pow_ui (a, a, 30002752UL, GMP_RNDN);
  if (mpfr_get_d (a) != 1.0/0.0) {
    fprintf (stderr, "Error for (-Inf)^30002752\n"); exit (1);
  }

  mpfr_clear (a);
  mpfr_clear (b);
}

int main ()
{
  check_pow_ui ();
  return 0;
}
