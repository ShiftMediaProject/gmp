/* Test mpz_add, mpz_add_ui, mpz_cmp, mpz_cmp, mpz_mul, mpz_sqrtrem.

Copyright 1991, 1993, 1994, 1996, 2000, 2001 Free Software Foundation, Inc.

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
#include <sys/time.h>

#include "gmp.h"
#include "gmp-impl.h"

void debug_mp ();

main (int argc, char **argv)
{
  mpz_t x2;
  mpz_t x, rem;
  mpz_t temp, temp2;
  mp_size_t x2_size;
  int i;
  int reps = 20000;
  gmp_randstate_t rands;
  mpz_t bs;
  unsigned long bsi, size_range;
  char *perform_seed;

  gmp_randinit (rands, GMP_RAND_ALG_LC, 64);

  perform_seed = getenv ("GMP_CHECK_RANDOMIZE");
  if (perform_seed != 0)
    {
      struct timeval tv;
      gettimeofday (&tv, NULL);
      gmp_randseed_ui (rands, tv.tv_sec + tv.tv_usec);
      printf ("PLEASE INCLUDE THIS SEED NUMBER IN ALL BUG REPORTS:\n");
      printf ("GMP_CHECK_RANDOMIZE is set--seeding with %ld\n",
	      tv.tv_sec + tv.tv_usec);
    }

  mpz_init (bs);

  if (argc == 2)
     gmp_randseed_ui (rands, atoi (argv[1]));

  mpz_init (x2);
  mpz_init (x);
  mpz_init (rem);
  mpz_init (temp);
  mpz_init (temp2);

  for (i = 0; i < reps; i++)
    {
      mpz_urandomb (bs, rands, 32);
      size_range = mpz_get_ui (bs) % 12 + 2; /* 0..8191 bit operands */

      mpz_urandomb (bs, rands, size_range);
      x2_size = mpz_get_ui (bs);
      mpz_rrandomb (x2, rands, x2_size);

      /* printf ("%ld\n", SIZ (x2)); */

      mpz_sqrtrem (x, rem, x2);
      mpz_mul (temp, x, x);

      /* Is square of result > argument?  */
      if (mpz_cmp (temp, x2) > 0)
	dump_abort (x2, x, rem);

      mpz_add_ui (temp2, x, 1);
      mpz_mul (temp2, temp2, temp2);

      /* Is square of (result + 1) <= argument?  */
      if (mpz_cmp (temp2, x2) <= 0)
	dump_abort (x2, x, rem);

      mpz_add (temp2, temp, rem);

      /* Is the remainder wrong?  */
      if (mpz_cmp (x2, temp2) != 0)
	dump_abort (x2, x, rem);
    }

  exit (0);
}

dump_abort (mpz_t x2, mpz_t x, mpz_t rem)
{
  fprintf (stderr, "ERROR\n");
  fprintf (stderr, "x2        = "); debug_mp (x2, -16);
  fprintf (stderr, "x         = "); debug_mp (x, -16);
  fprintf (stderr, "remainder = "); debug_mp (rem, -16);
  abort();
}

void
debug_mp (mpz_t x, int base)
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
