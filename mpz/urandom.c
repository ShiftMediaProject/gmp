/* urandomb(rop, state) -- Generate a uniform pseudorandom integer in
the range 0 to 2^n - 1, inclusive, using `state' as the random state
previously initialized by a call to gmp_rand_init().  The `n' in the
above range is passed to gmp_rand_init().

Copyright (C) 1999 Free Software Foundation, Inc.

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

/* For linear congruental (LC), we use one of algorithms (1) or (2).

LC algorithm (1).

	X = (aX + c) mod m

[D. Knuth, "The Art of Computer Programming: Volume 2, Seminumerical Algorithms",
Third Edition, Addison Wesley, 1998, pp. 184-185.]

	X is the seed and the result
	a is chosen so that
	    a mod 8 = 5 [3.2.1.2] and [3.2.1.3] 
	    .01m < a < .99m
	    its binary or decimal digits is not a simple, regular pattern
	    it has no large quotients when Euclid's algorithm is used to find
	      gcd(a, m) [3.3.3]
	    it passes the spectral test [3.3.4]
	    it passes several tests of [3.3.2]
	c has no factor in common with m (c=1 or c=a can be good)
	m is large (2^30)
	  is a power of 2 [3.2.1.1]
	
The least significant digits of the generated number are not very
random.  It should be regarded as a random fraction X/m.  To get a
random integer between 0 and n-1, multiply X/m by n and truncate.
(Don't use X/n [ex 3.4.1-3])

The ``accuracy'' in t dimensions is one part in ``the t'th root of m'' [3.3.4].

Don't generate more than about m/1000 numbers without changing a, c, or m.

The sequence length depends on chosen a,c,m.


LC algorithm (2).

	X = a * (X mod q) - r * (long) (X/q)
	if X<0 then X+=m

[Knuth, pp. 185-186.]

	X is the seed and the result
	  as a seed is nonzero and less than m
	a is a primitive root of m (which means that a^2 <= m)
	q is (long) m / a
	r is m mod a
	m is a prime number near the largest easily computed integer

which gives

	X = a * (X % ((long) m / a)) -
	    (M % a) * ((long) (X / ((long) m / a)))

Since m is prime, the least-significant bits of X are just as random as 
the most-significant bits. */

/* Blum, Blum, and Shub. 

   [Bruce Schneier, "Applied Cryptography", Second Edition, John Wiley
   & Sons, Inc., 1996, pp. 417-418.]

   "Find two large prime numbers, p and q, which are congruent to 3
   modulo 4.  The product of those numbers, n, is a blum integer.
   Choose another random integer, x, which is relatively prime to n.
   Compute
   	x[0] = x^2 mod n
   That's the seed for the generator."

   To generate a random bit, compute
   	x[i] = x[i-1]^2 mod n
   The least significant bit of x[i] is the one we want.

   We can use more than one bit from x[i], namely the
   	log2(bitlength of x[i])
   least significant bits of x[i].

   So, for a 32-bit seed we get 5 bits per computation.

   The non-predictability of this generator is based on the difficulty
   of factoring n.

 */


#include "gmp.h"
#include "gmp-impl.h"


/* Generate a random mpz_t.  The number will be in the range
   [0,2^nbits).  */

void
#if __STDC__
mpz_urandomb (mpz_t rop, unsigned long int nbits, gmp_rand_state *s)
#else
mpz_urandomb (rop, nbits, s)
     mpz_t rop;
     unsigned long int nbits;
     gmp_rand_state *s;
#endif
{
  mp_ptr rp;
  mp_size_t size;

  size = nbits / BITS_PER_MP_LIMB + (nbits % BITS_PER_MP_LIMB != 0);
  if (size > SIZ (rop))
    _mpz_realloc (rop, size);

  rp = PTR (rop);

  mpn_rawrandom (rp, nbits, s);
  MPN_NORMALIZE (rp, size);
  SIZ (rop) = size;
}

#if 0
/* Generate a random mpz_t.  The number will be in the range [0,n).  */
void
#if __STDC__
mpz_urandomn (mpz_t rop, mpz_t n, gmp_rand_state *s)
#else
mpz_urandomn (rop, n, s)
     mpz_t rop;
     mpz_t n;
     gmp_rand_state *s;
#endif
{
  mp_ptr rp;
  mp_size_t size, bsize;
  unsigned cnt;			/* FIXME: type? */

  rp = PTR (rop);
  size = SIZ (n);
  count_leading_zeros (cnt, rp[size - 1]);
  bsize = BITS_PER_MP_LIMP * size - cnt;

  mpn_rawrandom (tp, bsize + 20, s);
  MPN_NORMALIZE (tp, size);	
  SIZ (rop) = size;

  mpz_tdiv_r (rop, rop, n);	/* reduce to spec'd interval */
}
#endif
