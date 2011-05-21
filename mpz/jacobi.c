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

*/

int
mpz_jacobi (mpz_srcptr a, mpz_srcptr b)
{
  mp_srcptr  asrcp, bsrcp;
  mp_size_t  asize, bsize, n, itch;
  mp_limb_t  alow, blow;
  mp_ptr     ap, bp, scratch;
  unsigned   btwos;
  int        result_bit1;
  unsigned   bits;
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
  */

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

  JACOBI_STRIP_LOW_ZEROS (result_bit1, alow, bsrcp, bsize, blow);

  count_trailing_zeros (btwos, blow);
  result_bit1 ^= JACOBI_TWOS_U_BIT1(btwos, alow);
  blow >>= btwos;

  if (bsize > 1 && btwos > 0)
    {
      mp_limb_t second = bsrcp[1];
      blow |= second << (GMP_NUMB_BITS - btwos);
      if (bsize == 2 && (second >> btwos) == 0)
	bsize = 1;
    }

  if (asize < 0)
    {
      /* (-1/b) = -1 iff b = 3 (mod 4) */
      result_bit1 ^= JACOBI_N1B_BIT1(blow);
      asize = -asize;
    }
  
  JACOBI_STRIP_LOW_ZEROS (result_bit1, blow, asrcp, asize, alow);

  if (bsize == 1)
    {
      if (blow == 1)
	return JACOBI_BIT1_TO_PN (result_bit1);

      if (asize > 1)
	JACOBI_MOD_OR_MODEXACT_1_ODD (result_bit1, alow, asrcp, asize, blow);

      return mpn_jacobi_base (alow, blow, result_bit1);
    }

  if (asize == 1)
    {
      /* Logic copied from mpz_ui_kronecker */
      if (alow == 1)
	return JACOBI_BIT1_TO_PN (result_bit1);  /* (1/b)=1 */
	
      if (btwos > 0)
	{
	  /* Only bit 1 of blow is used below. */
	  if (btwos == GMP_NUMB_BITS - 1)
	    blow = bsrcp[1] << 1;
	  else
	    blow >> btwos;
	}

      else if ( (alow & 1) == 0)
	{
	  unsigned atwos;
	  count_trailing_zeros (atwos, alow);
	  alow >>= atwos;
	  result_bit1 ^= JACOBI_TWOS_U_BIT1 (atwos, blow);
	}

      if (alow == 1)
	return JACOBI_BIT1_TO_PN (result_bit1);  /* (1/b)=1 */

      /* (a/b*2^n) = (b*2^n / a) * RECIP(a,b) */
      result_bit1 ^= JACOBI_RECIP_UU_BIT1 (alow, blow);
      JACOBI_MOD_OR_MODEXACT_1_ODD (result_bit1, blow, bsrcp, bsize, alow);
      return mpn_jacobi_base (blow, alow, result_bit1);
    }

  bits = mpn_jacobi_init (alow, blow, (result_bit1>>1) & 1);

  /* Allocation strategy: When one operand is much larger than the
     other, we currently have to allocate space for the entire
     quotient, even though we need juste the lowest few bits. But we
     at least avoid allocating a copy of th larger input.

     We put the reduction of the larger operand first in the scratch
     area, followed by an area that holds first the quotient, and then
     the working copy of the smaller operand. */

  if (asize > bsize)
    {
      n = bsize;
      
      if (asize >= 2*bsize)
	itch = asize + 1;
      else
	itch = 2*bsize;
    }
  else
    {
      n = asize;

      if (bsize >= 2*asize)
	itch = bsize + 1;
      else
	itch = 2*asize;
    }

  TMP_MARK;

  scratch = TMP_ALLOC_LIMBS (itch);

  if (n < asize)
    {
      mp_limb_t q0;
      ap = scratch;
      bp = scratch + n;

      mpn_tdiv_qr (bp, ap, 0, asrcp, asize, bsrcp, n);
      q0 = bp[0];

      if (btwos > 0)
	{
	  ASSERT_NOCARRY (mpn_rshift (bp, bsrcp, n, btwos));
	  n -= (bp[n-1] | ap[n-1]) == 0;

	  /* We have reduced a -= q * 2^k b */
	  q0 <<= btwos;
	}
      else
	MPN_COPY (bp, bsrcp, n);
      
      bits = mpn_jacobi_update (bits, 1, q0 & 3);
      if (mpn_zero_p (ap, n))
	{
	  /* FIXME: n > 1 always? */
	  if (n > 1 || bp[0] != 1)
	    {
	      TMP_FREE;
	      return 0;
	    }

	  TMP_FREE;
	  return mpn_jacobi_finish (bits);
	}
    }
  else if (n < bsize)
    {
      mp_limb_t q0;
      mp_limb_t cy;
      bp = scratch;
      ap = scratch + n;

      mpn_tdiv_qr (ap, bp, 0, bsrcp, bsize, asrcp, n);
      q0 = ap[0];

      if (btwos > 0)
	{
	  /* Let b be the correctly shifted, odd, value, and b' = 2^k
	     b (k = btwos). We have divided

	       b' = q' a + r'

	     Let q' = 2^k q + ql, then we can recover the correct
	     division as

	       b = q a + r

	     where the remainder is

	       r = (ql a + r')/2^k
	   */
	  mp_limb_t ql, hi;

	  ql = q0 & ((CNST_LIMB(1) << btwos) - 1);
	  q0 = (q0 >> btwos) | (ap[1] << (GMP_LIMB_BITS - btwos));
	  hi = mpn_addmul_1 (bp, asrcp, n, ql);

	  ASSERT_NOCARRY (mpn_rshift (bp, bp, n, btwos));
	  bp[n-1] |= hi << (GMP_LIMB_BITS - btwos);
	}

      bits = mpn_jacobi_update (bits, 0, q0 & 3);

      if (mpn_zero_p (bp, n))
	{
	  TMP_FREE;

	  /* FIXME: n > 1 always? */
	  if (n > 1 || asrcp[0] != 1)
	    return 0;
	  else
	    return mpn_jacobi_finish (bits);
	}
      
      MPN_COPY (ap, asrcp, n);
    }
  else
    {
      ap = scratch;
      bp = scratch + n;

      MPN_COPY (ap, asrcp, n);
      if (btwos > 0)
	ASSERT_NOCARRY (mpn_rshift (bp, bsrcp, n, btwos));
      else
	MPN_COPY (bp, bsrcp, n);
    }

  res = mpn_jacobi_n (ap, bp, n, bits);

  TMP_FREE;
  return res;
}
