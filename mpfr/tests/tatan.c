/* Test file for mpfr_arctan.

Copyright 2001, 2002 Free Software Foundation.
Written by Paul Zimmermann, INRIA Lorraine.

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
#include "mpfr-test.h"

void worst_cases _PROTO((void));
int mpfr_arctan_aux2 _PROTO((mpfr_ptr, mpfr_srcptr, mp_rnd_t));

void
worst_cases (void)
{
  mpfr_t x, y, z;
 
  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_init2 (z, 53);

  mpfr_set_str_raw (x, "1.0000100110000001100111100011001110101110100111011101");
  mpfr_set_str_raw (y, "1.1001101101110100101100110011011101101000011010111110e-1");
  mpfr_atan (z, x, GMP_RNDN);
  if (mpfr_cmp (y, z))
    {
      fprintf (stderr, "Error in mpfr_atan for prec=53, rnd=GMP_RNDN\n");
      fprintf (stderr, "x=");
      mpfr_out_str (stderr, 2, 0, x, GMP_RNDN);
      fprintf (stderr, "\nexpected ");
      mpfr_out_str (stderr, 2, 0, y, GMP_RNDN);
      fprintf (stderr, "\ngot      ");
      mpfr_out_str (stderr, 2, 0, z, GMP_RNDN);
      fprintf (stderr, "\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
}

int
main (int argc, char *argv[])
{
  unsigned int prec, err, yprec, n, p0 = 2, p1 = 100, N = 10;
  mp_rnd_t rnd;
  mpfr_t x, y, z, t;

  worst_cases ();

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (t);

  /* tarctan prec - perform one random computation with precision prec */
  if (argc >= 2)
    {
      p0 = p1 = atoi (argv[1]);
      N = 1;
    }

  for (prec = p0; prec <= p1; prec++)
    {
      mpfr_set_prec (x, prec);
      mpfr_set_prec (z, prec);
      mpfr_set_prec (t, prec);
      yprec = prec + 10;

      for (n=0; n<N; n++)
	{
	  mpfr_random (x);
	  rnd = random () % 4;
	  mpfr_set_prec (y, yprec);
	  mpfr_atan (y, x, rnd);
	  err = (rnd == GMP_RNDN) ? yprec + 1 : yprec;
	  if (mpfr_can_round (y, err, rnd, rnd, prec))
	    {
	      mpfr_set (t, y, rnd);
	      mpfr_atan (z, x, rnd);
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
