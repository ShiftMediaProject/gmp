/* Mersenne Twister pseudo-random number generator functions.

Copyright 2002 Free Software Foundation, Inc.

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
MA 02111-1307, USA.  */

#include "gmp.h"
#include "gmp-impl.h"

/* This code implements the Mersenne Twister pseudorandom number generator
   by Takuji Nishimura and Makoto Matsumoto.  The buffer initialization
   function is different in order to permit seeds greater than 2^32-1.  */

/* Number of extractions used to warm the buffer up.  */
#define WARM_UP 2000

/* Default seed to use when the generator is not initialized.  */
#define DEFAULT_SEED 5489 /* was 4357 */

/* Period parameters.  */
#define N 624
#define M 397
#define MATRIX_A 0x9908B0DF   /* Constant vector a.  */

/* Tempering masks.  */
#define MASK_1 0x9D2C5680
#define MASK_2 0xEFC60000

/* State structure for MT.  */
typedef struct
{
  unsigned long int mt[N];	/* State array.  */
  int mti;			/* Index of current value.  */
} gmp_rand_mt_struct;


static void
recalc_buffer (unsigned long int mt[])
{
  static __gmp_const unsigned long int a_times_y[2] = {0, MATRIX_A};
  /* a_times_y[y] = y * MATRIX_A  for y=0,1 */

  unsigned long int y;
  int kk;

  for (kk = 0; kk < N - M; kk++)
    {
      y = (mt[kk] & 0x80000000) | (mt[kk + 1] & 0x7FFFFFFF);
      mt[kk] = mt[kk + M] ^ (y >> 1) ^ a_times_y[y & 0x01];
    }
  for (; kk < N - 1; kk++)
    {
      y = (mt[kk] & 0x80000000) | (mt[kk + 1] & 0x7FFFFFFF);
      mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ a_times_y[y & 0x01];
    }

  y = (mt[N - 1] & 0x80000000) | (mt[0] & 0x7FFFFFFF);
  mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ a_times_y[y & 0x01];
}


/* Get nbits bits of output from the generator into dest.
   Note that Mersenne Twister is designed to produce outputs in
   32-bit words.  */
static void
randget_mt (gmp_randstate_t rstate, mp_ptr dest, unsigned long int nbits)
{
  unsigned long int y;
  long int nlimbs;
  int rbits;
  long int i;
  int *pmti;
  unsigned long int *mt;

  pmti = &((gmp_rand_mt_struct *) RNG_STATE (rstate))->mti;
  mt = ((gmp_rand_mt_struct *) RNG_STATE (rstate))->mt;

  /* Use a default seed if randseed_mt has not been yet called.  */
  if (*pmti == N + 1)
    gmp_randseed_ui (rstate, DEFAULT_SEED);

  nlimbs = nbits / GMP_NUMB_BITS;
  rbits = nbits % GMP_NUMB_BITS;

#define NEXT_RANDOM			\
  do					\
    {					\
      if (*pmti >= N)			\
	{				\
	  recalc_buffer (mt);		\
	  *pmti = 0;			\
	}				\
      y = mt[(*pmti)++];		\
      y ^= (y >> 11);			\
      y ^= (y << 7) & MASK_1;		\
      y ^= (y << 15) & MASK_2;		\
      y ^= (y >> 18);			\
    }					\
  while (0)


  /* Handle the common cases of 32- or 64-bit limbs with fast,
     optimized routines, and the rest of cases with a general
     routine.  In all cases, no more than 31 bits are rejected
     for the last limb so that every version of the code is
     consistent with the others.  */

#if (GMP_NUMB_BITS == 32)

  for (i = 0; i < nlimbs; i++)
    {
      NEXT_RANDOM;
      dest[i] = (mp_limb_t) y;
    }
  if (rbits)
    {
      NEXT_RANDOM;
      dest[nlimbs] = (mp_limb_t) (y & ~(~0UL << rbits));
    }

#else /* GMP_NUMB_BITS != 32 */
#if (GMP_NUMB_BITS == 64)

  for (i = 0; i < nlimbs; i++)
    {
      NEXT_RANDOM;
      dest[i] = (mp_limb_t) y;
      NEXT_RANDOM;
      dest[i] |= (mp_limb_t) y << 32;
    }
  if (rbits)
    {
      if (rbits < 32)
	{
	  NEXT_RANDOM;
	  dest[nlimbs] = (mp_limb_t) (y & ~(~0UL << rbits))
	}
      else
	{
	  NEXT_RANDOM;
	  dest[nlimbs] = (mp_limb_t) y;
	  if (rbits > 32)
	    {
	      NEXT_RANDOM;
	      dest[nlimbs] |=
		((mp_limb_t) (y & ~(~0UL << (rbits-32)))) << 32;
	    }
	}
    }

#else /* GMP_NUMB_BITS != 64 */

  {
    /* Fall back to a general algorithm.  This algorithm works by
       keeping a pool of up to 64 bits (2 outputs from MT) acting
       as a shift register from which bits are consumed as needed.
       Bits are consumed using the LSB bits of bitpool_l, and
       inserted via bitpool_h and shifted to the right place.  */

    unsigned long int bitpool_h = 0;
    unsigned long int bitpool_l = 0;
    int bits_in_pool = 0;	/* Holds number of valid bits in the pool.  */
    int bits_to_fill;		/* Holds total number of bits to put in
				   destination.  */
    int bitidx;			/* Holds the destination bit position.  */
    int nlimbs2;	/* Number of whole+partial limbs to fill.  */

    nlimbs2 = nlimbs + (rbits != 0);

    for (i = 0; i < nlimbs2; i++)
      {
	bitidx = 0;
	if (i < nlimbs)
	  bits_to_fill = GMP_NUMB_BITS;
	else
	  bits_to_fill = rbits;

	dest[i] = CNST_LIMB (0);
	while (bits_to_fill >= 32) /* Process whole 32-bit blocks first.  */
	  {
	    if (bits_in_pool < 32)	/* Need more bits.  */
	      {
		/* 64-bit right shift. */
		NEXT_RANDOM;
		bitpool_h = y;
		bitpool_l |= (bitpool_h << bits_in_pool) & 0xFFFFFFFFUL;
		if (bits_in_pool == 0)
		  bitpool_h = 0;
		else
		  bitpool_h >>= 32 - bits_in_pool;
		bits_in_pool += 32;	/* We've got 32 more bits.  */
	      }

	    /* Fill a 32-bit chunk */
	    dest[i] |= ((mp_limb_t) bitpool_l) << bitidx;
	    bitpool_l = bitpool_h;
	    bits_in_pool -= 32;
	    bits_to_fill -= 32;
	    bitidx += 32;
	  }

	/* Cover the case where GMP_NUMB_BITS is not a multiple of 32.  */
	if (bits_to_fill != 0)
	  {
	    if (bits_in_pool < bits_to_fill)
	      {
		NEXT_RANDOM;
		bitpool_h = y;
		bitpool_l |= (bitpool_h << bits_in_pool) & 0xFFFFFFFFUL;
		if (bits_in_pool == 0)
		  bitpool_h = 0;
		else
		  bitpool_h >>= 32 - bits_in_pool;
		bits_in_pool += 32;
	      }

	    dest[i] |= (((mp_limb_t) bitpool_l
			 & ~(~CNST_LIMB (0) << bits_to_fill))
			<< bitidx);
	    bitpool_l = ((bitpool_l >> bits_to_fill)
			 | (bitpool_h << (32 - bits_to_fill))) & 0xFFFFFFFFUL;
	    bitpool_h >>= bits_to_fill;
	    bits_in_pool -= bits_to_fill;
	  }
      }
  }

#endif /* GMP_NUMB_BITS != 64 */
#endif /* GMP_NUMB_BITS != 32 */
}


/* Seeding function.  Uses powering modulo a non-Mersenne prime to obtain
   a permutation of the input seed space.  The modulus is 2^19937-20023,
   which is probably prime.  The power is 1074888996.  In order to avoid
   seeds 0 and 1 generating invalid or strange output, the input seed is
   first manipulated as follows:

     seed1 = seed mod (2^19937-20027) + 2

   so that seed1 lies between 2 and 2^19937-20026 inclusive. Then the
   powering is performed as follows:

     seed2 = (seed1^1074888996) mod (2^19937-20023)

   and then seed2 is used to bootstrap the buffer.

   This method aims to give guarantees that:
     a) seed2 will never be zero,
     b) seed2 will very seldom have a very low population of ones in its
	binary representation, and
     c) every seed between 0 and 2^19937-20028 (inclusive) will yield a
	different sequence.

   CAVEATS:

   The period of the seeding function is 2^19937-20027.  This means that
   with seeds 2^19937-20027, 2^19937-20026, ... the exact same sequences
   are obtained as with seeds 0, 1, etc.; it also means that seed -1
   produces the same sequence as seed 2^19937-20028, etc.
 */

static int
randseed_mt (gmp_randstate_t rstate, mpz_srcptr seed)
{
  int i;
  gmp_rand_mt_struct *p;
  mpz_t mod;    /* Modulus.  */
  mpz_t seed1;  /* Intermediate result.  */

  p = (gmp_rand_mt_struct *) RNG_STATE (rstate);

  mpz_init (mod);
  mpz_init (seed1);

  mpz_set_ui (mod, 0);
  mpz_setbit (mod, 19937);
  mpz_sub_ui (mod, mod, 20027);
  mpz_mod (seed1, seed, mod);	/* Reduce `seed' modulo `mod'.  */
  mpz_add_ui (seed1, seed1, 2);	/* seed1 is now ready.  */
  mpz_add_ui (mod, mod, 4);	/* Prepare modulus for powering.  */
  mpz_powm_ui (seed1, seed1, 1074888996, mod);

  /* Split seed1 into N 32-bit chunks.  */
  for (i = 0; i < N; i++)
    {
      p->mt[i] = mpz_get_ui (seed1) & 0xFFFFFFFF;
      mpz_tdiv_q_2exp (seed1, seed1, 32);
    }

  mpz_clear (mod);
  mpz_clear (seed1);

  /* The last bit should be placed at the left.  */
  if (p->mt[N - 1] != 0)
    p->mt[N - 1] = 0x80000000;


  if (WARM_UP)
    {
      /* Warm the generator up.  */
      for (i = 0; i < WARM_UP / N; i++)
	recalc_buffer (p->mt);
      p->mti = WARM_UP % N;
    }
  else
    p->mti = N;

  return 0;
}


static void
randclear_mt (gmp_randstate_t rstate)
{
  (*__gmp_free_func) (RNG_STATE (rstate), sizeof (gmp_rand_mt_struct));
}


static __gmp_const gmp_randfnptr_t Mersenne_Twister_Generator = {
  randseed_mt,
  randget_mt,
  randclear_mt,
};

/* Initialize MT-specific data.  */
void
gmp_randinit_mt (gmp_randstate_t rstate)
{
  /* Set the generator functions.  */
  RNG_FNPTR (rstate) = &Mersenne_Twister_Generator;

  /* Allocate the MT-specific state.  */
  RNG_STATE (rstate) = (*__gmp_allocate_func) (sizeof (gmp_rand_mt_struct));

  /* mti = N + 1 means use default seed.  */
  ((gmp_rand_mt_struct *) RNG_STATE (rstate))->mti = N + 1;
}
