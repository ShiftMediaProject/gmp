/* mpz_probab_prime_p --
   An implementation of the probabilistic primality test found in Knuth's
   Seminumerical Algorithms book.  If the function mpz_probab_prime_p()
   returns 0 then n is not prime.  If it returns 1, then n is 'probably'
   prime.  If it returns 2, n is surely prime.  The probability of a false
   positive is (1/4)**reps, where reps is the number of internal passes of the
   probabilistic algorithm.  Knuth indicates that 25 passes are reasonable.

Copyright 1991, 1993, 1994, 1996, 1997, 1998, 1999, 2000, 2001 Free Software
Foundation, Inc.  Miller-Rabin code contributed by John Amanatides.

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
#include "longlong.h"

static int isprime _PROTO ((unsigned long int t));

int
mpz_probab_prime_p (mpz_srcptr n, int reps)
{
  mp_limb_t r;

  /* Handle small and negative n.  */
  if (mpz_cmp_ui (n, 1000000L) <= 0)
    {
      int is_prime;
      if (mpz_sgn (n) < 0)
	{
	  /* Negative number.  Negate and call ourselves.  */
	  mpz_t n2;
	  mpz_init (n2);
	  mpz_neg (n2, n);
	  is_prime = mpz_probab_prime_p (n2, reps);
	  mpz_clear (n2);
	  return is_prime;
	}
      is_prime = isprime (mpz_get_ui (n));
      return is_prime ? 2 : 0;
    }

  /* If n is now even, it is not a prime.  */
  if ((mpz_get_ui (n) & 1) == 0)
    return 0;

#if defined (PP)
  /* Check if n has small factors.  */
#if defined (PP_INVERTED)
  r = MPN_MOD_OR_PREINV_MOD_1 (PTR(n), SIZ(n), (mp_limb_t) PP,
                               (mp_limb_t) PP_INVERTED);
#else
  r = mpn_mod_1 (PTR(n), SIZ(n), (mp_limb_t) PP);
#endif
  if (r % 3 == 0
#if BITS_PER_MP_LIMB >= 4
      || r % 5 == 0
#endif
#if BITS_PER_MP_LIMB >= 8
      || r % 7 == 0
#endif
#if BITS_PER_MP_LIMB >= 16
      || r % 11 == 0 || r % 13 == 0
#endif
#if BITS_PER_MP_LIMB >= 32
      || r % 17 == 0 || r % 19 == 0 || r % 23 == 0 || r % 29 == 0
#endif
#if BITS_PER_MP_LIMB >= 64
      || r % 31 == 0 || r % 37 == 0 || r % 41 == 0 || r % 43 == 0
      || r % 47 == 0 || r % 53 == 0
#endif
      )
    {
      return 0;
    }
#endif /* PP */

  /* Do more dividing.  We collect small primes, using umul_ppmm, until we
     overflow a single limb.  We divide our number by the small primes product,
     and look for factors in the remainder.  */
  {
    unsigned long int ln2;
    unsigned long int q;
    mp_limb_t p1, p0, p;
    unsigned int primes[15];
    int nprimes;

    nprimes = 0;
    p = 1;
    ln2 = mpz_sizeinbase (n, 2) / 30; ln2 = ln2 * ln2;
    for (q = PP_FIRST_OMITTED; q < ln2; q += 2)
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
		      ASSERT_ALWAYS (mpn_mod_1 (PTR(n), SIZ(n), (mp_limb_t) primes[nprimes]) == 0);
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

  /* Perform a number of Miller-Rabin tests.  */
  return mpz_millerrabin (n, reps);
}

static int
isprime (unsigned long int t)
{
  unsigned long int q, r, d;

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
