/* Test file for mpfr_hypot.

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
#include <limits.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"
   
#define TEST_FUNCTION mpfr_hypot

int
main (int argc, char *argv[])
{
  unsigned int prec, err, yprec, n, p0 = 2, p1 = 100, N = 100;
  mp_rnd_t rnd;
  mpfr_t x1, x2, y, z, t;
  int inexact, compare, compare2;

  mpfr_init (x1);
  mpfr_init (x2);
  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (t);

  /* thypot prec - perform one random computation with precision prec */
  if (argc >= 2)
    {
      p0 = p1 = atoi (argv[1]);
      N = 1;
    }

  for (prec = p0; prec <= p1; prec++)
    {
      mpfr_set_prec (x1, prec);
      mpfr_set_prec (x2, prec);
      mpfr_set_prec (z, prec);
      mpfr_set_prec (t, prec);
      yprec = prec + 10;

      for (n=0; n<N; n++)
	{
	  mpfr_random(x1);
	  mpfr_random(x2);
          if (random() % 2)
	    mpfr_neg (x1, x1, GMP_RNDN);
          if (random() % 2)
	    mpfr_neg (x2, x2, GMP_RNDN);
	  rnd = random () % 4;
	  mpfr_set_prec (y, yprec);

	  compare =TEST_FUNCTION (y, x1,x2, rnd);
	  err = (rnd == GMP_RNDN) ? yprec + 1 : yprec;
	  if (mpfr_can_round (y, err, rnd, rnd, prec))
	    {
	      mpfr_set (t, y, rnd);
	      inexact = TEST_FUNCTION (z, x1,x2, rnd);
	      if (mpfr_cmp (t, z))
		{ 
		  printf ("results differ for x1=");
		  mpfr_out_str (stdout, 2, prec, x1, GMP_RNDN);
		  printf ("\n et x2=");
		  mpfr_out_str (stdout, 2, prec, x2, GMP_RNDN);
		  printf (" \n prec=%u rnd_mode=%s\n", prec,
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
	      compare2 = mpfr_cmp (t, y);
	      /* if rounding to nearest, cannot know the sign of t - f(x)
		 because of composed rounding: y = o(f(x)) and t = o(y) */
	      if ((rnd != GMP_RNDN) && (compare * compare2 >= 0))
		compare = compare + compare2;
	      else
		compare = inexact; /* cannot determine sign(t-f(x)) */
	      if (((inexact == 0) && (compare != 0)) ||
		  ((inexact > 0) && (compare <= 0)) ||
		  ((inexact < 0) && (compare >= 0)))
              {
		  fprintf (stderr, "Wrong inexact flag for rnd=%s: expected %d, got %d\n",
			   mpfr_print_rnd_mode (rnd), compare, inexact);
		  printf ("x1="); mpfr_print_binary (x1); putchar ('\n');
		  printf ("x2="); mpfr_print_binary (x2); putchar ('\n');
		  printf ("t="); mpfr_print_binary (t); putchar ('\n');
		  exit (1);
              }
	    }
	}
    }

  mpfr_clear (x1);
  mpfr_clear (x2);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);

  return 0;
}




