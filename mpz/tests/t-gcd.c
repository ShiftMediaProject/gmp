/* Test mpz_gcd, mpz_gcdext, mpz_mul, mpz_tdiv_r, mpz_add, mpz_cmp,
   mpz_cmp_ui, mpz_init_set, mpz_set, mpz_clear.

Copyright (C) 1991, 1993, 1994, 1996, 1997 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "urandom.h"

void debug_mp ();

#ifndef SIZE
#define SIZE 128
#endif

main (argc, argv)
     int argc;
     char **argv;
{
  mpz_t op1, op2, x;
  mpz_t gcd, gcd2, s, t;
  mpz_t temp1, temp2;
  mp_size_t op1_size, op2_size, x_size;
  int i;
  int reps = 2000;

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
      op1_size = urandom () % SIZE - SIZE/2;
      op2_size = urandom () % SIZE - SIZE/2;
      x_size = urandom () % SIZE/2;

      mpz_random2 (op1, op1_size);
      mpz_random2 (op2, op2_size);
      mpz_random2 (x, x_size);
      mpz_mul (op1, op1, x);
      mpz_mul (op2, op2, x);

      mpz_gcd (gcd, op1, op2);
      /* We know GCD will be at least X, since we multiplied in that factor.  */
      if (mpz_cmp (gcd, x) < 0 && mpz_sgn (op1) != 0 && mpz_sgn (op2) != 0)
	dump_abort (op1, op2);

      mpz_gcdext (gcd2, s, t, op1, op2);
      if (mpz_cmp (gcd, gcd2))
	dump_abort (op1, op2);

      mpz_mul (temp1, s, op1);
      mpz_mul (temp2, t, op2);
      mpz_add (gcd2, temp1, temp2);
      if (mpz_cmp (gcd, gcd2))
	dump_abort (op1, op2);
    }

  exit (0);
}

dump_abort (op1, op2)
     mpz_t op1, op2;
{
  fprintf (stderr, "ERROR\n");
  fprintf (stderr, "op1 = "); debug_mp (op1, -16);
  fprintf (stderr, "op2 = "); debug_mp (op2, -16);
  abort();
}

void
debug_mp (x, base)
     mpz_t x;
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
