/* _gmp_rand (rp, state, nbits) -- Generate a random bitstream of
   length NBITS in RP.  RP must have enough space allocated to hold
   NBITS.

Copyright (C) 1999, 2000  Free Software Foundation, Inc.

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

/* -------------------------------------------------- */

/* lc (rp, state) -- Generate next number in LC sequence.  Return the
   number of valid bits in the result.  NOTE: If 'm' is a power of 2
   (m2exp != 0), discard the lower half of the result.  */

static mp_size_t
#if __STDC__
lc (mp_ptr rp, gmp_randstate_t rstate)
#else
lc (rp, rstate)
     mp_ptr rp;
     gmp_randstate_t rstate;
#endif
{
  mp_ptr t1p, t2p, seedp, ap;
  mp_size_t t1n, t2n, seedn, an;
  mp_size_t shiftcount, retval;
  mp_limb_t tlimb;
  unsigned long int m2exp;
  TMP_DECL (mark);

  seedp = PTR (rstate->seed);
  seedn = SIZ (rstate->seed);
  /* An mpz with value 0 is represented with SIZ() == 0, which
     confuses the code below.  Say that SIZ() = 1 instead.  */
  if (seedn == 0)
    seedn = 1;

  ap = PTR (rstate->algdata.lc->a);
  an = SIZ (rstate->algdata.lc->a);

  m2exp = rstate->algdata.lc->m2exp;

  /* Allocate temporary storage.  t1 = a * seed, t2 = t1 + c % m.  */
  
  TMP_MARK (mark);
  t1n = an + seedn;
  t1p = TMP_ALLOC (t1n * BYTES_PER_MP_LIMB);

  t2n = MAX (t1n, an) + 1;	/* One extra for the add.  */
  if (m2exp == 0)
    t2n++;			/* One extra for the normalizing
                                   shift.  */
  t2p = TMP_ALLOC (t2n * BYTES_PER_MP_LIMB);
  /* Clear destination.  FIXME: Only necessary when SIZ (a*seed+c) <
     SIZ ((a*seed+c)%m), which happens in optimized modulo calculation
     where m is a power of two.  */
  MPN_ZERO (t2p, t2n);

  /* t1 = a * seed */
  if (seedn >= an)
    tlimb = mpn_mul (t1p, seedp, seedn, ap, an);
  else
    tlimb = mpn_mul (t1p, ap, an, seedp, seedn);
  t1n = an + seedn - (tlimb == 0);

  /* t2 = t1 + c */
  t2n = t1n;
  if (mpn_add_1 (t2p, t1p, t1n, (mp_limb_t) rstate->algdata.lc->c))
    {
      t2p[t2n] = 1;	/* Add carry. */
      t2n++;
    }
      
  /* t2 = t2 % m */
  if (m2exp != 0)
    {
      /* 'm' is a power of 2.  No need for a division, just trim the
	 size and clear some bits in (new) most significant limb.  */
      mp_size_t msli = m2exp / BITS_PER_MP_LIMB;
      mp_size_t savebits = m2exp % BITS_PER_MP_LIMB;

      /* FIXME: Avoid shifting if all bits to be cleared are 0
         already?  */
      if (savebits != 0)
	t2p[msli] &= (((mp_limb_t) 1 << savebits) - 1);
      else
	msli--;			/* Correction.  */

      t2n = msli + 1;
    }
  else
    {
      /* 'm' is not a power of 2.  The modulus operation is done with
	 mpn_divrem() after normalizing 'm' and compensating this by
	 shifting 't2'.  */
      mp_ptr mp, mcopyp;
      mp_size_t mn;

      /* Store normalized copy of 'm' in 'mcopy'.  */
      /* FIXME: Assumption: m != 0  */
      mp = PTR (rstate->algdata.lc->m);
      mn = SIZ (rstate->algdata.lc->m);
      count_leading_zeros (shiftcount, mp[mn - 1]); 
      if (shiftcount != 0)
	{
	  mcopyp = TMP_ALLOC (mn * BYTES_PER_MP_LIMB);
	  mpn_lshift (mcopyp, mp, mn, shiftcount);
	}
      else
	mcopyp = mp;

      /* Shift t2 left the same amount that m was shifted.  */
      if (shiftcount != 0)
	{
	  tlimb = mpn_lshift (t2p, t2p, t2n, shiftcount);
	  if (tlimb)
	    {
	      t2p[t2n] = tlimb;
	      t2n++;
	    }
	}

      mpn_divrem (t2p + mn, 0, t2p, t2n, mcopyp, mn);
      t2n = mn;
      /* Remainder is in t2.  */

      /* Shift down remainder.  */
      if (shiftcount != 0)
	{
	  mpn_rshift (t2p, t2p, t2n, shiftcount);
	  MPN_NORMALIZE (t2p, t2n);
	}
    }

  /* Save result as next seed.  */
  MPN_COPY (PTR (rstate->seed), t2p, t2n);
  SIZ (rstate->seed) = t2n;

  if (m2exp != 0)
    {
      /* Discard the lower half of the result.  */
      mp_size_t discardb = m2exp / 2;
      mp_size_t discardl = discardb / BITS_PER_MP_LIMB;

      t2n -= discardl;
      mpn_rshift (t2p, t2p + discardl, t2n, discardb % BITS_PER_MP_LIMB);
    }

  /* Store result at destination.  */
  MPN_COPY (rp, t2p, t2n);

  TMP_FREE (mark);

  /* Return number of valid bits in the result.  */
  if (m2exp != 0)
    retval = m2exp / 2 + m2exp % 2;
  else
    retval = SIZ (rstate->algdata.lc->m) * BITS_PER_MP_LIMB - shiftcount;
  return retval;
}

void
#if __STDC__
_gmp_rand (mp_ptr rp, gmp_randstate_t rstate, unsigned long int nbits)
#else
_gmp_rand_getraw (rp, rstate, nbits)
     mp_ptr rp;
     gmp_randstate_t rstate;
     unsigned long int nbits;
#endif
{
  mp_size_t rn;			/* Size of R.  */
  mp_size_t nbits_stored;	/* Bits stored in R so far.  */
  mp_size_t ri;			/* Index for current limb in R.  */

  rn = nbits / BITS_PER_MP_LIMB + (nbits % BITS_PER_MP_LIMB != 0);
  MPN_ZERO (rp, rn);		/* Clear destination. */

  switch (rstate->alg)
    {
    case GMP_RAND_ALG_LC:
      {
	mp_ptr tp;
	mp_size_t tn;
	mp_size_t nrandbits;
	mp_size_t shiftc;
	mp_limb_t tlimb;
	TMP_DECL (lcmark);
  
	/* Temp space. */
	TMP_MARK (lcmark);
	tp = TMP_ALLOC ((rn + 1) * BYTES_PER_MP_LIMB);

  /* Main loop:
     Fill RP with NBITS random bits.
     RP is built from least significant limb and up.
     RN is the number of limbs in RP.
     NBITS_STORED is the number of bits stored in RP so far.
     RI is the index in RP to currently most significant limb,
     i.e. where next chunk of random bits are to be copied.

     i) Get random bits from lc() and store them in TP.  
     NRANDBITS is the number of random bits in TP.
     TN is the number of limbs in TP.

     ii) Shift TP left as many steps as necessary for copying TP into
     RP.
     Mask in least significant limb of TP into (currently) most
     significant limb of RP.

     iii) Copy the rest of the limbs in TP to RP.  */

	ri = 0;
	nbits_stored = 0;
	while (nbits_stored < nbits)
	  {
	    nrandbits = lc (tp, rstate);
	    tn = nrandbits / BITS_PER_MP_LIMB
	      + (nrandbits % BITS_PER_MP_LIMB != 0);

	    /* Shift the bits in place for copying into
               destination. */
	    shiftc = nbits_stored % BITS_PER_MP_LIMB;
	    if (shiftc != 0)
	      {
		tlimb = mpn_lshift (tp, tp, tn, shiftc);
		if (tlimb != 0)
		  tp[tn++] = tlimb;
	      }

	    /* Mask in least significant limb.  */
	    rp[ri] |= tp[0];

	    /* Is rp[ri] full yet?  */
	    if (nrandbits >= BITS_PER_MP_LIMB - shiftc)
	      {
		ri++;
		nbits_stored += BITS_PER_MP_LIMB - shiftc;
		nrandbits -= BITS_PER_MP_LIMB - shiftc;
	      }
	    else
	      {
		nbits_stored += nrandbits;
		nrandbits = 0;
	      }

	    if (nbits_stored >= nbits)
	      break;

	    /* More bits to store?  */
	    if (nrandbits > 0)
	      {
		mp_size_t storec;

		/* Don't store more than the caller has asked for.  */
		if (nrandbits > BITS_PER_MP_LIMB * (rn - ri))
		  nrandbits = BITS_PER_MP_LIMB * (rn - ri);
		storec = nrandbits / BITS_PER_MP_LIMB
		  + (nrandbits % BITS_PER_MP_LIMB != 0);

		MPN_COPY (rp + ri, tp + 1, storec);
		ri += storec;
		nbits_stored += nrandbits;
	      }
	  }

	/* Clear excess bits (most significant ones).  */
	if (nbits_stored > nbits)
	  {
	    mp_size_t nsave = nbits % BITS_PER_MP_LIMB;
	    rp[rn - 1] &= (((mp_size_t) 1 << nsave) - 1);
	  }

	TMP_FREE (lcmark);
	break;
      }
      
    default:
      gmp_errno |= GMP_ERROR_UNSUPPORTED_ARGUMENT;
      break;
    }
}
