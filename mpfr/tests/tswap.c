/* Test file for mpfr_swap.

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
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"

int
main (void)
{
  mpfr_t u, v;

  mpfr_init2 (u, 24);
  mpfr_init2 (v, 53);
  mpfr_set_ui (u, 16777215, GMP_RNDN); /* 2^24 - 1 */
  mpfr_set_d (v, 9007199254740991.0, GMP_RNDN); /* 2^53 - 1 */
  mpfr_swap (u, v);
  mpfr_swap (u, v);
  if (mpfr_cmp_ui (u, 16777215) || (mpfr_get_d1 (v) != 9007199254740991.0)) {
    fprintf (stderr, "Error in mpfr_swap\n");
    exit (1);
  }
  mpfr_clear (u);
  mpfr_clear (v);

  return 0;
}
