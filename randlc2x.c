/* Linear Congruential pseudo-random number generator functions.

Copyright 1999, 2000, 2001, 2002  Free Software Foundation, Inc.

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

/* State structure for LC.  */
typedef struct {
  mpz_t _mp_seed;		/* Current value.  Fixed size.  */
  mpz_t _mp_a;			/* Multiplier.  */
  mp_size_t _cn;		/* Number of limbs of the adder.  */
  mp_limb_t _cp[LIMBS_PER_ULONG];	/* Limbs of the adder.  */
  unsigned long int _mp_m2exp;	/* Modulus is 2 ^ m2exp.  */
} gmp_rand_lc_struct;


/* lc (rp, state) -- Generate next number in LC sequence.  Return the
   number of valid bits in the result.  Discards the lower half of the
   result.  */

static unsigned long int
lc (mp_ptr rp, gmp_randstate_t rstate)
{
  mp_ptr tp, seedp, ap;
  mp_size_t ta;
  mp_size_t tn, seedn, an;
  unsigned long int m2exp;
  unsigned long int bits;
  int cy;
  mp_size_t xn;
  gmp_rand_lc_struct *p;
  TMP_DECL (mark);

  p = (gmp_rand_lc_struct *) RNG_STATE (rstate);

  m2exp = p->_mp_m2exp;

  seedp = PTR (p->_mp_seed);
  seedn = SIZ (p->_mp_seed);

  ap = PTR (p->_mp_a);
  an = SIZ (p->_mp_a);

  /* Allocate temporary storage.  Let there be room for calculation of
     (A * seed + C) % M, or M if bigger than that.  */

  TMP_MARK (mark);

  ta = an + seedn + 1;
  tn = BITS_TO_LIMBS (m2exp);
  if (ta <= tn) /* that is, if (ta < tn + 1) */
    {
      mp_size_t tmp = an + seedn;
      ta = tn + 1;
      tp = (mp_ptr) TMP_ALLOC (ta * BYTES_PER_MP_LIMB);
      MPN_ZERO (&tp[tmp], ta - tmp); /* mpn_mul won't zero it out.  */
    }
  else
    tp = (mp_ptr) TMP_ALLOC (ta * BYTES_PER_MP_LIMB);

  /* t = a * seed.  NOTE: an is always > 0; see initialization.  */
  if (seedn > an)
    mpn_mul (tp, seedp, seedn, ap, an);
  else
    mpn_mul (tp, ap, an, seedp, seedn);

  /* t = t + c.  NOTE: tn is always >= p->_cn (precondition for __GMPN_ADD);
     see initialization.  */
  __GMPN_ADD (cy, tp, tp, tn, p->_cp, p->_cn);

  /* t = t % m */
  tp[m2exp / GMP_NUMB_BITS] &= (CNST_LIMB (1) << m2exp % GMP_NUMB_BITS) - 1;

  /* Save result as next seed.  */
  MPN_COPY (PTR (p->_mp_seed), tp, tn);

  /* Discard the lower m2exp/2 of the result.  */
  bits = m2exp / 2;
  xn = bits / GMP_NUMB_BITS;

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

  TMP_FREE (mark);

  /* Return number of valid bits in the result.  */
  return (m2exp + 1) / 2;
}


/* Obtain a sequence of random numbers.  */
static void
randget_lc (gmp_randstate_t rstate, mp_ptr rp, unsigned long int nbits)
{
  mp_size_t rn;			/* Size of R.  */
  unsigned long int rbitpos;
  int chunk_nbits;
  mp_ptr tp;
  mp_size_t tn;
  gmp_rand_lc_struct *p;
  TMP_DECL (lcmark);

  p = (gmp_rand_lc_struct *) RNG_STATE (rstate);

  rn = BITS_TO_LIMBS (nbits);

  TMP_MARK (lcmark);

  chunk_nbits = p->_mp_m2exp / 2;
  tn = BITS_TO_LIMBS (chunk_nbits);

  tp = (mp_ptr) TMP_ALLOC (tn * BYTES_PER_MP_LIMB);

  rbitpos = 0;
  while (rbitpos + chunk_nbits <= nbits)
    {
      mp_ptr r2p = rp + rbitpos / GMP_NUMB_BITS;

      if (rbitpos % GMP_NUMB_BITS != 0)
	{
	  mp_limb_t savelimb, rcy;
	  /* Target of new chunk is not bit aligned.  Use temp space
	     and align things by shifting it up.  */
	  lc (tp, rstate);
	  savelimb = r2p[0];
	  rcy = mpn_lshift (r2p, tp, tn, rbitpos % GMP_NUMB_BITS);
	  r2p[0] |= savelimb;
	  /* bogus */
	  if ((chunk_nbits % GMP_NUMB_BITS + rbitpos % GMP_NUMB_BITS)
	      > GMP_NUMB_BITS)
	    r2p[tn] = rcy;
	}
      else
	{
	  /* Target of new chunk is bit aligned.  Let `lc' put bits
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
      tn = BITS_TO_LIMBS (last_nbits);
      lc (tp, rstate);
      if (rbitpos % GMP_NUMB_BITS != 0)
	{
	  mp_limb_t savelimb, rcy;
	  /* Target of new chunk is not bit aligned.  Use temp space
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
	  &= ~(~CNST_LIMB (0) << nbits % GMP_NUMB_BITS);
    }

  TMP_FREE (lcmark);
}


static void
randseed_lc (gmp_randstate_t rstate, mpz_srcptr seed)
{
  gmp_rand_lc_struct *p = (gmp_rand_lc_struct *) RNG_STATE (rstate);
  mpz_ptr seedp = p->_mp_seed;
  mp_size_t seedn = BITS_TO_LIMBS (p->_mp_m2exp);

  /* Store p->_mp_seed as an unnormalized integer with size enough
     for numbers up to 2^m2exp-1.  That size can't be zero.  */
  mpz_fdiv_r_2exp (seedp, seed, p->_mp_m2exp);
  MPN_ZERO (&PTR (seedp)[SIZ (seedp)], seedn - SIZ (seedp));
  SIZ (seedp) = seedn;
}


static void
randclear_lc (gmp_randstate_t rstate)
{
  gmp_rand_lc_struct *p = (gmp_rand_lc_struct *) RNG_STATE (rstate);

  mpz_clear (p->_mp_seed);
  mpz_clear (p->_mp_a);
  (*__gmp_free_func) (p, sizeof (gmp_rand_lc_struct));
}

static const gmp_randfnptr_t Linear_Congruential_Generator = {
  randseed_lc,
  randget_lc,
  randclear_lc
};

/* Initialize LC-specific data.  */
void
gmp_randinit_lc_2exp (gmp_randstate_t rstate,
		      mpz_srcptr a,
		      unsigned long int c,
		      unsigned long int m2exp)
{
  gmp_rand_lc_struct *p;
  mp_size_t seedn = BITS_TO_LIMBS (m2exp);

  ASSERT_ALWAYS (m2exp != 0);

  RNG_FNPTR (rstate) = (void *) &Linear_Congruential_Generator;
  RNG_STATE (rstate) =
    (mp_ptr) (*__gmp_allocate_func) (sizeof (gmp_rand_lc_struct));

  p = (gmp_rand_lc_struct *) RNG_STATE (rstate);
  mpz_init2 (p->_mp_seed, m2exp);

  /* Set parameters and default seed.  */
  MPN_ZERO (PTR (p->_mp_seed), seedn);
  SIZ (p->_mp_seed) = seedn;
  PTR (p->_mp_seed)[0] = 1;

  mpz_init2 (p->_mp_a, m2exp);
  /* Avoid negative a.  */
  mpz_fdiv_r_2exp (p->_mp_a, a, m2exp);

  /* Avoid SIZ(a) == 0 to avoid checking for special case in lc().  */
  if (SIZ (p->_mp_a) == 0)
    {
      SIZ (p->_mp_a) = 1;
      PTR (p->_mp_a)[0] = CNST_LIMB (0);
    }

  MPN_SET_UI (p->_cp, p->_cn, c);

  /* Internally we may discard any bits of c above m2exp.  The following
     code ensures that __GMPN_ADD in lc() will always work.  */
  if (seedn == 1 && p->_cn == 2) /* Only case to worry about.  */
    {
      p->_cp[1] = CNST_LIMB (0); /* Mask out c (probably not needed.) */
      p->_cn = (p->_cp[0] != 0); /* Fix size.  */
    }

  p->_mp_m2exp = m2exp;
}
