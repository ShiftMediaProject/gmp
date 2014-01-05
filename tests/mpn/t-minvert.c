/* Copyright 2013, 2014 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>		/* for strtol */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "tests/tests.h"

#define MAX_SIZE 50

#define COUNT 200

static void
mpz_to_mpn (mp_ptr ap, mp_size_t an, const mpz_t b)
{
  mp_size_t bn = mpz_size (b);
  ASSERT_ALWAYS (bn <= an);
  MPN_COPY_INCR (ap, mpz_limbs_read (b), bn);
  MPN_ZERO (ap + bn, an - bn);
}

int
mpz_eq_mpn (mp_ptr ap, mp_size_t an, const mpz_t b)
{
  mp_size_t bn = mpz_size (b);

  return (bn >= 0 && bn <= an
	  && mpn_cmp (ap, mpz_limbs_read (b), bn) == 0
	  && mpn_zero_p (ap + bn, an - bn));
}

int
main (int argc, char **argv)
{
  gmp_randstate_ptr rands;
  long count = COUNT;
  mp_ptr mp;
  mp_ptr ap;
  mp_ptr vp;
  mp_ptr tp;
  mp_ptr scratch;
  mpz_t m, a, r, g;
  int test;
  tests_start ();
  rands = RANDS;

  TMP_DECL;

  TMP_MARK;
  mpz_init (m);
  mpz_init (a);
  mpz_init (r);
  mpz_init (g);

  if (argc > 1)
    {
      char *end;
      count = strtol (argv[1], &end, 0);
      if (*end || count <= 0)
	{
	  fprintf (stderr, "Invalid test count: %s.\n", argv[1]);
	  return 1;
	}
    }

  mp = TMP_ALLOC_LIMBS (MAX_SIZE);
  ap = TMP_ALLOC_LIMBS (MAX_SIZE);
  vp = TMP_ALLOC_LIMBS (MAX_SIZE);
  tp = TMP_ALLOC_LIMBS (MAX_SIZE);
  scratch = TMP_ALLOC_LIMBS (mpn_sec_minvert_itch (MAX_SIZE));

  for (test = 0; test < count; test++)
    {
      mp_bitcnt_t bits;
      int rres, tres;
      mp_size_t n;

      bits = urandom () % (GMP_NUMB_BITS * MAX_SIZE) + 1;

      if (test & 1)
	mpz_rrandomb (m, rands, bits);
      else
	mpz_urandomb (m, rands, bits);
      if (test & 2)
	mpz_rrandomb (a, rands, bits);
      else
	mpz_urandomb (a, rands, bits);

      mpz_setbit (m, 0);
      if (test & 4)
	{
	  /* Ensure it really is invertible */
	  if (mpz_sgn (a) == 0)
	    mpz_set_ui (a, 1);
	  else
	    for (;;)
	      {
		mpz_gcd (g, a, m);
		if (mpz_cmp_ui (g, 1) == 0)
		  break;
		mpz_remove (a, a, g);
	      }
	}

      rres = mpz_invert (r, a, m);
      if ( (test & 4) && !rres)
	{
	  gmp_fprintf (stderr, "test %d: Not invertible!\n"
		       "m = %Zd\n"
		       "a = %Zd\n", test, m, a);
	  abort ();
	}
      ASSERT_ALWAYS (! (test & 4) || rres);

      n = (bits + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS;
      ASSERT_ALWAYS (n <= MAX_SIZE);

      mpz_to_mpn (ap, n, a);
      mpz_to_mpn (mp, n, m);
      tres = mpn_sec_minvert (tp,
			      ap, mp, n, 2*bits, scratch);

      if (rres != tres || (rres == 1 && !mpz_eq_mpn (tp, n, r)))
	{
	  gmp_fprintf (stderr, "Test %d failed.\n"
		       "m = %Zd\n"
		       "a = %Zd\n", test, m, a);
	  fprintf (stderr, "ref ret: %d\n"
		  "got ret: %d\n", rres, tres);
	  if (rres)
	    gmp_fprintf (stderr, "ref: %Zd\n", r);
	  if (tres)
	    gmp_fprintf (stderr, "got: %Nd\n", tp, n);
	  abort ();
	}
    }

  TMP_FREE;

  mpz_clear (m);
  mpz_clear (a);
  mpz_clear (r);
  mpz_clear (g);

  tests_end ();
  return 0;
}
