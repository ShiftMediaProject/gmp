/* Generic test file for functions with one mpfr_t argument.

Copyright 2001, 2002 Free Software Foundation.

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

void test_generic _PROTO((int, int, int));

void
test_generic (int p0, int p1, int N)
{
  mp_prec_t prec, yprec;
  mpfr_t x, y, z, t;
  mp_rnd_t rnd;
  int inexact, compare, compare2;
  unsigned int n;

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (t);

  /* generic test */
  for (prec = p0; prec <= p1; prec++)
    {
      mpfr_set_prec (x, prec);
      mpfr_set_prec (z, prec);
      mpfr_set_prec (t, prec);
      yprec = prec + 10;

      for (n=0; n<N; n++)
	{
#if defined(RAND_FUNCTION)
	  RAND_FUNCTION (x);
#else
	  mpfr_random (x);
#endif
	  rnd = random () % 4;
	  mpfr_set_prec (y, yprec);
	  compare = TEST_FUNCTION (y, x, rnd);
	  if (mpfr_can_round (y, yprec, rnd, rnd, prec))
	    {
	      mpfr_set (t, y, rnd);
	      inexact = TEST_FUNCTION (z, x, rnd);
	      if (mpfr_cmp (t, z))
		{
		  printf ("results differ for x=");
		  mpfr_out_str (stdout, 2, prec, x, GMP_RNDN);
		  printf (" prec=%u rnd_mode=%s\n", (unsigned) prec,
			  mpfr_print_rnd_mode (rnd));
		  printf ("got      ");
		  mpfr_out_str (stdout, 2, prec, z, GMP_RNDN);
		  putchar ('\n');
		  printf ("expected ");
		  mpfr_out_str (stdout, 2, prec, t, GMP_RNDN);
		  putchar ('\n');
		  printf ("approx  ");
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
		  printf ("x="); mpfr_print_binary (x); putchar ('\n');
		  printf ("y="); mpfr_print_binary (y); putchar ('\n');
		  printf ("t="); mpfr_print_binary (t); putchar ('\n');
		  exit (1);
		}
	    }
	}
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);
}
