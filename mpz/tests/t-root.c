/* Test mpz_add, mpz_add_ui, mpz_cmp, mpz_cmp, mpz_mul, mpz_sqrtrem.

Copyright (C) 1991, 1993, 1994, 1996, 2000 Free Software Foundation, Inc.

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
#include "gmp.h"
#include "gmp-impl.h"
#include "urandom.h"

void debug_mp ();

#ifndef SIZE
#define SIZE 100
#endif

main (argc, argv)
     int argc;
     char **argv;
{
  mpz_t x2;
  mpz_t x;
  mpz_t temp, temp2;
  mp_size_t x2_size;
  int i;
  int reps = 10000;
  unsigned long nth;

  if (argc == 2)
     reps = atoi (argv[1]);

  mpz_init (x2);
  mpz_init (x);
  mpz_init (temp);
  mpz_init (temp2);

  for (i = 0; i < reps; i++)
    {
      x2_size = urandom () % SIZE + 1;
      mpz_random2 (x2, x2_size);
      nth = urandom () % mpz_sizeinbase (x2, 2) + 1;
      if ((urandom() & 1) == 0)
	{
	  /* With 50% probability, set x2 just below a perfect power.  */
	  mpz_root (x, x2, nth);
	  mpz_pow_ui (x2, x, nth);
	  if (mpz_sgn (x2) != 0)
	    mpz_sub_ui (x2, x2, 1L);
	}

      mpz_root (x, x2, nth);
      mpz_pow_ui (temp, x, nth);

      /* Is power of result > argument?  */
      if (mpz_cmp (temp, x2) > 0)
	{
	  fprintf (stderr, "ERROR after test %d\n", i);
	  debug_mp (x2, 10);
	  debug_mp (x, 10);
	  fprintf (stderr, "nth: %lu\n", nth);
	  abort ();
	}

      if (nth > 1 && mpz_cmp_ui (temp, 1L) > 0 && ! mpz_perfect_power_p (temp))
	{
	  fprintf (stderr, "ERROR in mpz_perfect_power_p after test %d\n", i);
	  debug_mp (temp, 10);
	  debug_mp (x, 10);
	  fprintf (stderr, "nth: %lu\n", nth);
	  abort ();
	}

      if (nth > 10000)
	continue;		/* skip too expensive test */

      mpz_add_ui (temp2, x, 1L);
      mpz_pow_ui (temp2, temp2, nth);

      /* Is square of (result + 1) <= argument?  */
      if (mpz_cmp (temp2, x2) <= 0)
	{
	  fprintf (stderr, "ERROR after test %d\n", i);
	  debug_mp (x2, 10);
	  debug_mp (x, 10);
	  fprintf (stderr, "nth: %lu\n", nth);
	  abort ();
	}
    }

  exit (0);
}

void
debug_mp (x, base)
     mpz_t x;
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
