/* Test mpq_get_d and mpq_set_d

Copyright 1991, 1993, 1994, 1996, 2000, 2001, 2002, 2003 Free Software
Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"

#ifndef SIZE
#define SIZE 8
#endif

/* VAX D floats only have an 8 bit signed exponent, so anything 2^128 or
   bigger will overflow, that being 4 limbs. */
#if defined (__vax__) && SIZE > 4
#undef SIZE
#define SIZE 4
#define EPSIZE 3
#else
#define EPSIZE SIZE
#endif

void dump _PROTO ((mpq_t));

void
check_monotonic (int argc, char **argv)
{
  mpq_t a;
  mp_size_t size;
  int reps = 100;
  int i, j;
  double last_d, new_d;
  mpq_t qlast_d, qnew_d;
  mpq_t eps;

  if (argc == 2)
     reps = atoi (argv[1]);

  /* The idea here is to test the monotonousness of mpq_get_d by adding
     numbers to the numerator and denominator.  */

  mpq_init (a);
  mpq_init (eps);
  mpq_init (qlast_d);
  mpq_init (qnew_d);

  for (i = 0; i < reps; i++)
    {
      size = urandom () % SIZE - SIZE/2;
      mpz_random2 (mpq_numref (a), size);
      do
	{
	  size = urandom () % SIZE - SIZE/2;
	  mpz_random2 (mpq_denref (a), size);
	}
      while (mpz_cmp_ui (mpq_denref (a), 0) == 0);

      mpq_canonicalize (a);

      last_d = mpq_get_d (a);
      mpq_set_d (qlast_d, last_d);
      for (j = 0; j < 10; j++)
	{
	  size = urandom () % EPSIZE + 1;
	  mpz_random2 (mpq_numref (eps), size);
	  size = urandom () % EPSIZE + 1;
	  mpz_random2 (mpq_denref (eps), size);
	  mpq_canonicalize (eps);

	  mpq_add (a, a, eps);
	  mpq_canonicalize (a);
	  new_d = mpq_get_d (a);
	  if (last_d > new_d)
	    {
	      fprintf (stderr, "\nERROR (test %d/%d): bad mpq_get_d results\n", i, j);
	      printf ("last: %.16g\n", last_d);
	      printf (" new: %.16g\n", new_d); dump (a);
	      abort ();
	    }
	  mpq_set_d (qnew_d, new_d);
	  MPQ_CHECK_FORMAT (qnew_d);
	  if (mpq_cmp (qlast_d, qnew_d) > 0)
	    {
	      fprintf (stderr,
		       "ERROR (test %d/%d): bad mpq_set_d results\n", i, j);
	      printf ("last: %.16g\n", last_d); dump (qlast_d);
	      printf (" new: %.16g\n", new_d); dump (qnew_d);
	      abort ();
	    }
	  last_d = new_d;
	  mpq_set (qlast_d, qnew_d);
	}
    }

  mpq_clear (a);
  mpq_clear (eps);
  mpq_clear (qlast_d);
  mpq_clear (qnew_d);
}

void
dump (mpq_t x)
{
  mpz_out_str (stdout, 10, mpq_numref (x));
  printf ("/");
  mpz_out_str (stdout, 10, mpq_denref (x));
  printf ("\n");
}

/* Check various values 2^n and 1/2^n. */
void
check_onebit (void)
{
  static const long data[] = {
    -3*GMP_NUMB_BITS-1, -3*GMP_NUMB_BITS, -3*GMP_NUMB_BITS+1,
    -2*GMP_NUMB_BITS-1, -2*GMP_NUMB_BITS, -2*GMP_NUMB_BITS+1,
    -GMP_NUMB_BITS-1, -GMP_NUMB_BITS, -GMP_NUMB_BITS+1,
    -5, -2, -1, 0, 1, 2, 5,
    GMP_NUMB_BITS-1, GMP_NUMB_BITS, GMP_NUMB_BITS+1,
    2*GMP_NUMB_BITS-1, 2*GMP_NUMB_BITS, 2*GMP_NUMB_BITS+1,
    3*GMP_NUMB_BITS-1, 3*GMP_NUMB_BITS, 3*GMP_NUMB_BITS+1,
  };

  int     i, neg;
  long    exp, l;
  mpq_t   q;
  double  got, want;
  /* FIXME: It'd be better to base this on the float format. */
#ifdef __vax
  int     limit = 127;  /* vax fp numbers have limited range */
#else
  int     limit = 512;
#endif

  mpq_init (q);

  for (i = 0; i < numberof (data); i++)
    {
      exp = data[i];

      mpq_set_ui (q, 1L, 1L);
      if (exp >= 0)
        mpq_mul_2exp (q, q, exp);
      else
        mpq_div_2exp (q, q, -exp);

      want = 1.0;
      for (l = 0; l < exp; l++)
        want *= 2.0;
      for (l = 0; l > exp; l--)
        want /= 2.0;

      for (neg = 0; neg <= 1; neg++)
        {
          if (neg)
            {
              mpq_neg (q, q);
              want = -want;
            }

          got = mpq_get_d (q);

          if (got != want)
            {
              printf    ("mpq_get_d wrong on %s2**%d\n", i, neg ? "-" : "");
              mpq_trace ("   q    ", q);
              d_trace   ("   want ", want);
              d_trace   ("   got  ", got);
              abort();
            }
        }
    }
  mpq_clear (q);
}

int
main (int argc, char **argv)
{
  tests_start ();

  check_onebit ();
  check_monotonic (argc, argv);

  tests_end ();
  exit (0);
}
