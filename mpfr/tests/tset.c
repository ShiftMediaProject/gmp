/* Test file for mpfr_set.

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-test.h"

int
main (void)
{
  mp_prec_t p, q;
  mpfr_t x, y, z, u;
  mp_rnd_t rnd;
  int inexact, cmp;

  /* check prototypes of mpfr_init_set_* */
  inexact = mpfr_init_set_si (x, -1, GMP_RNDN);
  inexact = mpfr_init_set (y, x, GMP_RNDN);
  inexact = mpfr_init_set_ui (z, 1, GMP_RNDN);
  inexact = mpfr_init_set_d (u, 1.0, GMP_RNDN);

  mpfr_set_prec (y, 11);
  mpfr_set_str_raw (y, "0.11111111100E-8");
  mpfr_set_prec (x, 2);
  mpfr_set (x, y, GMP_RNDN);
  mpfr_set_str_raw (y, "1.0E-8");
  if (mpfr_cmp (x, y))
    {
      fprintf (stderr, "Error for y=0.11111111100E-8, prec=2, rnd=GMP_RNDN\n");
      exit (1);
    }
  
  for (p=2; p<500; p++)
    {
      mpfr_set_prec (x, p);
      mpfr_random (x);
      if (LONG_RAND () % 2)
	mpfr_neg (x, x, GMP_RNDN);
      for (q=2; q<2*p; q++)
	{
	  mpfr_set_prec (y, q);
	  for (rnd=0; rnd<4; rnd++)
	    {
	      inexact = mpfr_set (y, x, rnd);
	      cmp = mpfr_cmp (y, x);
	      if (((inexact == 0) && (cmp != 0)) ||
		  ((inexact > 0) && (cmp <= 0)) ||
		  ((inexact < 0) && (cmp >= 0)))
		{
		  fprintf (stderr, "Wrong inexact flag in mpfr_set: expected %d, got %d\n", cmp, inexact);
		  exit (1);
		}
	    }
	}
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (u);
  return 0;
}
