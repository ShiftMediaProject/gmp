/* Test file for mpfr_pow and mpfr_pow_ui.

Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "mpfr-impl.h"
#include "mpfr-test.h"

void check_pow_ui  _PROTO ((void));
void check_inexact _PROTO ((mp_prec_t));
void special       _PROTO ((void));

void
check_pow_ui (void)
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
  mpfr_pow_ui (a, b, 4294967295UL, GMP_RNDN);

  mpfr_set_inf (a, -1);
  mpfr_pow_ui (a, a, 4049053855UL, GMP_RNDN);
  if (!mpfr_inf_p (a) || (mpfr_sgn (a) >= 0))
    {
      fprintf (stderr, "Error for (-Inf)^4049053855\n");
      exit (1);
    }

  mpfr_set_inf (a, -1);
  mpfr_pow_ui (a, a, (unsigned long) 30002752, GMP_RNDN);
  if (!mpfr_inf_p (a) || (mpfr_sgn (a) <= 0))
    {
      fprintf (stderr, "Error for (-Inf)^30002752\n");
      exit (1);
    }

  mpfr_clear (a);
  mpfr_clear (b);
}

void
check_inexact (mp_prec_t p)
{
  mpfr_t x, y, z, t;
  unsigned long u;
  mp_prec_t q;
  int inexact, cmp;
  mp_rnd_t rnd;

  mpfr_init2 (x, p);
  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (t);
  mpfr_random (x);
  u = LONG_RAND() % 2;
  for (q=2; q<=p; q++)
    for (rnd=0; rnd<4; rnd++)
      {
	mpfr_set_prec (y, q);
	mpfr_set_prec (z, q + 10);
	mpfr_set_prec (t, q);
	inexact = mpfr_pow_ui (y, x, u, rnd);
	cmp = mpfr_pow_ui (z, x, u, rnd);
	if (mpfr_can_round (z, q + 10, rnd, rnd, q))
	  {
	    cmp = mpfr_set (t, z, rnd) || cmp;
	    if (mpfr_cmp (y, t))
	      {
		fprintf (stderr, "results differ for u=%lu rnd=%s\n", u,
			 mpfr_print_rnd_mode(rnd));
		printf ("x="); mpfr_print_binary (x); putchar ('\n');
		printf ("y="); mpfr_print_binary (y); putchar ('\n');
		printf ("t="); mpfr_print_binary (t); putchar ('\n');
		printf ("z="); mpfr_print_binary (z); putchar ('\n');
		exit (1);
	      }
	    if (((inexact == 0) && (cmp != 0)) ||
		((inexact != 0) && (cmp == 0)))
	      {
		fprintf (stderr, "Wrong inexact flag for p=%u, q=%u, rnd=%s\n",
			 (unsigned) p, (unsigned) q, mpfr_print_rnd_mode (rnd));
		printf ("expected %d, got %d\n", cmp, inexact);
		printf ("u=%lu x=", u); mpfr_print_binary (x); putchar ('\n');
                printf ("y="); mpfr_print_binary (y); putchar ('\n');
		exit (1);
	      }
	  }
      }

  /* check exact power */
  mpfr_set_prec (x, p);
  mpfr_set_prec (y, p);
  mpfr_set_prec (z, p);
  mpfr_set_d (x, 4.0, GMP_RNDN);
  mpfr_set_d (y, 0.5, GMP_RNDN);
  mpfr_pow (z, x, y, GMP_RNDZ);

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);
}

void
special ()
{
  mpfr_t x, y, z, t;

  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_init2 (z, 53);
  mpfr_init2 (t, 2);

  mpfr_set_prec (x, 64);
  mpfr_set_prec (y, 64);
  mpfr_set_prec (z, 64);
  mpfr_set_prec (t, 64);
  mpfr_set_str_raw (x, "0.111011000111100000111010000101010100110011010000011");
  mpfr_set_str_raw (y, "0.111110010100110000011101100011010111000010000100101");
  mpfr_set_str_raw (t, "0.1110110011110110001000110100100001001111010011111000010000011001");
    ;
  mpfr_pow (z, x, y, GMP_RNDN);
  if (mpfr_cmp (z, t))
    {
      fprintf (stderr, "Error in mpfr_pow for prec=64, rnd=GMP_RNDN\n");
      exit (1);
    }

  mpfr_set_prec (x, 53);
  mpfr_set_prec (y, 53);
  mpfr_set_prec (z, 53);
  mpfr_set_d (x, 5.68824667828621954868e-01, GMP_RNDN);
  mpfr_set_d (y, 9.03327850535952658895e-01, GMP_RNDN);
  mpfr_pow (z, x, y, GMP_RNDZ);
  if (mpfr_get_d1 (z) != 0.60071044650456473235)
    {
      fprintf (stderr, "Error in mpfr_pow for prec=53, rnd=GMP_RNDZ\n");
      exit (1);
    }

  mpfr_set_prec (t, 2);
  mpfr_set_prec (x, 30);
  mpfr_set_prec (y, 30);
  mpfr_set_prec (z, 30);
  mpfr_set_str (x, "1.00000000001010111110001111011e1", 2, GMP_RNDN);
  mpfr_set_d (t, -0.5, GMP_RNDN);
  mpfr_pow (z, x, t, GMP_RNDN);
  mpfr_set_str (y, "1.01101001111010101110000101111e-1", 2, GMP_RNDN);
  if (mpfr_cmp (z, y))
    {
      fprintf (stderr, "Error in mpfr_pow for prec=30, rnd=GMP_RNDN\n");
      exit (1);
    }

  mpfr_set_prec (x, 21);
  mpfr_set_prec (y, 21);
  mpfr_set_prec (z, 21);
  mpfr_set_str (x, "1.11111100100001100101", 2, GMP_RNDN);
  mpfr_pow (z, x, t, GMP_RNDZ);
  mpfr_set_str (y, "1.01101011010001100000e-1", 2, GMP_RNDN);
  if (mpfr_cmp (z, y))
    {
      fprintf (stderr, "Error in mpfr_pow for prec=21, rnd=GMP_RNDZ\n");
      fprintf (stderr, "Expected ");
      mpfr_out_str (stderr, 2, 0, y, GMP_RNDN);
      fprintf (stderr, "\nGot      ");
      mpfr_out_str (stderr, 2, 0, z, GMP_RNDN);
      fprintf (stderr, "\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (t);
}

int
main (void)
{
  mp_prec_t p;

  tests_start_mpfr ();

  special ();

  check_pow_ui ();

  for (p=2; p<100; p++)
    check_inexact (p);

  tests_end_mpfr ();
  return 0;
}

