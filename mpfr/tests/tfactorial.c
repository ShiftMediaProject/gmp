/* Test file for mpfr_factorial.

Copyright 2001 Free Software Foundation.
Adapted from tarctan.c.

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
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#define TEST_FUNCTION mpfr_fac_ui

int
main (int argc, char *argv[])
{
  unsigned int prec, err, yprec, n, k, zeros;
  mp_rnd_t rnd;
  mpfr_t x, y, z, t;
  int inexact;

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (t);

  mpfr_fac_ui (y, 0, GMP_RNDN);
 
  if (mpfr_cmp_ui (y, 1))
    {
      printf ("mpfr_fac_ui(0) does not give 1\n");
      exit (1);
    }

  for (prec = 2; prec <= 100; prec++)
    {
      mpfr_set_prec (x, prec);
      mpfr_set_prec (z, prec);
      mpfr_set_prec (t, prec);
      yprec = prec + 10;
      mpfr_set_prec (y, yprec);

      for (n=0; n<100; n++)
	for (rnd=0; rnd<4; rnd++)
	  { 
	    inexact = mpfr_fac_ui (y, n, rnd);
	    err = (rnd == GMP_RNDN) ? yprec + 1 : yprec;
	    if (mpfr_can_round (y, err, rnd, rnd, prec))
	      {
		mpfr_set (t, y, rnd);
		inexact = mpfr_fac_ui (z, n, rnd);
		/* fact(n) ends with floor(n/2)+floor(n/4)+... zeros */
		for (k=n/2, zeros=0; k; k >>= 1)
		  zeros += k;
		if (MPFR_EXP(y) <= prec + zeros) /* result should be exact */
		  {
		    if (inexact)
		      {
			fprintf (stderr, "Wrong inexact flag: expected exact\n");
			exit (1);
		      }
		  }
		else /* result is inexact */
		  {
		    if (!inexact)
		      {
			fprintf (stderr, "Wrong inexact flag: expected inexact\n");
			printf ("n=%u prec=%u\n", n, prec);
			mpfr_print_binary(z); putchar('\n');
			exit (1);
		      }
		  }
		if (mpfr_cmp (t, z))
		  {
		    printf ("results differ for x=");
		    mpfr_out_str (stdout, 2, prec, x, GMP_RNDN);
		    printf (" prec=%u rnd_mode=%s\n", prec,
			    mpfr_print_rnd_mode (rnd));
		    printf ("   got ");
		    mpfr_out_str (stdout, 2, prec, z, GMP_RNDN);
		    putchar ('\n');
		    printf ("   expected ");
		    mpfr_out_str (stdout, 2, prec, t, GMP_RNDN);
		    putchar ('\n');
		    printf ("   approximation was ");
		    mpfr_print_binary (y);
		    putchar ('\n');
		    exit (1);
		  }
	      }
	  }
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);

  return 0;
}
