/*

Copyright 2012, Free Software Foundation, Inc.

This file is part of the GNU MP Library test suite.

The GNU MP Library test suite is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

The GNU MP Library test suite is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along with
the GNU MP Library test suite.  If not, see http://www.gnu.org/licenses/.  */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "mini-random.h"

#define MAXBITS 400
#define COUNT 10000

static void
dump (const char *label, const mpz_t x)
{
  char *buf = mpz_get_str (NULL, 16, x);
  fprintf (stderr, "%s: %s\n", label, buf);
  free (buf);
}

int
main (int argc, char **argv)
{
  unsigned i;
  mpz_t a, b, res, ref;

  hex_random_init ();

  mpz_init (a);
  mpz_init (b);
  mpz_init (res);
  mpz_init (ref);

  for (i = 0; i < COUNT; i++)
    {
      mini_random_op (OP_AND, MAXBITS, a, b, ref);
      mpz_and (res, a, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_and failed:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}

      mini_random_op (OP_IOR, MAXBITS, a, b, ref);
      mpz_ior (res, a, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_ior failed:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}

      mini_random_op (OP_XOR, MAXBITS, a, b, ref);
      mpz_xor (res, a, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_xor failed:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}

      if (mpz_popcount (a) != mpz_hamdist (res, b))
	{
	  fprintf (stderr, "mpz_popcount(a) and mpz_hamdist(r,b) differ:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  fprintf (stderr, "mpz_popcount(a) = %lu:\n", mpz_popcount (a));
	  fprintf (stderr, "mpz_hamdist(r,b) = %lu:\n", mpz_hamdist (res, b));
	  abort ();
	}
    }
  mpz_clear (a);
  mpz_clear (b);
  mpz_clear (res);
  mpz_clear (ref);

  return 0;
}
