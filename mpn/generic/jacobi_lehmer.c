/* jacobi_lehmer.c

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 1996, 1998, 2000, 2001, 2002, 2003, 2004, 2008, 2010 Free Software
Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library.  If not, see http://www.gnu.org/licenses/.  */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

/* FIXME: Bad name for this symbol. */
#ifndef JACOBI_2_METHOD
#define JACOBI_2_METHOD 2
#endif

#if GMP_NAIL_BITS > 0
#error Nails not supported.
#endif

/* FIXME: Duplicated in hgcd2.c. Should move to gmp-impl.h, and
   possibly be renamed. */
static inline mp_limb_t
div1 (mp_ptr rp,
      mp_limb_t n0,
      mp_limb_t d0)
{
  mp_limb_t q = 0;

  if ((mp_limb_signed_t) n0 < 0)
    {
      int cnt;
      for (cnt = 1; (mp_limb_signed_t) d0 >= 0; cnt++)
	{
	  d0 = d0 << 1;
	}

      q = 0;
      while (cnt)
	{
	  q <<= 1;
	  if (n0 >= d0)
	    {
	      n0 = n0 - d0;
	      q |= 1;
	    }
	  d0 = d0 >> 1;
	  cnt--;
	}
    }
  else
    {
      int cnt;
      for (cnt = 0; n0 >= d0; cnt++)
	{
	  d0 = d0 << 1;
	}

      q = 0;
      while (cnt)
	{
	  d0 = d0 >> 1;
	  q <<= 1;
	  if (n0 >= d0)
	    {
	      n0 = n0 - d0;
	      q |= 1;
	    }
	  cnt--;
	}
    }
  *rp = n0;
  return q;
}

/* Two-limb division optimized for small quotients.  */
static inline mp_limb_t
div2 (mp_ptr rp,
      mp_limb_t nh, mp_limb_t nl,
      mp_limb_t dh, mp_limb_t dl)
{
  mp_limb_t q = 0;

  if ((mp_limb_signed_t) nh < 0)
    {
      int cnt;
      for (cnt = 1; (mp_limb_signed_t) dh >= 0; cnt++)
	{
	  dh = (dh << 1) | (dl >> (GMP_LIMB_BITS - 1));
	  dl = dl << 1;
	}

      while (cnt)
	{
	  q <<= 1;
	  if (nh > dh || (nh == dh && nl >= dl))
	    {
	      sub_ddmmss (nh, nl, nh, nl, dh, dl);
	      q |= 1;
	    }
	  dl = (dh << (GMP_LIMB_BITS - 1)) | (dl >> 1);
	  dh = dh >> 1;
	  cnt--;
	}
    }
  else
    {
      int cnt;
      for (cnt = 0; nh > dh || (nh == dh && nl >= dl); cnt++)
	{
	  dh = (dh << 1) | (dl >> (GMP_LIMB_BITS - 1));
	  dl = dl << 1;
	}

      while (cnt)
	{
	  dl = (dh << (GMP_LIMB_BITS - 1)) | (dl >> 1);
	  dh = dh >> 1;
	  q <<= 1;
	  if (nh > dh || (nh == dh && nl >= dl))
	    {
	      sub_ddmmss (nh, nl, nh, nl, dh, dl);
	      q |= 1;
	    }
	  cnt--;
	}
    }

  rp[0] = nl;
  rp[1] = nh;

  return q;
}


/* Schönhage's rules:
 *
 * Assume r0 = r1 q1 + r2, with r0 odd, and r1 = q2 r2 + r3
 *
 * If r1 is odd, then
 *
 *   (r1 | r0) = s(r1, r0) (r0 | r1) = s(r1, r0) (r2, r1)
 *
 * where s(x,y) = (-1)^{(x-1)(y-1)/4} = (-1)^[x = y = 3 (mod 4)].
 *
 * If r1 is even, r2 must be odd. We have
 *
 *   (r1 | r0) = (r1 - r0 | r0) = (-1)^(r0-1)/2 (r0 - r1 | r0)
 *             = (-1)^(r0-1)/2 s(r0, r0 - r1) (r0 | r0 - r1)
 *             = (-1)^(r0-1)/2 s(r0, r0 - r1) (r1 | r0 - r1)
 *
 * Now, if r1 = 0 (mod 4), then the sign factor is +1, and repeating
 * q1 times gives
 *
 *   (r1 | r0) = (r1 | r2) = (r3 | r2)
 *
 * On the other hand, if r1 = 2 (mod 4), the sign factor is
 * (-1)^{(r0-1)/2}, and repeating q1 times gives the exponent
 *
 *   (r0-1)/2 + (r0-r1-1)/2 + ... + (r0 - (q1-1) r1)/2
 *   = q1 (r0-1)/2 + q1 (q1-1)/2
 *
 * and we can summarize the even case as
 *
 *   (r1 | r0) = t(r1, r0, q1) (r3 | r2)
 *
 * where t(x,y,q) = (-1)^{[x = 2 (mod 4)] (q(y-1)/2 + y(q-1)/2)}
 *
 * What about termination? The remainder sequence ends with (0|1) = 1
 * (or (0 | r) = 0 if r != 1). What are the possible cases? If r1 is
 * odd, r2 may be zero. If r1 is even, then r2 = r0 - q1 r1 is odd and
 * hence non-zero. We may have r3 = r1 - q2 r2 = 0.
 *
 * Examples: (11|15) = - (15|11) = - (4|11)
 *            (4|11) =    (4| 3) =   (1| 3)
 *            (1| 3) = (3|1) = (0|1) = 1
 *
 *             (2|7) = (2|1) = (0|1) = 1
 *
 * Detail:     (2|7) = (2-7|7) = (-1|7)(5|7) = -(7|5) = -(2|5)
 *             (2|5) = (2-5|5) = (-1|5)(3|5) =  (5|3) =  (2|3)
 *             (2|3) = (2-3|3) = (-1|3)(1|3) = -(3|1) = -(2|1)
 *
 */

/* In principle, the state consists of four variables: e (one bit), a,
   b (two bits each), d (one bit). Collected factors are (-1)^e. a and
   b are the least significant bits of the current remainders. d
   (denominator) is 0 if we're currently subtracting multiplies of a
   from b, and 1 if we're subtracting b from a.

   e is stored in the least significant bit, while a, b and d are
   coded as only 13 distinct values in bits 1-4, according to the
   following table. For rows not mentioning d, the value is either
   implied, or it doesn't matter. */

#if WANT_ASSERT
static const struct
{
  unsigned char a;
  unsigned char b;
} decode_table[13] = {
  /*  0 */ { 0, 1 },
  /*  1 */ { 0, 3 },
  /*  2 */ { 1, 1 },
  /*  3 */ { 1, 3 },
  /*  4 */ { 2, 1 },
  /*  5 */ { 2, 3 },
  /*  6 */ { 3, 1 },
  /*  7 */ { 3, 3 }, /* d = 1 */
  /*  8 */ { 1, 0 },
  /*  9 */ { 1, 2 },
  /* 10 */ { 3, 0 },
  /* 11 */ { 3, 2 },
  /* 12 */ { 3, 3 }, /* d = 0 */
};
#define JACOBI_A(bits) (decode_table[(bits)>>1].a)
#define JACOBI_B(bits) (decode_table[(bits)>>1].b)
#endif /* WANT_ASSERT */

const unsigned char jacobi_table[208] = {
#include "jacobitab.h"
};

/* Computes (a / b) where b is odd, and a and b are otherwise arbitrary
   two-limb numbers. */
#if JACOBI_2_METHOD == 1
static int
mpn_jacobi_2 (mp_srcptr ap, mp_srcptr bp, unsigned bit)
{
  mp_limb_t ah, al, bh, bl;
  int c;

  al = ap[0];
  ah = ap[1];
  bl = bp[0];
  bh = bp[1];

  ASSERT (bl & 1);

  bl = ((bh << (GMP_NUMB_BITS - 1)) & GMP_NUMB_MASK) | (bl >> 1);
  bh >>= 1;

  if ( (bh | bl) == 0)
    return 1 - 2*(bit & 1);
  
  if ( (ah | al) == 0)
    return 0;

  if (al == 0)
    {
      al = ah;
      ah = 0;
      bit ^= GMP_NUMB_BITS & (bl ^ (bl >> 1));
    }
  count_trailing_zeros (c, al);
  bit ^= c & (bl ^ (bl >> 1));
  
  c++;
  if (UNLIKELY (c == GMP_NUMB_BITS))
    {
      al = ah;
      ah = 0;
    }
  else
    {
      al = ((ah << (GMP_NUMB_BITS - c)) & GMP_NUMB_MASK) | (al >> c);
      ah >>= c;
    }
  while ( (ah | bh) > 0)
    {
      mp_limb_t th, tl;
      mp_limb_t bgta;

      sub_ddmmss (th, tl, ah, al, bh, bl);
      if ( (tl | th) == 0)
	return 0;

      bgta = LIMB_HIGHBIT_TO_MASK (th);

      /* If b > a, invoke reciprocity */
      bit ^= (bgta & al & bl);

      /* b <-- min (a, b) */
      add_ssaaaa (bh, bl, bh, bl, th & bgta, tl & bgta);

      if ( (bh | bl) == 0)
	return 1 - 2*(bit & 1);

      /* a <-- |a - b| */
      al = (bgta ^ tl) - bgta;
      ah = (bgta ^ th);

      if (UNLIKELY (al == 0))
	{
	  /* If b > a, al == 0 implies that we have a carry to
	     propagate. */
	  al = ah - bgta;
	  ah = 0;
	  bit ^= GMP_NUMB_BITS & (bl ^ (bl >> 1));
	}
      count_trailing_zeros (c, al);
      c++;
      bit ^= c & (bl ^ (bl >> 1));

      if (UNLIKELY (c == GMP_NUMB_BITS))
	{
	  al = ah;
	  ah = 0;
	}
      else
	{
	  al = ((ah << (GMP_NUMB_BITS - c)) & GMP_NUMB_MASK) | (al >> c);
	  ah >>= c;
	}
    }

  ASSERT (bl > 0);
  
  while ( (al | bl) & GMP_LIMB_HIGHBIT)
    {
      /* Need an extra comparison to get the mask. */
      mp_limb_t t = al - bl;
      mp_limb_t bgta = - (bl > al);

      if (t == 0)
	return 0;

      /* If b > a, invoke reciprocity */
      bit ^= (bgta & al & bl);
      
      /* b <-- min (a, b) */
      bl += (bgta & t);

      /* a <-- |a - b| */
      al = (t ^ bgta) - bgta;

      /* Number of trailing zeros is the same no matter if we look at
       * t or a, but using t gives more parallelism. */
      count_trailing_zeros (c, t);
      c ++;
      /* (2/b) = -1 if b = 3 or 5 mod 8 */
      bit ^= c & (bl ^ (bl >> 1));

      if (UNLIKELY (c == GMP_NUMB_BITS))
	return 1 - 2*(bit & 1);

      al >>= c;
    }

  /* Here we have a little impedance mismatch. Better to inline it? */
  return mpn_jacobi_base (2*al+1, 2*bl+1, bit << 1);
}
#elif JACOBI_2_METHOD == 2
static int
mpn_jacobi_2 (mp_srcptr ap, mp_srcptr bp, unsigned bit)
{
  mp_limb_t ah, al, bh, bl;
  int c;

  al = ap[0];
  ah = ap[1];
  bl = bp[0];
  bh = bp[1];

  ASSERT (bl & 1);

  /* Use bit 1. */
  bit <<= 1;

  if (bh == 0 && bl == 1)
    /* (a|1) = 1 */
    return 1 - (bit & 2);

  if (al == 0)
    {
      if (ah == 0)
	/* (0|b) = 0, b > 1 */
	return 0;

      count_trailing_zeros (c, ah);
      bit ^= ((GMP_NUMB_BITS + c) << 1) & (bl ^ (bl >> 1));

      al = bl;
      bl = ah >> c;

      if (bl == 1)
	/* (1|b) = 1 */
	return 1 - (bit & 2);

      ah = bh;

      bit ^= al & bl;

      goto b_reduced;
    }
  if ( (al & 1) == 0)
    {
      count_trailing_zeros (c, al);

      al = ((ah << (GMP_NUMB_BITS - c)) & GMP_NUMB_MASK) | (al >> c);
      ah >>= c;
      bit ^= (c << 1) & (bl ^ (bl >> 1));
    }
  if (ah == 0)
    {
      if (bh > 0)
	{
	  bit ^= al & bl;
	  MP_LIMB_T_SWAP (al, bl);
	  ah = bh;
	  goto b_reduced;
	}
      goto ab_reduced;
    }

  while (bh > 0)
    {
      /* Compute (a|b) */
      while (ah > bh)
	{
	  sub_ddmmss (ah, al, ah, al, bh, bl);
	  if (al == 0)
	    {
	      count_trailing_zeros (c, ah);
	      bit ^= ((GMP_NUMB_BITS + c) << 1) & (bl ^ (bl >> 1));
	  
	      al = bl;
	      bl = ah >> c;
	      ah = bh;
	      goto b_reduced;
	    }
	  count_trailing_zeros (c, al);
	  bit ^= (c << 1) & (bl ^ (bl >> 1));
	  al = ((ah << (GMP_NUMB_BITS - c)) & GMP_NUMB_MASK) | (al >> c);
	  ah >>= c;

	}
      if (ah == bh)
	goto cancel_hi;
      
      if (ah == 0)
	{
	  bit ^= al & bl;
	  MP_LIMB_T_SWAP (al, bl);
	  ah = bh;
	  break;
	}
      
      bit ^= al & bl;

      /* Compute (b|a) */
      while (bh > ah)
	{
	  sub_ddmmss (bh, bl, bh, bl, ah, al);
	  if (bl == 0)
	    {
	      count_trailing_zeros (c, bh);
	      bit ^= ((GMP_NUMB_BITS + c) << 1) & (al ^ (al >> 1));

	      bl = bh >> c;
	      bit ^= al & bl;
	      goto b_reduced;
	    }
	  count_trailing_zeros (c, bl);
	  bit ^= (c << 1) & (al ^ (al >> 1));
	  bl = ((bh << (GMP_NUMB_BITS - c)) & GMP_NUMB_MASK) | (bl >> c);
	  bh >>= c;
	}
      bit ^= al & bl;

      /* Compute (a|b) */
      if (ah == bh)
	{
	cancel_hi:
	  if (al < bl)
	    {
	      MP_LIMB_T_SWAP (al, bl);
	      bit ^= al & bl;
	    }
	  al -= bl;
	  if (al == 0)
	    return 0;

	  count_trailing_zeros (c, al);
	  bit ^= (c << 1) & (bl ^ (bl >> 1));
	  al >>= c;

	  if (al == 1)
	    return 1 - (bit & 2);

	  MP_LIMB_T_SWAP (al, bl);
	  bit ^= al & bl;
	  break;
	}
    }

 b_reduced:
  /* Compute (a|b), with b a single limb. */
  ASSERT (bl > 1);

  while (ah > 0)
    {
      ah -= (al < bl);
      al -= bl;
      if (al == 0)
	{
	  if (ah == 0)
	    return 0;
	  count_leading_zeros (c, ah);
	  bit ^= ((GMP_NUMB_BITS + c) << 1) & (bl ^ (bl >> 1));
	  al = ah >> c;
	  goto ab_reduced;
	}
      count_trailing_zeros (c, al);

      al = ((ah << (GMP_NUMB_BITS - c)) & GMP_NUMB_MASK) | (al >> c);
      ah >>= c;
      bit ^= (c << 1) & (bl ^ (bl >> 1));      
    }
 ab_reduced:
  ASSERT (bl > 0);
  return mpn_jacobi_base (al, bl, bit);
}
#else
#error Unsupported value for JACOBI_2_METHOD
#endif

static int
mpn_hgcd2_jacobi (mp_limb_t ah, mp_limb_t al, mp_limb_t bh, mp_limb_t bl,
		  struct hgcd_matrix1 *M, unsigned *bitsp)
{
  mp_limb_t u00, u01, u10, u11;
  unsigned bits = *bitsp;

  if (ah < 2 || bh < 2)
    return 0;

  if (ah > bh || (ah == bh && al > bl))
    {
      sub_ddmmss (ah, al, ah, al, bh, bl);
      if (ah < 2)
	return 0;

      u00 = u01 = u11 = 1;
      u10 = 0;
      bits = mpn_jacobi_update (bits, 1, 1);
    }
  else
    {
      sub_ddmmss (bh, bl, bh, bl, ah, al);
      if (bh < 2)
	return 0;

      u00 = u10 = u11 = 1;
      u01 = 0;
      bits = mpn_jacobi_update (bits, 0, 1);
    }

  if (ah < bh)
    goto subtract_a;

  for (;;)
    {
      ASSERT (ah >= bh);
      if (ah == bh)
	goto done;

      if (ah < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2)))
	{
	  ah = (ah << (GMP_LIMB_BITS / 2) ) + (al >> (GMP_LIMB_BITS / 2));
	  bh = (bh << (GMP_LIMB_BITS / 2) ) + (bl >> (GMP_LIMB_BITS / 2));

	  break;
	}

      /* Subtract a -= q b, and multiply M from the right by (1 q ; 0
	 1), affecting the second column of M. */
      ASSERT (ah > bh);
      sub_ddmmss (ah, al, ah, al, bh, bl);

      if (ah < 2)
	goto done;

      if (ah <= bh)
	{
	  /* Use q = 1 */
	  u01 += u00;
	  u11 += u10;
	  bits = mpn_jacobi_update (bits, 1, 1);
	}
      else
	{
	  mp_limb_t r[2];
	  mp_limb_t q = div2 (r, ah, al, bh, bl);
	  al = r[0]; ah = r[1];
	  if (ah < 2)
	    {
	      /* A is too small, but q is correct. */
	      u01 += q * u00;
	      u11 += q * u10;
	      bits = mpn_jacobi_update (bits, 1, q & 3);
	      goto done;
	    }
	  q++;
	  u01 += q * u00;
	  u11 += q * u10;
	  bits = mpn_jacobi_update (bits, 1, q & 3);
	}
    subtract_a:
      ASSERT (bh >= ah);
      if (ah == bh)
	goto done;

      if (bh < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2)))
	{
	  ah = (ah << (GMP_LIMB_BITS / 2) ) + (al >> (GMP_LIMB_BITS / 2));
	  bh = (bh << (GMP_LIMB_BITS / 2) ) + (bl >> (GMP_LIMB_BITS / 2));

	  goto subtract_a1;
	}

      /* Subtract b -= q a, and multiply M from the right by (1 0 ; q
	 1), affecting the first column of M. */
      sub_ddmmss (bh, bl, bh, bl, ah, al);

      if (bh < 2)
	goto done;

      if (bh <= ah)
	{
	  /* Use q = 1 */
	  u00 += u01;
	  u10 += u11;
	  bits = mpn_jacobi_update (bits, 0, 1);
	}
      else
	{
	  mp_limb_t r[2];
	  mp_limb_t q = div2 (r, bh, bl, ah, al);
	  bl = r[0]; bh = r[1];
	  if (bh < 2)
	    {
	      /* B is too small, but q is correct. */
	      u00 += q * u01;
	      u10 += q * u11;
	      bits = mpn_jacobi_update (bits, 0, q & 3);
	      goto done;
	    }
	  q++;
	  u00 += q * u01;
	  u10 += q * u11;
	  bits = mpn_jacobi_update (bits, 0, q & 3);
	}
    }

  /* NOTE: Since we discard the least significant half limb, we don't
     get a truly maximal M (corresponding to |a - b| <
     2^{GMP_LIMB_BITS +1}). */
  /* Single precision loop */
  for (;;)
    {
      ASSERT (ah >= bh);
      if (ah == bh)
	break;

      ah -= bh;
      if (ah < (CNST_LIMB (1) << (GMP_LIMB_BITS / 2 + 1)))
	break;

      if (ah <= bh)
	{
	  /* Use q = 1 */
	  u01 += u00;
	  u11 += u10;
	  bits = mpn_jacobi_update (bits, 1, 1);
	}
      else
	{
	  mp_limb_t r;
	  mp_limb_t q = div1 (&r, ah, bh);
	  ah = r;
	  if (ah < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2 + 1)))
	    {
	      /* A is too small, but q is correct. */
	      u01 += q * u00;
	      u11 += q * u10;
	      bits = mpn_jacobi_update (bits, 1, q & 3);
	      break;
	    }
	  q++;
	  u01 += q * u00;
	  u11 += q * u10;
	  bits = mpn_jacobi_update (bits, 1, q & 3);
	}
    subtract_a1:
      ASSERT (bh >= ah);
      if (ah == bh)
	break;

      bh -= ah;
      if (bh < (CNST_LIMB (1) << (GMP_LIMB_BITS / 2 + 1)))
	break;

      if (bh <= ah)
	{
	  /* Use q = 1 */
	  u00 += u01;
	  u10 += u11;
	  bits = mpn_jacobi_update (bits, 0, 1);
	}
      else
	{
	  mp_limb_t r;
	  mp_limb_t q = div1 (&r, bh, ah);
	  bh = r;
	  if (bh < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2 + 1)))
	    {
	      /* B is too small, but q is correct. */
	      u00 += q * u01;
	      u10 += q * u11;
	      bits = mpn_jacobi_update (bits, 0, q & 3);
	      break;
	    }
	  q++;
	  u00 += q * u01;
	  u10 += q * u11;
	  bits = mpn_jacobi_update (bits, 0, q & 3);
	}
    }

 done:
  M->u[0][0] = u00; M->u[0][1] = u01;
  M->u[1][0] = u10; M->u[1][1] = u11;
  *bitsp = bits;

  return 1;
}


#define BITS_FAIL 31

/* If a common factor is found, returns zero and sets *BITSP to
 * BITS_FAIL. */
static mp_size_t
mpn_jacobi_subdiv_step (unsigned *bitsp,
			mp_ptr ap, mp_ptr bp, mp_size_t n, mp_ptr tp)
{
  mp_size_t an, bn;
  unsigned denominator;

  ASSERT (n > 0);
  ASSERT (ap[n-1] > 0 || bp[n-1] > 0);

  an = bn = n;
  MPN_NORMALIZE (ap, an);
  MPN_NORMALIZE (bp, bn);

  if (UNLIKELY (an == 0))
    {
      if (bn > 1 || bp[0] > 1)
	*bitsp = BITS_FAIL;
      return 0;
    }

  else if (UNLIKELY (bn == 0))
    {
      if (an > 1 || ap[0] > 1)
	*bitsp = BITS_FAIL;
      return 0;
    }
  /* Arrange so that a > b, subtract an -= bn, and maintain
     normalization. */
  denominator = 1;
  if (an < bn)
    {
      MPN_PTR_SWAP (ap, an, bp, bn);
      denominator = 0;
    }
  else if (an == bn)
    {
      int c;
      MPN_CMP (c, ap, bp, an);
      if (UNLIKELY (c == 0))
	{
	  if (an > 1 || ap[0] > 0)
	    *bitsp = BITS_FAIL;
	  return 0;
	}
      else if (c < 0)
	{
	  MP_PTR_SWAP (ap, bp);
	  denominator = 0;
	}
    }

  ASSERT_NOCARRY (mpn_sub (ap, ap, an, bp, bn));
  MPN_NORMALIZE (ap, an);
  ASSERT (an > 0);

  *bitsp = mpn_jacobi_update (*bitsp, denominator, 1);

  /* Arrange so that a > b, and divide a = q b + r */
  /* FIXME: an < bn happens when we have cancellation. If that is the
     common case, then we could reverse the roles of a and b to avoid
     the swap. */
  if (an < bn)
    {
      MPN_PTR_SWAP (ap, an, bp, bn);
      denominator ^= 1;
    }
  else if (an == bn)
    {
      int c;
      MPN_CMP (c, ap, bp, an);
      if (UNLIKELY (c == 0))
	{
	  if (an > 1 || ap[0] > 1)
	    *bitsp = BITS_FAIL;
	  return 0;
	}
      else if (c < 0)
	{
	  MP_PTR_SWAP (ap, bp);
	  denominator ^= 1;
	}
    }

  mpn_tdiv_qr (tp, ap, 0, ap, an, bp, bn);
  *bitsp = mpn_jacobi_update (*bitsp, denominator, tp[0] & 3);

  if (mpn_zero_p (ap, bn))
    {
      if (bn > 1 || bp[0] > 1)
	*bitsp = BITS_FAIL;
      return 0;
    }
  return bn;
}

/* Computes (a | b), where b is odd and normalized. */
int
mpn_jacobi_lehmer (mp_ptr ap, mp_ptr bp, mp_size_t n, unsigned bits, mp_ptr tp)
{
  ASSERT (n > 0);
  ASSERT (ap[n-1] > 0 || bp[n-1] > 0);
  ASSERT ((ap[0] | bp[0]) & 1);

  while (n > 2)
    {
      struct hgcd_matrix1 M;
      mp_limb_t ah, al, bh, bl;
      mp_limb_t mask;

      mask = ap[n-1] | bp[n-1];
      ASSERT (mask > 0);

      if (mask & GMP_NUMB_HIGHBIT)
	{
	  ah = ap[n-1]; al = ap[n-2];
	  bh = bp[n-1]; bl = bp[n-2];
	}
      else
	{
	  int shift;

	  count_leading_zeros (shift, mask);
	  ah = MPN_EXTRACT_NUMB (shift, ap[n-1], ap[n-2]);
	  al = MPN_EXTRACT_NUMB (shift, ap[n-2], ap[n-3]);
	  bh = MPN_EXTRACT_NUMB (shift, bp[n-1], bp[n-2]);
	  bl = MPN_EXTRACT_NUMB (shift, bp[n-2], bp[n-3]);
	}

      /* Try an mpn_nhgcd2 step */
      if (mpn_hgcd2_jacobi (ah, al, bh, bl, &M, &bits))
	{
	  n = mpn_matrix22_mul1_inverse_vector (&M, tp, ap, bp, n);
	  MP_PTR_SWAP (ap, tp);
	}
      else
	{
	  /* mpn_hgcd2 has failed. Then either one of a or b is very
	     small, or the difference is very small. Perform one
	     subtraction followed by one division. */
	  n = mpn_jacobi_subdiv_step (&bits, ap, bp, n, tp);
	  if (!n)
	    return bits == BITS_FAIL ? 0 : mpn_jacobi_finish (bits);
	}
    }
  ASSERT ( (ap[0] & 3) == JACOBI_A (bits));
  ASSERT ( (bp[0] & 3) == JACOBI_B (bits));

  if (bits >= 16)
    MP_PTR_SWAP (ap, bp);
        
  if (n == 1)
    {
      mp_limb_t al, bl;
      al = ap[0];
      bl = bp[0];
      
      if (bl == 1)
	return 1 - 2*(bits & 1);
      else
	return mpn_jacobi_base (al, bl, bits << 1);
    }

  else
    return mpn_jacobi_2 (ap, bp, bits & 1);
}
