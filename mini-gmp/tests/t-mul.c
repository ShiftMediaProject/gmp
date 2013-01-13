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
  mpz_t a, b, res, res_ui, ref;

  hex_random_init ();

  mpz_init (a);
  mpz_init (b);
  mpz_init (res);
  mpz_init (res_ui);
  mpz_init (ref);

  for (i = 0; i < COUNT; i++)
    {
      mini_random_op (OP_MUL, MAXBITS, a, b, ref);
      mpz_mul (res, a, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "mpz_mul failed:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}
      if (mpz_fits_slong_p (b)) {
	mpz_mul_si (res_ui, a, mpz_get_si (b));
	if (mpz_cmp (res_ui, ref))
	  {
	    fprintf (stderr, "mpz_mul_si failed:\n");
	    dump ("a", a);
	    dump ("b", b);
	    dump ("r", res_ui);
	    dump ("ref", ref);
	    abort ();
	  }
      }
    }
  mpz_clear (a);
  mpz_clear (b);
  mpz_clear (res);
  mpz_clear (res_ui);
  mpz_clear (ref);

  return 0;
}
