/* _gmp_rand (rp, state, nbits) -- Generate a random bitstream of
   length NBITS in RP.  RP must have enough space allocated to hold
   NBITS.

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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

/* For linear congruential (LC), we use one of algorithms (1) or (2).
   (gmp-3.0 uses algorithm (1) with 'm' as a power of 2.)

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


/* lc (rp, state) -- Generate next number in LC sequence.  Return the
   number of valid bits in the result.  NOTE: If 'm' is a power of 2
   (m2exp != 0), discard the lower half of the result.  */

static
unsigned long int
lc (mp_ptr rp, gmp_randstate_t rstate)
{
  mp_ptr tp, seedp, ap;
  mp_size_t ta;
  mp_size_t tn, seedn, an;
  unsigned long int m2exp;
  mp_limb_t c;
  TMP_DECL (mark);

  m2exp = rstate->_mp_algdata._mp_lc->_mp_m2exp;

  /* The code below assumes the mod part is a power of two.  Make sure
     that is the case.  */
  ASSERT_ALWAYS (m2exp != 0);

  c = (mp_limb_t) rstate->_mp_algdata._mp_lc->_mp_c;

  seedp = PTR (rstate->_mp_seed);
  seedn = SIZ (rstate->_mp_seed);

  if (seedn == 0)
    {
      /* Seed is 0.  Result is C % M.  Assume table is sensibly stored,
	 with C smaller than M.  */
      *rp = c;

      *seedp = c;
      SIZ (rstate->_mp_seed) = 1;
      return m2exp;
    }

  ap = PTR (rstate->_mp_algdata._mp_lc->_mp_a);
  an = SIZ (rstate->_mp_algdata._mp_lc->_mp_a);

  /* Allocate temporary storage.  Let there be room for calculation of
     (A * seed + C) % M, or M if bigger than that.  */

  TMP_MARK (mark);
  ta = an + seedn + 1;
  tp = (mp_ptr) TMP_ALLOC (ta * BYTES_PER_MP_LIMB);

  /* t = a * seed */
  if (seedn >= an)
    mpn_mul (tp, seedp, seedn, ap, an);
  else
    mpn_mul (tp, ap, an, seedp, seedn);
  tn = an + seedn;

  /* t = t + c */
  tp[tn] = 0;			/* sentinel, stops MPN_INCR_U */
  MPN_INCR_U (tp, tn, c);

  ASSERT_ALWAYS (m2exp / GMP_NUMB_BITS < ta);

  /* t = t % m */
  tp[m2exp / GMP_NUMB_BITS] &= ((mp_limb_t) 1 << m2exp % GMP_NUMB_BITS) - 1;
  tn = (m2exp + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS;

  /* Save result as next seed.  */
  MPN_COPY (PTR (rstate->_mp_seed), tp, tn);
  SIZ (rstate->_mp_seed) = tn;

  {
    /* Discard the lower m2exp/2 bits of result.  */
    unsigned long int bits = m2exp / 2;
    mp_size_t xn = bits / GMP_NUMB_BITS;

    tn -= xn;
    if (tn > 0)
      {
	unsigned int cnt = bits % GMP_NUMB_BITS;
	if (cnt != 0)
	  { 
	    mpn_rshift (tp, tp + xn, tn, cnt);
	    MPN_COPY_INCR (rp, tp, xn + 1); 
	  }
	else			/* Even limb boundary.  */
	  MPN_COPY_INCR (rp, tp + xn, tn);
      }
  }

  TMP_FREE (mark);

  /* Return number of valid bits in the result.  */
  return (m2exp + 1) / 2;
}

#ifdef RAWRANDEBUG
/* Set even bits to EVENBITS and odd bits to ! EVENBITS in RP.
   Number of bits is m2exp in state.  */
/* FIXME: Remove.  */
unsigned long int
lc_test (mp_ptr rp, gmp_randstate_t s, const int evenbits)
{
  unsigned long int rn, nbits;
  int f;

  nbits = s->_mp_algdata._mp_lc->_mp_m2exp / 2;
  rn = nbits / GMP_NUMB_BITS + (nbits % GMP_NUMB_BITS != 0);
  MPN_ZERO (rp, rn);

  for (f = 0; f < nbits; f++)
    {
      mpn_lshift (rp, rp, rn, 1);
      if (f % 2 == ! evenbits)
	rp[0] += 1;
    }

  return nbits;
}
#endif /* RAWRANDEBUG */

void
_gmp_rand (mp_ptr rp, gmp_randstate_t rstate, unsigned long int nbits)
{
  mp_size_t rn;			/* Size of R.  */

  rn = (nbits + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS;

  switch (rstate->_mp_alg)
    {
    case GMP_RAND_ALG_LC:
      {
	unsigned long int rbitpos;
	int chunk_nbits;
	mp_ptr tp;
	mp_size_t tn;
	TMP_DECL (lcmark);

	TMP_MARK (lcmark);

	chunk_nbits = rstate->_mp_algdata._mp_lc->_mp_m2exp / 2;
	tn = (chunk_nbits + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS;

	tp = (mp_ptr) TMP_ALLOC (tn * BYTES_PER_MP_LIMB);

	rbitpos = 0;
	while (rbitpos + chunk_nbits <= nbits)
	  {
	    mp_ptr r2p = rp + rbitpos / GMP_NUMB_BITS;

	    if (rbitpos % GMP_NUMB_BITS != 0)
	      {
		mp_limb_t savelimb, rcy;
		/* Target of of new chunk is not bit aligned.  Use temp space
		   and align things by shifting it up.  */
		lc (tp, rstate);
		savelimb = r2p[0];
		rcy = mpn_lshift (r2p, tp, tn, rbitpos % GMP_NUMB_BITS);
		r2p[0] |= savelimb;
/* bogus */	if ((chunk_nbits % GMP_NUMB_BITS + rbitpos % GMP_NUMB_BITS)
		    > GMP_NUMB_BITS)
		  r2p[tn] = rcy;
	      }
	    else
	      {
		/* Target of of new chunk is bit aligned.  Let `lc' put bits
		   directly into our target variable.  */
		lc (r2p, rstate);
	      }
	    rbitpos += chunk_nbits;
	  }

	/* Handle last [0..chunk_nbits) bits.  */
	if (rbitpos != nbits)
	  {
	    mp_ptr r2p = rp + rbitpos / GMP_NUMB_BITS;
	    int last_nbits = nbits - rbitpos;
	    tn = (last_nbits + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS;
	    lc (tp, rstate);
	    if (rbitpos % GMP_NUMB_BITS != 0)
	      {
		mp_limb_t savelimb, rcy;
		/* Target of of new chunk is not bit aligned.  Use temp space
		   and align things by shifting it up.  */
		savelimb = r2p[0];
		rcy = mpn_lshift (r2p, tp, tn, rbitpos % GMP_NUMB_BITS);
		r2p[0] |= savelimb;
		if (rbitpos + tn * GMP_NUMB_BITS - rbitpos % GMP_NUMB_BITS < nbits)
		  r2p[tn] = rcy;
	      }
	    else
	      {
		MPN_COPY (r2p, tp, tn);
	      }
	    /* Mask off top bits if needed.  */
	    if (nbits % GMP_NUMB_BITS != 0)
	      rp[nbits / GMP_NUMB_BITS]
		&= ~ ((~(mp_limb_t) 0) << nbits % GMP_NUMB_BITS);
	  }

	TMP_FREE (lcmark);
	break;
      }

    default:
      ASSERT (0);
      break;
    }
}
