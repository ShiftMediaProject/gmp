/* mpn_rawrandom (rp, nbits, state) -- FIXME: document func. */
/* FIXME: add fsf blurb */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"		/* for count_leading_zeros() */

/* Generate a random bitstream of length NBITS in RP. */

/* RP must have enough space allocated to hold NBITS. */

void
#if __STDC__
mpn_rawrandom (mp_ptr rp, unsigned long int nbits, gmp_rand_state s)
#else
mpn_rawrandom (rp, nbits, s)
     mp_ptr rp;
     unsigned long int nbits;
     gmp_rand_state s;
#endif
{
  mp_ptr tp, seedp, ap, mp, mcopyp, sump, dstp;
  mp_size_t tsize, rpsize, seedsize, asize, msize, sumsize, n;
  unsigned shiftcount;		/* FIXME: type? */
  unsigned genbits;
  mp_size_t nlimbs;
  mp_limb_t tlimb;		/* temp */
  

  seedp = PTR (s->seed);
  ap = PTR (s->data.lc->a);
  asize = SIZ (s->data.lc->a);
  mp = PTR (s->data.lc->m);
  msize = SIZ (s->data.lc->m);
#if 0
  MPN_NORMALIZE (mp, msize);	/* FIXME: Callers responsibility? */
#endif
  rpsize = nbits / BITS_PER_MP_LIMB + (nbits % BITS_PER_MP_LIMB != 0);

  TMP_DECL (mark);


  MPN_ZERO (rp, rpsize);	/* Clear destination. */

  /* Temporary areas and their maximum sizes:
     tp = seed*a -- SIZ(m)+SIZ(a) [since seed can grow up to the size of m]
     sump = seed*a + c -- SIZ(tp) + 2 [one for c and one for shifting left]
     mcopyp = shifted m -- SIZ(m)
  */
     
  TMP_MARK (mark);
  tsize = msize + asize;
  tp = TMP_ALLOC (tsize * BYTES_PER_MP_LIMB); 
  sump = TMP_ALLOC ((2 + tsize) * BYTES_PER_MP_LIMB);
  mcopyp = TMP_ALLOC (msize * BYTES_PER_MP_LIMB);

  /* mpn_divrem() documentation: "It is required that the most
     significant bit of the divisor is set." */
  /* Normalize m (shift left) and store in mcopyp. */
  /* Assumption: m != 0 */
  count_leading_zeros (shiftcount, mp[msize - 1]); 
  if (shiftcount)
    mpn_lshift (mcopyp, mp, msize, shiftcount);

  nlimbs = rpsize;
  dstp = rp;
  genbits = 0;
  /* rop = (seed * a + c) % m */
  while (genbits < nbits)
    {
      seedsize = SIZ (s->seed);
      if (seedsize >= asize)
	tlimb = mpn_mul (tp, seedp, seedsize, ap, asize);
      else
	tlimb = mpn_mul (tp, ap, asize, seedp, seedsize);
      sumsize = asize + seedsize + (tlimb != 0);

      if (mpn_add_1 (sump, tp, sumsize, (mp_limb_t) s->data.lc->c))
	{
	  sump[sumsize] = 1;	/* Add carry. */
	  sumsize++;
	}
      
      /* Shift sum left the same amount that m was shifted. */
      if (shiftcount)
	{
	  tlimb = mpn_lshift (sump, sump, sumsize, shiftcount);
	  if (tlimb)
	    {
	      sump[sumsize] = tlimb;
	      sumsize++;
	    }
	}

      mpn_divrem (sump + msize, 0, sump, sumsize, mcopyp, msize);
      /* Now remainder is in sump, size is msize. */
      sumsize = msize;		/* FIXME: Convenient. */

      /* Shift down remainder. */
      if (shiftcount)
	mpn_rshift (sump, sump, sumsize, shiftcount);
      MPN_NORMALIZE (sump, sumsize);

      /* Save result as next seed.  Make sure it's space for it. */
      if (sumsize > SIZ (s->seed))
	  _mpz_realloc (s->seed, sumsize);
      MPN_COPY (PTR (s->seed), sump, sumsize);
      SIZ (s->seed) = sumsize;

      MPN_COPY (dstp, sump, sumsize);

      /* We have now generated s->size (SIZE) number of bits.  If
      NBITS > SIZE, we take another round and generate another SIZE
      number of bits, *overwriting* the X most significant bits of the
      result from this round.  (Not too efficient!  In worst case,
      we'll only generate 1 bit per round.  FIXME.) X is in this case
      NBITS % BITS_PER_MP_LIMB.  */

      if (genbits)
	genbits -= nbits % BITS_PER_MP_LIMB;
      genbits += s->size;
    }


  /* Mask off excess bits. */
  /* If we shift down, discarding least signifant bits, I *think* that
     we are making a mistake.  I don't think the numbers are
     universally distributed. */
  n = genbits % nbits;
  if (n)
    rp[rpsize - 1] &= (~(mp_limb_t) 0) >> (BITS_PER_MP_LIMB - n);

  TMP_FREE (mark);
}
