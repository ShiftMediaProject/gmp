/* mpz_millerrabin(n,reps) -- An implementation of the probabilistic primality
   test found in Knuth's Seminumerical Algorithms book.  If the function
   mpz_millerrabin() returns 0 then n is not prime.  If it returns 1, then n is
   'probably' prime.  The probability of a false positive is (1/4)**reps, where
   reps is the number of internal passes of the probabilistic algorithm.  Knuth
   indicates that 25 passes are reasonable.

   THE FUNCTIONS IN THIS FILE ARE FOR INTERNAL USE ONLY.  THEY'RE ALMOST
   CERTAIN TO BE SUBJECT TO INCOMPATIBLE CHANGES OR DISAPPEAR COMPLETELY IN
   FUTURE GNU MP RELEASES.

Copyright 1991, 1993, 1994, 1996, 1997, 1998, 1999, 2000, 2001 Free Software
Foundation, Inc.  Contributed by John Amanatides.

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

#include "gmp.h"
#include "gmp-impl.h"

static int millerrabin _PROTO ((mpz_srcptr n, mpz_srcptr nm1,
                                mpz_ptr x, mpz_ptr y,
                                mpz_srcptr q, unsigned long int k));

int
mpz_millerrabin (mpz_srcptr n, int reps)
{
  int r;
  mpz_t nm1, x, y, q;
  unsigned long int k;
  gmp_randstate_t rstate;
  int is_prime;
  TMP_DECL (marker);
  TMP_MARK (marker);

  MPZ_TMP_INIT (nm1, SIZ (n) + 1);
  mpz_sub_ui (nm1, n, 1L);

  MPZ_TMP_INIT (x, SIZ (n));
  MPZ_TMP_INIT (y, 2 * SIZ (n)); /* mpz_powm_ui needs excessive memory!!! */

  /* Perform a Fermat test.  */
  mpz_set_ui (x, 210L);
  mpz_powm (y, x, nm1, n);
  if (mpz_cmp_ui (y, 1L) != 0)
    {
      TMP_FREE (marker);
      return 0;
    }

  MPZ_TMP_INIT (q, SIZ (n));

  /* Find q and k, where q is odd and n = 1 + 2**k * q.  */
  k = mpz_scan1 (nm1, 0L);
  mpz_tdiv_q_2exp (q, nm1, k);

  gmp_randinit (rstate, GMP_RAND_ALG_DEFAULT, 32L);

  is_prime = 1;
  for (r = 0; r < reps && is_prime; r++)
    {
      do
	mpz_urandomb (x, rstate, mpz_sizeinbase (n, 2) - 1);
      while (mpz_cmp_ui (x, 1L) <= 0);

      is_prime = millerrabin (n, nm1, x, y, q, k);
    }

  gmp_randclear (rstate);

  TMP_FREE (marker);
  return is_prime;
}

static int
millerrabin (mpz_srcptr n, mpz_srcptr nm1, mpz_ptr x, mpz_ptr y,
             mpz_srcptr q, unsigned long int k)
{
  unsigned long int i;

  mpz_powm (y, x, q, n);

  if (mpz_cmp_ui (y, 1L) == 0 || mpz_cmp (y, nm1) == 0)
    return 1;

  for (i = 1; i < k; i++)
    {
      mpz_powm_ui (y, y, 2L, n);
      if (mpz_cmp (y, nm1) == 0)
	return 1;
      if (mpz_cmp_ui (y, 1L) == 0)
	return 0;
    }
  return 0;
}
