/* Test mpz_perfect_square_p and mpz_perfect_power_p

Copyright 2000, 2001 Free Software Foundation, Inc.

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

void debug_mp _PROTO ((mpz_t, int));

int
main (int argc, char **argv)
{
  mpz_t x2, x2t, x;
  mp_size_t x2n;
  int res;
  int i;
  /* int cnt = 0; */
  int reps = 100000;
  gmp_randstate_ptr rands;
  mpz_t bs;

  tests_start ();
  rands = RANDS;

  mpz_init (bs);

  if (argc == 2)
     reps = atoi (argv[1]);

  mpz_init (x2);
  mpz_init (x);
  mpz_init (x2t);

  for (i = 0; i < reps; i++)
    {
      mpz_urandomb (bs, rands, 9);
      x2n = mpz_get_ui (bs);
      mpz_rrandomb (x2, rands, x2n);
      /* mpz_out_str (stdout, -16, x2); puts (""); */

      res = mpz_perfect_square_p (x2);
      mpz_sqrt (x, x2);
      mpz_mul (x2t, x, x);

      if (res != (mpz_cmp (x2, x2t) == 0))
	abort ();

      /* cnt += res != 0; */
    }
  /* printf ("%d/%d perfect squares\n", cnt, reps); */

  mpz_clear (bs);
  mpz_clear (x2);
  mpz_clear (x);
  mpz_clear (x2t);

  tests_end ();
  exit (0);
}

void
debug_mp (mpz_t x, int base)
{
  mpz_out_str (stderr, base, x); fputc ('\n', stderr);
}
