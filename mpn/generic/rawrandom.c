/* mpn_rawrandom (rp, nbits, state) -- FIXME: document func. */
/* FIXME: add fsf blurb */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"		/* for count_leading_zeros() */

/* Generate a random bitstream of length NBITS in RP. */

/* RP must have enough space allocated to hold NBITS. */

#if 0
 #if BITS_PER_LONGINT >= BITS_PER_MP_LIMB
  #define LIMBS_PER_LONGINT (BITS_PER_LONGINT / BITS_PER_MP_LIMB)
 #else
  #define LIMBS_PER_LONGINT 1
 #endif  
#endif

void
#if __STDC__
mpn_rawrandom (mp_ptr rp, unsigned long int nbits, gmp_rand_state *s)
#else
mpn_rawrandom (rp, nbits, s)
     mp_ptr rp;
     unsigned long int nbits;
     gmp_rand_state *s;
#endif
{
  mp_ptr tp, seedp, ap, mp, mcopyp, sump, dstp;
  mp_size_t tsize, rpsize, seedsize, asize, msize, mcopysize, sumsize, n;
  unsigned shiftcount;		/* FIXME: type? */
  mp_size_t limbstocopy, nlimbs;
  mp_limb_t tlimb;		/* temp */
  

  seedp = PTR (s->seed);
  seedsize = SIZ (s->seed);
  ap = PTR (s->data.lc->a);
  asize = SIZ (s->data.lc->a);
  mp = PTR (s->data.lc->m);
  msize = SIZ (s->data.lc->m);
#if 0
  MPN_NORMALIZE (mp, msize);	/* FIXME: Callers responsibility? */
#endif
  rpsize = nbits / BITS_PER_MP_LIMB + (nbits % BITS_PER_MP_LIMB != 0);

  TMP_DECL (mark);

  /* We need two temp areas to work in.  The product seed*a needs
     SIZ(seed)+SIZ(a) limbs. The sum seed*a + c may need one more
     limb.  The result after doing mod m needs SIZ(m) limbs. */

  /* Assumption: SIZ (seed) + SIZ (a) >= LIMBS_PER_LONGINT */

  TMP_MARK (mark);
  tsize = seedsize + asize;
  tp = TMP_ALLOC (tsize * BYTES_PER_MP_LIMB); 

  /* Two extra limbs, one for adding c and one for shifting when
     normalizing m. */
  sump = TMP_ALLOC ((2 + tsize) * BYTES_PER_MP_LIMB);

  /* We need a copy of m since it has to be shifted left in order to
     fulfill the requirements of mpn_divrem(). */
  mcopysize = msize;		
  mcopyp = TMP_ALLOC (msize * BYTES_PER_MP_LIMB);

  /* mpn_divrem doc: "It is required that the most significant bit of
     the divisor is set." */
  /* Problem: mpn_lshift() shifts at most n-1 bits "on an n-bit
     machine."  Is that BITS_PER_MP_LIMB-1? */
  /* Easy solution: Shift left one bit at a time and right shift
     when first 1 is shifted out. */
  /* Assumption: m != 0 */

/* How does count_leading_zeros() work, really?  Result in a variable
   passed by value.  A macro?  Can't find it. */
  count_leading_zeros (shiftcount, mp[msize - 1]); 
  if (shiftcount)
    mpn_lshift (mcopyp, mp, msize, shiftcount);

#if STUPID_SLOW_EASY_WAY_OF_NORMALIZING
  shiftcount = 0;
  if (!mpn_lshift (mcopyp, mp, msize, 1))
    do
      shiftcount++;
    while (!mpn_lshift (mcopyp, mcopyp, msize, 1));
  mpn_rshift (mcopyp, mcopyp, msize, 1);
  mcopyp[msize-1] |= (1 << (BITS_PER_MP_LIMB - 1)); /* set most
                                                       significant bit */
#endif
	
  nlimbs = nbits / BITS_PER_MP_LIMB + (nbits % BITS_PER_MP_LIMB != 0);
  dstp = rp;
  /* rop = (seed * a + c) % m */
  do
    {
      if (seedsize >= asize)
	tlimb = mpn_mul (tp, seedp, seedsize, ap, asize);
      else
	tlimb = mpn_mul (tp, ap, asize, seedp, seedsize);
      if (0 == tlimb)
	tsize--;

      sumsize = tsize;
      if (mpn_add_1 (sump, tp, tsize, (mp_limb_t) s->data.lc->c))
	{
	  sump[sumsize] = 1;	/* Add carry. */
	  sumsize++;
	}
      
      /* Shift sum the same amount that m was shifted to produce the
         normalized copy in mcopyp. */
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

      /* FIXME: Use the s->size least significant bits.  Or should we
         skip the BITS field in the scheme struct and let M determine
         the size alone?  The user can still construct a fast (and
         bad) generator by specing a scheme with a small M and ask for
         lots of bits (NBITS).  In the case where the user passes
         gmp_rand_init() a SIZE that doesn't correspond to any
         tabulated scheme, we end up with two different situations:
         (1) we get a scheme with a larger M than necessary, or (2) we
         get a scheme with a smaller M than necessary.  The size of M
         will affect sumsize above and all is well.  */

      /* Save result as next seed.  Make sure it's space for it. */
      if (sumsize > SIZ (s->seed))
	_mpz_realloc (s->seed, sumsize);
      MPN_COPY (PTR (s->seed), sump, sumsize);
      
      /* Don't copy more than the user wants. */
      limbstocopy = MIN (nlimbs, sumsize); 

      MPN_COPY (dstp, sump, limbstocopy);
      dstp += limbstocopy;

      nlimbs -= limbstocopy;
    }
  while (nlimbs > 0);

  /* Mask off excess bits. */
  /* If we shift down, discarding least signifant bits, I *think* that
     we are making a mistake.  I don't think the numbers are
     universally distributed. */
  n = nbits % BITS_PER_MP_LIMB;
  if (n)
#if DISCARDING_LOWBITS
    mpn_rshift (rp, rp, nbits, BITS_PER_MP_LIMB - n);
#else
    rp[rpsize - 1] &= (~(mp_limb_t) 0) >> (BITS_PER_MP_LIMB - n);
#endif

  TMP_FREE (mark);
}

#if OLD_MPZ_STYLE
  switch (s->alg)
    {
    case GMP_RAND_ALG_LC:
      /* rop = (seed * a + c) % m */
      mpz_mul (rop, s->seed, s->data.lc->scheme.a);
      mpz_add_ui (rop, rop, s->data.lc->scheme.c);
      mpz_mod (rop, rop, s->data.lc->scheme.m);
  
      /* Save result for next seed. */
      /* FIXME: This is the right place to save seed, right? */
      mpz_set (s->seed, rop);

      break;			/* GMP_RAND_ALG_LC */

    case GMP_RAND_ALG_BBS:
      /* We can use log2(s->size) bits from the calculation, assuming
	 that seed contains at least s->size bits of random data. */
      /* rop = seed^2 mod s->data.bbs->bi */

      mpz_powm_ui (rop, s->seed, 2, s->data.bbs->bi);

      /* Save result for next seed. */
      mpz_set (s->seed, rop);

      /* FIXME: Mask result? */

      break;
    }
#endif

