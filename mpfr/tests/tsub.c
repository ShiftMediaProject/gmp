/* Test file for mpfr_sub.

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
#include "mpfr-impl.h"
#include "mpfr-test.h"

void check_diverse _PROTO((void));
void bug_ddefour _PROTO((void));
void check_two_sum _PROTO((mp_prec_t));
void check_inexact _PROTO((void));

void
check_diverse (void)
{
  mpfr_t x, y, z;
  double res, got;
  int inexact;

  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);

  mpfr_set_prec (x, 32);
  mpfr_set_prec (y, 63);
  mpfr_set_prec (z, 63);
  mpfr_set_str_raw (x, "0.101101111011011100100100100111E31");
  mpfr_set_str_raw (y, "0.111110010010100100110101101010001001100101110001000101110111111E-1");
  mpfr_sub (z, x, y, GMP_RNDN);
  mpfr_set_str_raw (y, "0.1011011110110111001001001001101100000110110101101100101001011E31");
  if (mpfr_cmp (z, y))
    {
      fprintf (stderr, "Error in mpfr_sub (5)\n");
      printf ("expected "); mpfr_print_binary (y); putchar ('\n');
      printf ("got      "); mpfr_print_binary (z); putchar ('\n');
      exit (1);
    }

  mpfr_set_prec (y, 63);
  mpfr_set_prec (z, 63);
  mpfr_set_str_raw (y, "0.1011011110110111001001001001101100000110110101101100101001011E31");
  mpfr_sub_ui (z, y, 1541116494, GMP_RNDN);
  mpfr_set_str_raw (y, "-0.11111001001010010011010110101E-1");
  if (mpfr_cmp (z, y))
    {
      fprintf (stderr, "Error in mpfr_sub (7)\n");
      printf ("expected "); mpfr_print_binary (y); putchar ('\n');
      printf ("got      "); mpfr_print_binary (z); putchar ('\n');
      exit (1);
    }

  mpfr_set_prec (y, 63);
  mpfr_set_prec (z, 63);
  mpfr_set_str_raw (y, "0.1011011110110111001001001001101100000110110101101100101001011E31");
  mpfr_sub_ui (z, y, 1541116494, GMP_RNDN);
  mpfr_set_str_raw (y, "-0.11111001001010010011010110101E-1");
  if (mpfr_cmp (z, y))
    {
      fprintf (stderr, "Error in mpfr_sub (6)\n");
      printf ("expected "); mpfr_print_binary (y); putchar ('\n');
      printf ("got      "); mpfr_print_binary (z); putchar ('\n');
      exit (1);
    }

  mpfr_set_prec (x, 32);
  mpfr_set_prec (y, 32);
  mpfr_set_str_raw (x, "0.10110111101001110100100101111000E0");
  mpfr_set_str_raw (y, "0.10001100100101000100110111000100E0");
  if ((inexact = mpfr_sub (x, x, y, GMP_RNDN)))
    {
      fprintf (stderr, "Wrong inexact flag (2): got %d instead of 0\n",
	       inexact);
      exit (1);
    }

  mpfr_set_prec (x, 32);
  mpfr_set_prec (y, 32);
  mpfr_set_str_raw (x, "0.11111000110111011000100111011010E0");
  mpfr_set_str_raw (y, "0.10011111101111000100001000000000E-3");
  if ((inexact = mpfr_sub (x, x, y, GMP_RNDN)))
    {
      fprintf (stderr, "Wrong inexact flag (1): got %d instead of 0\n",
	       inexact);
      exit (1);
    }

  mpfr_set_prec (x, 33);
  mpfr_set_prec (y, 33);
  mpfr_set_ui (x, 1, GMP_RNDN);
  mpfr_set_str_raw (y, "-0.1E-32");
  mpfr_add (x, x, y, GMP_RNDN);
  mpfr_set_str_raw (y, "0.111111111111111111111111111111111E0");
  if (mpfr_cmp (x, y))
    {
      fprintf (stderr, "Error in mpfr_sub (1 - 1E-33) with prec=33\n");
      printf ("Expected "); mpfr_print_binary (y); putchar ('\n');
      printf ("got      "); mpfr_print_binary (x); putchar ('\n');
      exit (1);
    }

  mpfr_set_prec (x, 32);
  mpfr_set_prec (y, 33);
  mpfr_set_ui (x, 1, GMP_RNDN);
  mpfr_set_str_raw (y, "-0.1E-32");
  mpfr_add (x, x, y, GMP_RNDN);
  if (mpfr_cmp_ui (x, 1))
    {
      fprintf (stderr, "Error in mpfr_sub (1 - 1E-33) with prec=32\n");
      printf ("Expected 1.0, got "); mpfr_print_binary (x); putchar ('\n');
      exit (1);
    }

  mpfr_set_prec (x, 65);
  mpfr_set_prec (y, 65);
  mpfr_set_prec (z, 64);
  mpfr_set_str_raw (x, "1.1110111011110001110111011111111111101000011001011100101100101101");
  mpfr_set_str_raw (y, "0.1110111011110001110111011111111111101000011001011100101100101100");
  mpfr_sub (z, x, y, GMP_RNDZ);
  if (mpfr_cmp_ui (z, 1))
    {
      fprintf (stderr, "Error in mpfr_sub (1)\n");
      exit (1);
    }
  mpfr_sub (z, x, y, GMP_RNDU);
  mpfr_set_str_raw (x, "1.000000000000000000000000000000000000000000000000000000000000001");
  if (mpfr_cmp (z, x))
    {
      fprintf (stderr, "Error in mpfr_sub (2)\n");
      exit (1);
    }
  mpfr_set_str_raw (x, "1.1110111011110001110111011111111111101000011001011100101100101101");
  mpfr_sub (z, x, y, GMP_RNDN);
  if (mpfr_cmp_ui (z, 1))
    {
      fprintf (stderr, "Error in mpfr_sub (3)\n");
      exit (1);
    }
  mpfr_set_prec (x, 66);
  mpfr_set_str_raw (x, "1.11101110111100011101110111111111111010000110010111001011001010111");
  mpfr_sub (z, x, y, GMP_RNDN);
  if (mpfr_cmp_ui (z, 1))
    {
      fprintf (stderr, "Error in mpfr_sub (4)\n");
      exit (1);
    }
  
  /* check in-place operations */
  mpfr_set_d (x, 1.0, GMP_RNDN);
  mpfr_sub (x, x, x, GMP_RNDN);
  if (mpfr_get_d1 (x) != 0.0)
    {
      fprintf (stderr, "Error for mpfr_add (x, x, x, GMP_RNDN) with x=1.0\n");
      exit (1);
    }

  mpfr_set_prec (x, 53);
  mpfr_set_prec (y, 53);
  mpfr_set_prec (z, 53);
  mpfr_set_d (x, 1.229318102e+09, GMP_RNDN);
  mpfr_set_d (y, 2.32221184180698677665e+05, GMP_RNDN);
  mpfr_sub (z, x, y, GMP_RNDN);
  res = 1229085880.815819263458251953125;
  got = mpfr_get_d1 (z);
  if (got != res)
    {
      fprintf (stderr, "Error in mpfr_sub (1.22e9 - 2.32e5)\n");
      printf ("expected %1.20e, got %1.20e\n", res, got);
      exit (1);
    }

  mpfr_set_prec (x, 112);
  mpfr_set_prec (y, 98);
  mpfr_set_prec (z, 54);
  mpfr_set_str_raw (x, "0.11111100100000000011000011100000101101010001000111E-401");
  mpfr_set_str_raw (y, "0.10110000100100000101101100011111111011101000111000101E-464");
  mpfr_sub (z, x, y, GMP_RNDN);
  if (mpfr_cmp (z, x)) {
    fprintf (stderr, "mpfr_sub(z, x, y) failed for prec(x)=112, prec(y)=98\n");
    printf ("expected "); mpfr_print_binary (x); putchar('\n');
    printf ("got      "); mpfr_print_binary (z); putchar('\n');
    exit (1);
  }

  mpfr_set_prec (x, 33);
  mpfr_set_ui (x, 1, GMP_RNDN);
  mpfr_div_2exp (x, x, 32, GMP_RNDN);
  mpfr_sub_ui (x, x, 1, GMP_RNDN);

  mpfr_set_prec (x, 5);
  mpfr_set_prec (y, 5);
  mpfr_set_str_raw (x, "1e-12");
  mpfr_set_ui (y, 1, GMP_RNDN);
  mpfr_sub (x, y, x, GMP_RNDD);
  mpfr_set_str_raw (y, "0.11111");
  if (mpfr_cmp (x, y))
    {
      fprintf (stderr, "Error in mpfr_sub (x, y, x, GMP_RNDD) for x=2^(-12), y=1\n");
      exit (1);
    }

  mpfr_set_prec (x, 24);
  mpfr_set_prec (y, 24);
  mpfr_set_str_raw (x, "-0.100010000000000000000000E19");
  mpfr_set_str_raw (y, "0.100000000000000000000100E15");
  mpfr_add (x, x, y, GMP_RNDD);
  mpfr_set_str_raw (y, "-0.1E19");
  if (mpfr_cmp (x, y))
    {
      fprintf (stderr, "Error in mpfr_add (2)\n");
      exit (1);
    }

  mpfr_set_prec (x, 2);
  mpfr_set_prec (y, 10);
  mpfr_set_prec (z, 10);
  mpfr_set_ui (y, 0, GMP_RNDN);
  mpfr_set_str_raw (z, "0.100000000000000000000100E15");
  if (mpfr_sub (x, y, z, GMP_RNDN) <= 0)
    {
      fprintf (stderr, "Wrong inexact flag in x=mpfr_sub(0,z) for prec(z)>prec(x)\n");
      exit (1);
    }
  if (mpfr_sub (x, z, y, GMP_RNDN) >= 0)
    {
      fprintf (stderr, "Wrong inexact flag in x=mpfr_sub(z,0) for prec(z)>prec(x)\n");
      exit (1);
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
}

void
bug_ddefour(void)
{
    mpfr_t ex, ex1, ex2, ex3, tot, tot1;

    mpfr_init2(ex, 53);
    mpfr_init2(ex1, 53);
    mpfr_init2(ex2, 53);
    mpfr_init2(ex3, 53);
    mpfr_init2(tot, 150);
    mpfr_init2(tot1, 150);

    mpfr_set_ui( ex, 1, GMP_RNDN);
    mpfr_mul_2exp( ex, ex, 906, GMP_RNDN);
    mpfr_log( tot, ex, GMP_RNDN);
    mpfr_set( ex1, tot, GMP_RNDN); /* ex1 = high(tot) */
    mpfr_sub( ex2, tot, ex1, GMP_RNDN); /* ex2 = high(tot - ex1) */
    mpfr_sub( tot1, tot, ex1, GMP_RNDN); /* tot1 = tot - ex1 */
    mpfr_set( ex3, tot1, GMP_RNDN); /* ex3 = high(tot - ex1) */

    if (mpfr_cmp(ex2, ex3)) 
      {
	fprintf (stderr, "Error in ddefour test.\n");
	printf ("ex2="); mpfr_print_binary (ex2); putchar ('\n');
	printf ("ex3="); mpfr_print_binary (ex3); putchar ('\n');
	exit (1);
      }

    mpfr_clear (ex);
    mpfr_clear (ex1);
    mpfr_clear (ex2);
    mpfr_clear (ex3);
    mpfr_clear (tot);
    mpfr_clear (tot1);
}

/* if u = o(x-y), v = o(u-x), w = o(v+y), then x-y = u-w */
void
check_two_sum (mp_prec_t p)
{
  mpfr_t x, y, u, v, w;
  mp_rnd_t rnd;
  int inexact;
  
  mpfr_init2 (x, p);
  mpfr_init2 (y, p);
  mpfr_init2 (u, p);
  mpfr_init2 (v, p);
  mpfr_init2 (w, p);
  mpfr_random (x);
  mpfr_random (y);
  if (mpfr_cmp_abs (x, y) < 0)
    mpfr_swap (x, y);
  rnd = LONG_RAND() % 4;
  rnd = GMP_RNDN;
  inexact = mpfr_sub (u, x, y, GMP_RNDN);
  mpfr_sub (v, u, x, GMP_RNDN);
  mpfr_add (w, v, y, GMP_RNDN);
  /* as u = (x-y) - w, we should have inexact and w of opposite signs */
  if (((inexact == 0) && mpfr_cmp_ui (w, 0)) ||
      ((inexact > 0) && (mpfr_cmp_ui (w, 0) <= 0)) ||
      ((inexact < 0) && (mpfr_cmp_ui (w, 0) >= 0)))
    {
      fprintf (stderr, "Wrong inexact flag for prec=%u, rnd=%s\n", (unsigned)p,
	       mpfr_print_rnd_mode (rnd));
      printf ("x="); mpfr_print_binary(x); putchar('\n');
      printf ("y="); mpfr_print_binary(y); putchar('\n');
      printf ("u="); mpfr_print_binary(u); putchar('\n');
      printf ("v="); mpfr_print_binary(v); putchar('\n');
      printf ("w="); mpfr_print_binary(w); putchar('\n');
      printf ("inexact = %d\n", inexact);
      exit (1);
    }
  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (u);
  mpfr_clear (v);
  mpfr_clear (w);
}

#define MAX_PREC 100

void
check_inexact (void)
{
  mpfr_t x, y, z, u;
  mp_prec_t px, py, pu, pz;
  int inexact, cmp;
  mp_rnd_t rnd;
  
  mpfr_init (x);
  mpfr_init (y);
  mpfr_init (z);
  mpfr_init (u);

  for (px=2; px<MAX_PREC; px++)
    {
      mpfr_set_prec (x, px);
      mpfr_random (x);
      for (pu=2; pu<MAX_PREC; pu++)
	{
	  mpfr_set_prec (u, pu);
	  mpfr_random (u);
	  for (py=2; py<MAX_PREC; py++)
	    {
	      mpfr_set_prec (y, py);
	      pz =  (mpfr_cmp_abs (x, u) >= 0) ? MPFR_EXP(x)-MPFR_EXP(u)
		: MPFR_EXP(u)-MPFR_EXP(x);
	      pz = pz + MAX(MPFR_PREC(x), MPFR_PREC(u));
	      mpfr_set_prec (z, pz);
	      rnd = LONG_RAND () % 4;
	      if (mpfr_sub (z, x, u, rnd))
		{
		  fprintf (stderr, "z <- x - u should be exact\n");
		  exit (1);
		}
	      for (rnd=0; rnd<4; rnd++)
		{
		  inexact = mpfr_sub (y, x, u, rnd);
		  cmp = mpfr_cmp (y, z);
		  if (((inexact == 0) && (cmp != 0)) ||
		      ((inexact > 0) && (cmp <= 0)) ||
		      ((inexact < 0) && (cmp >= 0)))
		    {
		      fprintf (stderr, "Wrong inexact flag for rnd=%s\n",
			   mpfr_print_rnd_mode(rnd));
		      printf ("expected %d, got %d\n", cmp, inexact);
		      printf ("x="); mpfr_print_binary (x); putchar ('\n');
		      printf ("u="); mpfr_print_binary (u); putchar ('\n');
		      printf ("y=  "); mpfr_print_binary (y); putchar ('\n');
		      printf ("x-u="); mpfr_print_binary (z); putchar ('\n');
		      exit (1);
		    }
		}
	    }
	}
    }

  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (z);
  mpfr_clear (u);
}

int
main(void)
{
  mp_prec_t p;
  unsigned i;

  check_diverse ();
  check_inexact ();
  bug_ddefour ();

  for (p=2; p<200; p++)
    for (i=0; i<200; i++)
      check_two_sum (p);

  return 0;
}
