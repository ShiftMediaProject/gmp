/* mpz_rrandomb -- Generate a positive random mpz_t of specified bit size, with
   long runs of consecutive ones and zeros in the binary representation.
   Meant for testing of other MP routines.

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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

static void gmp_rrandomb _PROTO ((mp_ptr rp, gmp_randstate_t rstate, unsigned long int nbits));

void
mpz_rrandomb (mpz_ptr x, gmp_randstate_t rstate, unsigned long int nbits)
{
  mp_size_t nl = 0;

  if (nbits != 0)
    {
      mp_ptr xp;
      nl = (nbits + GMP_NUMB_BITS - 1) / GMP_NUMB_BITS;
      MPZ_REALLOC (x, nl);

      xp = PTR(x);
      gmp_rrandomb (xp, rstate, nbits);
      MPN_NORMALIZE (xp, nl);
    }

  SIZ(x) = nl;
}

/* It's a bit tricky to get this right, so please test the code well if you
   hack with it.  Some early versions of the function produced random numbers
   with the leading limb == 0, and some versions never made the most
   significant bit set.

   This code and mpn_random2 are almost identical, though the latter makes bit
   runs of 1 to 32, and forces the first block to contain 1-bits.

   The random state produces some number of random bits per underlying lc
   invocation (BITS_PER_RANDCALL).  We should perhaps ask for that, instead of
   asking for 32, presuming that limbs are at least 32 bits.  FIXME: Handle
   smaller limbs, such as 4-bit limbs useful for testing purposes, or limbs
   truncated by nailing.

   For efficiency, we make sure to use most bits returned from _gmp_rand, since
   the underlying random number generator is slow.  We keep returned bits in
   ranm/ran, and a count of how many bits remaining in ran_nbits.  */

#define LOGBITS_PER_BLOCK 4

/* Ask _gmp_rand for 32 bits per call unless that's more than a limb can hold.
   Thus, we get the same random number sequence in the common cases.
   FIXME: We should always generate the same random number sequence!  */
#if GMP_NUMB_BITS < 32
#define BITS_PER_RANDCALL GMP_NUMB_BITS
#else
#define BITS_PER_RANDCALL 32
#endif

static void
gmp_rrandomb (mp_ptr rp, gmp_randstate_t rstate, unsigned long int nbits)
{
  int nb;
  int bit_pos;			/* bit number of least significant bit where
				   next bit field to be inserted */
  mp_size_t ri;			/* index in rp */
  mp_limb_t ran, ranm;		/* buffer for random bits */
  mp_limb_t acc;		/* accumulate output random data here */
  int ran_nbits;		/* number of valid bits in ran */

  ran_nbits = 0;
  bit_pos = (nbits - 1) % GMP_NUMB_BITS;
  ri = (nbits - 1) / GMP_NUMB_BITS;

  acc = 0;
  while (ri >= 0)
    {
      if (ran_nbits < LOGBITS_PER_BLOCK + 1)
	{
	  _gmp_rand (&ranm, rstate, BITS_PER_RANDCALL);
	  ran = ranm;
	  ran_nbits = BITS_PER_RANDCALL;
	}

      nb = (ran >> 1) % (1 << LOGBITS_PER_BLOCK) + 1;
      if ((ran & 1) != 0)
	{
	  /* Generate a string of nb ones.  */
	  if (nb > bit_pos)
	    {
	      rp[ri--] = acc | (((mp_limb_t) 2 << bit_pos) - 1);
	      bit_pos += GMP_NUMB_BITS;
	      bit_pos -= nb;
	      acc = ((~(mp_limb_t) 1) << bit_pos) & GMP_NUMB_MASK;
	    }
	  else
	    {
	      bit_pos -= nb;
	      acc |= (((mp_limb_t) 2 << nb) - 2) << bit_pos;
	    }
	}
      else
	{
	  /* Generate a string of nb zeroes.  */
	  if (nb > bit_pos)
	    {
	      rp[ri--] = acc;
	      acc = 0;
	      bit_pos += GMP_NUMB_BITS;
	    }
	  bit_pos -= nb;
	}
      ran_nbits -= LOGBITS_PER_BLOCK + 1;
      ran >>= LOGBITS_PER_BLOCK + 1;
    }
}
