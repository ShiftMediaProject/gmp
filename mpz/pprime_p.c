/* mpz_probab_prime_p --
   An implementation of the probabilistic primality test found in Knuth's
   Seminumerical Algorithms book.  If the function mpz_probab_prime_p()
   returns 0 then n is not prime.  If it returns 1, then n is 'probably'
   prime.  If it returns 2, n is surely prime.  The probability of a false
   positive is (1/4)**reps, where reps is the number of internal passes of the
   probabilistic algorithm.  Knuth indicates that 25 passes are reasonable.

Copyright (C) 1991, 1993, 1994, 1996, 1997, 1998, 1999, 2000 Free Software
Foundation, Inc.  Contributed by John Amanatides.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

static int isprime ();
static int millerrabin ();

int
mpz_probab_prime_p (m, reps)
     mpz_srcptr m;
     int reps;
{
  mpz_t n, tmp, two, n_minus_1, x, y, q;
  mp_limb_t r;
  int i, is_prime;
  unsigned long int k;

  mpz_init (n);
  /* Take the absolute value of M, to handle positive and negative primes.  */
  mpz_abs (n, m);

  /* Handle small n.  */
  if (mpz_cmp_ui (n, 1000000L) <= 0)
    {
      is_prime = isprime (mpz_get_ui (n));
      mpz_clear (n);
      return is_prime ? 2 : 0;
    }

  /* Return if n is now even.  */
  if ((mpz_get_ui (n) & 1) == 0)
    {
      mpz_clear (n);
      return 0;
    }

  /* Check if n has small factors.  */
  if (UDIV_TIME > (2 * UMUL_TIME + 6))
    r = mpn_preinv_mod_1 (PTR(n), SIZ(n), (mp_limb_t) PP, (mp_limb_t) PP_INVERTED);
  else
    r = mpn_mod_1 (PTR(n), SIZ(n), (mp_limb_t) PP);
  if (r % 3 == 0 || r % 5 == 0 || r % 7 == 0 || r % 11 == 0 || r % 13 == 0
      || r % 17 == 0 || r % 19 == 0 || r % 23 == 0 || r % 29 == 0
#if BITS_PER_MP_LIMB == 64
      || r % 31 == 0 || r % 37 == 0 || r % 41 == 0 || r % 43 == 0
      || r % 47 == 0 || r % 53 == 0
#endif
      )
    {
      mpz_clear (n);
      return 0;
    }

#if 1
  /* Do more dividing.  */
  {
    unsigned long int ln2;
    unsigned long int q;
    mp_limb_t p1, p0, p;
    unsigned int primes[15];
    int nprimes;

    nprimes = 0;
    p = 1;
    ln2 = SIZ(n)*BITS_PER_MP_LIMB/30; ln2 = ln2 * ln2;
    for (q = BITS_PER_MP_LIMB == 64 ? 59 : 31; q < ln2; q += 2)
      {
	if (isprime (q))
	  {
	    umul_ppmm (p1, p0, p, q);
	    if (p1 != 0)
	      {
		r = mpn_mod_1 (PTR(n), SIZ(n), p);
		while (--nprimes >= 0)
		  if (r % primes[nprimes] == 0)
		    {
		      if (mpn_mod_1 (PTR(n), SIZ(n), (mp_limb_t) primes[nprimes]) != 0)
			abort ();
		      mpz_clear (n);
		      return 0;
		    }
		p = q;
		nprimes = 0;
	      }
	    else
	      {
		p = p0;
	      }
	    primes[nprimes++] = q;
	  }
      }
  }
#endif

  mpz_init (tmp);
  mpz_init (n_minus_1);
  mpz_sub_ui (n_minus_1, n, 1L);

  /* Perform a Fermat test.  */
  mpz_init_set_ui (two, 210L);
  mpz_powm (tmp, two, n_minus_1, n);
  mpz_clear (two);
  if (mpz_cmp_ui (tmp, 1L) != 0)
    {
      mpz_clear (n_minus_1);
      mpz_clear (tmp);
      mpz_clear (n);
      return 0;
    }

  /* Finally perform a number of Miller-Rabin tests.  */
     
  mpz_init (x);
  mpz_init (y);
  mpz_init (q);

  /* Find q and k, where q is odd and n = 1 + 2**k * q.  */
  k = mpz_scan1 (n_minus_1, 0L);
  mpz_tdiv_q_2exp (q, n_minus_1, k);

  is_prime = 1;
  for (i = 0; i < reps && is_prime; i++)
    is_prime &= millerrabin (n, n_minus_1, x, y, q, k);

  mpz_clear (n_minus_1);
  mpz_clear (n);
  mpz_clear (x);
  mpz_clear (y);
  mpz_clear (q);
  return is_prime;
}

static int
isprime (t)
     unsigned long int t;
{
  unsigned int q, r, d;

  if (t < 3 || (t & 1) == 0)
    return t == 2;

  for (d = 3, r = 1; r != 0; d += 2)
    {
      q = t / d;
      r = t - q * d;
      if (q < d)
	return 1;
    }
  return 0;
}

static int
millerrabin (n, n_minus_1, x, y, q, k)
     mpz_srcptr n;
     mpz_srcptr n_minus_1;
     mpz_ptr x;
     mpz_ptr y;
     mpz_srcptr q;
     unsigned long int k;
{
  unsigned long int i;
  gmp_randstate_t rstate;
  unsigned long int nb;

  nb = mpz_sizeinbase (n, 2);
  gmp_randinit (rstate, GMP_RAND_ALG_DEFAULT, 32L);

  /* find random x s.t. 1 < x < n */
  do
    {
      mpz_urandomb (x, rstate, nb);
      mpz_mod (x, x, n);
    }
  while (mpz_cmp_ui (x, 1L) <= 0);

  mpz_powm (y, x, q, n);

  if (mpz_cmp_ui (y, 1L) == 0 || mpz_cmp (y, n_minus_1) == 0)
    {
      gmp_randclear (rstate);
      return 1;
    }

  for (i = 1; i < k; i++)
    {
      mpz_powm_ui (y, y, 2L, n);
      if (mpz_cmp (y, n_minus_1) == 0)
	{
	  gmp_randclear (rstate);
	  return 1;
	}
      if (mpz_cmp_ui (y, 1L) == 0)
	{
	  gmp_randclear (rstate);
	  return 0;
	}
    }
  gmp_randclear (rstate);
  return 0;
}
