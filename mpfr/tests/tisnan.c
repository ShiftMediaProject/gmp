/* Test file for mpfr_nan_p, mpfr_inf_p and mpfr_number_p.

Copyright 2001 Free Software Foundation.

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
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"

int
main (void)
{
  mpfr_t  x;

  mpfr_init (x);

  /* check +infinity gives non-zero for mpfr_inf_p only */
  mpfr_set_ui (x, 1L, GMP_RNDZ);
  mpfr_div_ui (x, x, 0L, GMP_RNDZ);
  if (mpfr_nan_p (x)) {
    fprintf (stderr, "Error: mpfr_nan_p(+Inf) gives non-zero\n");
    exit (1);
  }
  if (mpfr_inf_p (x) == 0) {
    fprintf (stderr, "Error: mpfr_inf_p(+Inf) gives zero\n");
    exit (1);
  }
  if (mpfr_number_p (x)) {
    fprintf (stderr, "Error: mpfr_number_p(+Inf) gives non-zero\n");
    exit (1);
  }

  /* same for -Inf */
  mpfr_neg (x, x, GMP_RNDN);
  if (mpfr_nan_p (x)) {
    fprintf (stderr, "Error: mpfr_nan_p(-Inf) gives non-zero\n");
    exit (1);
  }
  if (mpfr_inf_p (x) == 0) {
    fprintf (stderr, "Error: mpfr_inf_p(-Inf) gives zero\n");
    exit (1);
  }
  if (mpfr_number_p (x)) {
    fprintf (stderr, "Error: mpfr_number_p(-Inf) gives non-zero\n");
    exit (1);
  }

  /* same for NaN */
  mpfr_sub (x, x, x, GMP_RNDN);
  if (mpfr_nan_p (x) == 0) {
    fprintf (stderr, "Error: mpfr_nan_p(NaN) gives zero\n");
    exit (1);
  }
  if (mpfr_inf_p (x)) {
    fprintf (stderr, "Error: mpfr_inf_p(NaN) gives non-zero\n");
    exit (1);
  }
  if (mpfr_number_p (x)) {
    fprintf (stderr, "Error: mpfr_number_p(NaN) gives non-zero\n");
    exit (1);
  }

  /* same for an ordinary number */
  mpfr_set_ui (x, 1, GMP_RNDN);
  if (mpfr_nan_p (x)) {
    fprintf (stderr, "Error: mpfr_nan_p(1) gives non-zero\n");
    exit (1);
  }
  if (mpfr_inf_p (x)) {
    fprintf (stderr, "Error: mpfr_inf_p(1) gives non-zero\n");
    exit (1);
  }
  if (mpfr_number_p (x) == 0) {
    fprintf (stderr, "Error: mpfr_number_p(1) gives zero\n");
    exit (1);
  }

  mpfr_clear (x);

  return 0;
}
