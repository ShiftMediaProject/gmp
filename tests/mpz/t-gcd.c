/* Test mpz_gcd, mpz_gcdext, mpz_mul, mpz_tdiv_r, mpz_add, mpz_cmp,
   mpz_cmp_ui, mpz_init_set, mpz_set, mpz_clear.

Copyright 1991, 1993, 1994, 1996, 1997, 2000, 2001, 2002 Free Software
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

void dump_abort _PROTO ((int, mpz_t, mpz_t));
void debug_mp _PROTO ((mpz_t, int));

int
main (int argc, char **argv)
{
  mpz_t op1, op2, x;
  mpz_t gcd, gcd2, s, t;
  mpz_t temp1, temp2;
  mp_size_t op1_size, op2_size, x_size;
  int i;
  int reps = 2000;
  gmp_randstate_ptr rands;
  mpz_t bs;
  unsigned long bsi, size_range;

  tests_start ();
  rands = RANDS;

  mpz_init (bs);

  if (argc == 2)
    reps = atoi (argv[1]);

  mpz_init (op1);
  mpz_init (op2);
  mpz_init (x);
  mpz_init (gcd);
  mpz_init (gcd2);
  mpz_init (temp1);
  mpz_init (temp2);
  mpz_init (s);
  mpz_init (t);

  for (i = 0; i < reps; i++)
    {
      mpz_urandomb (bs, rands, 32);
      size_range = mpz_get_ui (bs) % 12 + 2; /* 0..8191 bit operands */

      mpz_urandomb (bs, rands, size_range);
      op1_size = mpz_get_ui (bs);
      mpz_rrandomb (op1, rands, op1_size);

      mpz_urandomb (bs, rands, size_range);
      op2_size = mpz_get_ui (bs);
      mpz_rrandomb (op2, rands, op2_size);

      mpz_urandomb (bs, rands, size_range);
      x_size = mpz_get_ui (bs);
      mpz_rrandomb (x, rands, x_size);

      mpz_urandomb (bs, rands, 2);
      bsi = mpz_get_ui (bs);
      if ((bsi & 1) != 0)
	mpz_neg (op1, op1);
      if ((bsi & 2) != 0)
	mpz_neg (op2, op2);

      /* printf ("%ld %ld\n", SIZ (op1), SIZ (op2)); */

      mpz_mul (op1, op1, x);
      mpz_mul (op2, op2, x);

      mpz_gcd (gcd, op1, op2);
      /* We know GCD will be at least X, since we multiplied both operands
	 with it.  */
      if (mpz_cmp (gcd, x) < 0 && mpz_sgn (op1) != 0 && mpz_sgn (op2) != 0)
	dump_abort (i, op1, op2);

      if (mpz_fits_ulong_p (op2))
	{
	  mpz_gcd_ui (gcd2, op1, mpz_get_ui (op2));
	  if (mpz_cmp (gcd, gcd2))
	    dump_abort (i, op1, op2);
	}

      mpz_gcdext (gcd2, s, t, op1, op2);
      if (mpz_cmp (gcd, gcd2))
	dump_abort (i, op1, op2);

      mpz_gcdext (gcd2, s, NULL, op1, op2);
      if (mpz_cmp (gcd, gcd2))
	dump_abort (i, op1, op2);

      mpz_mul (temp1, s, op1);
      mpz_mul (temp2, t, op2);
      mpz_add (gcd2, temp1, temp2);
      if (mpz_cmp (gcd, gcd2))
	dump_abort (i, op1, op2);
    }

  mpz_clear (bs);
  mpz_clear (op1);
  mpz_clear (op2);
  mpz_clear (x);
  mpz_clear (gcd);
  mpz_clear (gcd2);
  mpz_clear (temp1);
  mpz_clear (temp2);
  mpz_clear (s);
  mpz_clear (t);

  tests_end ();
  exit (0);
}

void
dump_abort (int testnr, mpz_t op1, mpz_t op2)
{
  fprintf (stderr, "ERROR in test %d\n", testnr);
  fprintf (stderr, "op1 = "); debug_mp (op1, -16);
  fprintf (stderr, "op2 = "); debug_mp (op2, -16);
  abort();
}

void
debug_mp (mpz_t x, int base)
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
