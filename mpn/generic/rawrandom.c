/* mpn_rawrandom (rp, nbits, state) -- Generate a random bitstream of
   length NBITS in RP.  RP must have enough space allocated to hold
   NBITS.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"		/* for count_leading_zeros() */

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
  mp_size_t shiftcount, savecount, genbits;
  mp_limb_t tlimb;
  TMP_DECL (mark);

  
  seedp = PTR (s->seed);
  ap = PTR (s->data.lc->a);
  asize = SIZ (s->data.lc->a);
  mp = PTR (s->data.lc->m);
  msize = SIZ (s->data.lc->m);
  rpsize = nbits / BITS_PER_MP_LIMB + (nbits % BITS_PER_MP_LIMB != 0);
  MPN_ZERO (rp, rpsize);	/* Clear destination. */

  /* Temporary areas and their maximum sizes:
     tp = seed*a -- SIZ(m)+SIZ(a) [since seed can grow up to the size of m]
     sump = seed*a + c -- SIZ(tp) + 2 [one for c and one for shifting left]
     mcopyp = shifted m -- SIZ(m) */
     
  TMP_MARK (mark);
  tsize = msize + asize;
  tp = TMP_ALLOC (tsize * BYTES_PER_MP_LIMB); 
  sump = TMP_ALLOC ((2 + tsize) * BYTES_PER_MP_LIMB);

  /* mpn_divrem() documentation: "It is required that the most
     significant bit of the divisor is set."  Normalize m (shift left)
     and store in mcopyp.  Assumption: m != 0 */

  count_leading_zeros (shiftcount, mp[msize - 1]); 
  if (shiftcount)
    {
      mcopyp = TMP_ALLOC (msize * BYTES_PER_MP_LIMB);
      mpn_lshift (mcopyp, mp, msize, shiftcount);
    }
  else
    mcopyp = mp;

  /* rp = (seed * a + c) % m */
  dstp = rp;
  genbits = savecount = 0;
  while (1)
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
      sumsize = msize;		/* Now remainder is in sump, size is
                                   msize. */

      /* Shift down remainder. */
      if (shiftcount)
	{
	  mpn_rshift (sump, sump, sumsize, shiftcount);
	  MPN_NORMALIZE (sump, sumsize);
	}

      /* Save result as next seed.  Make sure it's space for it. */
      if (sumsize > SIZ (s->seed))
	_mpz_realloc (s->seed, sumsize);
      MPN_COPY (PTR (s->seed), sump, sumsize);
      SIZ (s->seed) = sumsize;

      if (savecount)		/* Save some bits? */
	{			
	  sump[sumsize] = mpn_lshift (sump, sump, sumsize, savecount);
	  sumsize++;
	  tlimb = *dstp & (~(mp_limb_t) 0 >> (BITS_PER_MP_LIMB - savecount));
	}

      /* Store result at destination. */
      MPN_COPY (dstp, sump, sumsize);

      /* Restore `savecount' lower bits of tlimb in *dstp. */
      if (savecount)
	{
	  *dstp &= (~(mp_limb_t) 0 >> (BITS_PER_MP_LIMB - savecount));
	  *dstp |= tlimb;
	}

      genbits += s->size;

      if (genbits < nbits)
	{			/* Another round. */
	  savecount = genbits % BITS_PER_MP_LIMB;
	  dstp += sumsize;
	}
      else
	break;			/* Done. */
    }

  /* Mask off excess bits.  Save least significant bits. */
  n = nbits % BITS_PER_MP_LIMB;
  if (n)
    rp[rpsize - 1] &= ((~(mp_limb_t) 0) >> (BITS_PER_MP_LIMB - n));

  TMP_FREE (mark);
}
