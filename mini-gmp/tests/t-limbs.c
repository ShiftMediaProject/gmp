/*

Copyright 2012, 2014, Free Software Foundation, Inc.

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
the GNU MP Library test suite.  If not, see https://www.gnu.org/licenses/.  */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "testutils.h"

#define MAXBITS 400
#define COUNT 100

static void
dump (const char *label, const mpz_t x)
{
  char *buf = mpz_get_str (NULL, 16, x);
  fprintf (stderr, "%s: %s\n", label, buf);
  testfree (buf);
}

void
my_mpz_mul (mpz_t r, mpz_srcptr a,  mpz_srcptr b)
{
  mp_limb_t *tp;
  mp_size_t tn, an, bn;

  an = mpz_size (a);
  bn = mpz_size (b);
  tn = an + bn;

  tp = mpz_limbs_write (r, tn);
  if (mpz_sgn (a) * mpz_sgn(b) == 0)
    mpn_zero (tp, tn);
  else if (an > bn)
    mpn_mul (tp, mpz_limbs_read (a), an, mpz_limbs_read (b), bn);
  else
    mpn_mul (tp, mpz_limbs_read (b), bn, mpz_limbs_read (a), an);

  if (mpz_sgn (a) != mpz_sgn(b))
    tn = - tn;

  mpz_limbs_finish (r, tn);
}

void
testmain (int argc, char **argv)
{
  unsigned i;
  mpz_t a, b, t, res, ref;

  mpz_init (a);
  mpz_init (b);
  mpz_init (res);
  mpz_init (ref);

  for (i = 0; i < COUNT; i++)
    {
      mini_random_op3 (OP_MUL, MAXBITS, a, b, ref);
      my_mpz_mul (res, a, b);
      if (mpz_cmp (res, ref))
	{
	  fprintf (stderr, "my_mpz_mul failed:\n");
	  dump ("a", a);
	  dump ("b", b);
	  dump ("r", res);
	  dump ("ref", ref);
	  abort ();
	}
    }
  mpz_clear (a);
  mpz_clear (b);
  mpz_clear (res);
  mpz_clear (ref);
}
