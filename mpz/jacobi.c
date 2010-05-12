/* mpz_jacobi, mpz_legendre, mpz_kronecker -- mpz/mpz Jacobi symbols.

Copyright 2000, 2001, 2002, 2005, 2010 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
for more details.

You should have received a copy of the GNU Lesser General Public License along
with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/* This code does triple duty as mpz_jacobi, mpz_legendre and
   mpz_kronecker. For ABI compatibility, the link symbol is
   __gmpz_jacobi, not __gmpz_kronecker, even though the latter would
   be more logical.

   mpz_jacobi could assume b is odd, but the improvements from that seem
   small compared to other operations, and anything significant should be
   checked at run-time since we'd like odd b to go fast in mpz_kronecker
   too.

   mpz_legendre could assume b is an odd prime, but knowing this doesn't
   present any obvious benefits.  Result 0 wouldn't arise (unless "a" is a
   multiple of b), but the checking for that takes little time compared to
   other operations.

   Enhancements:

   mpn_bdiv_qr should be used instead of mpn_tdiv_qr.

   Current code uses the binary algorithm for the smallest sizes, then
   Lehmer.  It could stright-forwardly be made subquadratic by using
   hgcd in the same way as mpn_gcd.  */

/* (a/2) = -1 iff a = 3 or a = -3 (mod 8), and (2/b) = -1 iff b = 3 or
   b = - 3 (mod 8). Note that when this is used, we have already
   excluded the case that a and both have a common factor of two. */

#define STRIP_TWOS(bit1, twos, other_low, p, n, low)		   \
  do {								   \
    JACOBI_STRIP_LOW_ZEROS ((bit1), (other_low), (p), (n), (low)); \
    count_trailing_zeros ((twos), (low));			   \
    (bit1) ^= JACOBI_TWOS_U_BIT1((twos), (other_low));		   \
    (low) >>= (twos);						   \
    if ((n) > 1 && (twos) > 0)					   \
      {								   \
	mp_limb_t __second = (p)[1];				   \
	(low) |= __second << (GMP_NUMB_BITS - (twos));		   \
	if ((n) == 2 && (__second >> (twos) == 0))		   \
	  n = 1;						   \
      }								   \
  } while(0)

int
mpz_jacobi (mpz_srcptr a, mpz_srcptr b)
{
  mp_srcptr  asrcp, bsrcp;
  mp_size_t  asize, bsize, itch;
  mp_limb_t  alow, blow;
  mp_ptr     ap, bp, scratch;
  unsigned   atwos, btwos;
  int        result_bit1;
  int        res;
  TMP_DECL;

  asize = SIZ(a);
  asrcp = PTR(a);
  alow = asrcp[0];

  bsize = SIZ(b);
  bsrcp = PTR(b);
  blow = bsrcp[0];

  /* The MPN jacobi functions requies positive a and b, and b odd. So
     we must to handle the cases of a or b zero, then signs, and then
     the case of even b.
   
     In addition, to reduce the number of cases, we arrange so that a
     is odd, and asize >= bsize. */

  if ( (((alow | blow) & 1) == 0))
    /* Common factor of 2 ==> (a/b) = 0 */
    return 0;

  if (bsize == 0)
    /* (a/0) = [ a = 1 or a = -1 ] */
    return JACOBI_LS0 (alow, asize);

  if (asize == 0)
    /* (0/b) = [ b = 1 or b = - 1 ] */
    return JACOBI_0LS (blow, bsize);

  if (bsize < 0)
    {
      /* (a/-1) = -1 if a < 0, +1 if a >= 0 */
      result_bit1 = (asize < 0) << 1;
      bsize = -bsize;
    }
  else
    result_bit1 = 0;

  STRIP_TWOS (result_bit1, btwos, alow, bsrcp, bsize, blow);

  if (asize < 0)
    {
      /* (-1/b) = -1 iff b = 3 (mod 4) */
      result_bit1 ^= JACOBI_N1B_BIT1(blow);
      asize = -asize;
    }
  
  STRIP_TWOS(result_bit1, atwos, blow, asrcp, asize, alow);

  /* Both numbers odd, so arrange so that asize >= bsize */
  if (asize < bsize)
    {
      unsigned t;
      MPN_SRCPTR_SWAP (asrcp, asize, bsrcp, bsize);
      MP_LIMB_T_SWAP (alow, blow);

      t = atwos;
      atwos = btwos;
      btwos = t;

      result_bit1 ^= JACOBI_RECIP_UU_BIT1 (alow, blow);
    }

  if (bsize == 1)
    {
      if (blow == 1)
	return JACOBI_BIT1_TO_PN (result_bit1);

      if (asize > 1)
	{
	  /* We work with {asrcp, asize} mod b, hence throw away the
	     old alow and undo the shift right by atwos. */
	  result_bit1 ^= JACOBI_TWOS_U_BIT1 (atwos, blow);

	  JACOBI_MOD_OR_MODEXACT_1_ODD (result_bit1, alow, asrcp, asize, blow);
	}

      return mpn_jacobi_base (alow, blow, result_bit1);
    }

  TMP_MARK;

  itch = 3*bsize;

  if (asize > bsize)
    {
      if (btwos > 0)
	{
	  if (asize >= 2 * bsize)
	    itch = asize + bsize + 1;
	}
      else if (atwos > 0)
	{
	  if (asize >= 2*bsize)
	    itch = 2*asize - bsize + 1;
	}
      else
	{
	  if (asize >= 3*bsize)
	    itch = asize + 1;
	}
    }

  ap = TMP_ALLOC_LIMBS (itch);
  bp = ap + bsize;
  scratch = bp + bsize;

  if (asize > bsize)
    {
      /* Do an initial divide. */
      if (btwos > 0)
	{
	  /* Result size: 2*bsize, extra: asize - bsize + 1 for
	     quotient, total: asize + bsize + 1. */
	  ASSERT (atwos == 0);

	  ASSERT_NOCARRY (mpn_rshift (bp, bsrcp, bsize, btwos));
	  bsize -= bp[bsize-1] == 0;

	  /* Note that if the shift eliminated the most significant
	     limb of b, the quotient gets one limb larger, but the
	     total storage needed for b and the quotient is unchanged.
	     To get sufficient space, we put the quotient at bp +
	     bsize rather than at scratch. */
	  mpn_tdiv_qr (bp + bsize, ap, 0, asrcp, asize, bp, bsize);
	}
      else
	{
	  if (atwos > 0)
	    {
	      /* Result size: bsize, extra: (asize - bsize) + (asize -
		 bsize + 1) for shifted value, and quotient, total: 2
		 asize - bsize + 1. */
	      ASSERT_NOCARRY (mpn_rshift (ap, asrcp, asize, atwos));
	      mpn_tdiv_qr (ap + asize, ap, 0, ap, asize, bsrcp, bsize);
	    }
	  else
	    /* Result size: bsize, extra: asize - bsize + 1 for
	       quotient, total asize + 1. */
	    mpn_tdiv_qr (bp, ap, 0, asrcp, asize, bsrcp, bsize);

	  MPN_COPY (bp, bsrcp, bsize);
	}
      alow = ap[0];
    }
  else
    {
      /* Result size: 2 * bsize, extra: 0. */
      if (atwos > 0)
	ASSERT_NOCARRY (mpn_rshift (ap, asrcp, asize, atwos));
      else
	MPN_COPY (ap, asrcp, asize);

      if (btwos > 0)
	ASSERT_NOCARRY (mpn_rshift (bp, bsrcp, bsize, btwos));
      else
	MPN_COPY (bp, bsrcp, bsize);

      bsize -= (ap[bsize-1] | bp[bsize-1]) == 0;
    }

  /* Scratch need: bsize */
  res = mpn_jacobi_lehmer (ap, bp, bsize,
			   mpn_jacobi_init (alow, blow, (result_bit1>>1) & 1),
			   scratch);

  TMP_FREE;
  return res;
}
