/* mpfr_tgamma -- test file for gamma function

Copyright 2001, 2002, 2003 Free Software Foundation.

This file is part of the MPFR Library, and was contributed by Mathieu Dutour.

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
#include "mpfr-test.h"

int mpfr_gamma (mpfr_ptr, mpfr_srcptr, mp_rnd_t);

int
main (void)
{
  unsigned int prec, err, yprec, n;
  mp_rnd_t rnd;
  mpfr_t x, y, z;
  double xd, yd;

  tests_start_mpfr ();

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);

  mpfr_set_prec (x, 53);
  mpfr_set_prec (y, 53);

  mpfr_set_d (x, xd = 1.0762904832837976166, GMP_RNDN);
  mpfr_gamma (y, x, GMP_RNDN);
  if (mpfr_get_d (y, GMP_RNDN) != (yd = 0.96134843256452096050))
    {
      fprintf (stderr, "mpfr_gamma(%1.20e) is wrong: expected %1.20e, got %1.20e\n",
               xd, yd, mpfr_get_d (y, GMP_RNDN));
      exit (1);
    }


  mpfr_set_d (x, xd = 9.23709516716202383435e-01, GMP_RNDN);
  mpfr_gamma (y, x, GMP_RNDN);
  if (mpfr_get_d (y, GMP_RNDN) != (yd = 1.0502315560291053398))
    {
      fprintf (stderr, "mpfr_gamma(%1.20e) is wrong: expected %1.20e, got %1.20e\n",
               xd, yd, mpfr_get_d (y, GMP_RNDN));
      exit (1);
    }

  for (prec = 2; prec <= 100; prec++)
    {
      mpfr_set_prec (x, prec);
      mpfr_set_prec (z, prec);
      yprec = prec + 10;

      for (n = 0; n < 10; n++)
	{
	  mpfr_random (x);
	  rnd = random () % 4;
	  mpfr_set_prec (y, yprec);
	  mpfr_gamma (y, x, rnd);
	  err = (rnd == GMP_RNDN) ? yprec + 1 : yprec;
	  if (mpfr_can_round (y, err, rnd, rnd, prec))
	    {
	      mpfr_round_prec (y, rnd, prec);
	      mpfr_gamma (z, x, rnd);
	      if (mpfr_cmp (y, z))
		{
		  printf ("results differ for x=");
		  mpfr_out_str (stdout, 2, prec, x, GMP_RNDN);
		  printf (" prec=%u rnd_mode=%s\n", prec,
			  mpfr_print_rnd_mode (rnd));
		  printf ("   got ");
		  mpfr_out_str (stdout, 2, prec, z, GMP_RNDN);
		  putchar ('\n');
		  printf ("   expected ");
		  mpfr_out_str (stdout, 2, prec, y, GMP_RNDN);
		  putchar ('\n');
		}
	    }
	}
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);

  tests_end_mpfr ();
  return 0;
}
