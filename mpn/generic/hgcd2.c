/* hgcd2.c

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 1996, 1998, 2000-2004, 2008, 2012, 2019 Free Software Foundation,
Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the GNU MP Library.  If not,
see https://www.gnu.org/licenses/.  */

#include "gmp-impl.h"
#include "longlong.h"

#ifndef HGCD2_DIV1_METHOD
#define HGCD2_DIV1_METHOD 3
#endif

#ifndef HGCD2_DIV2_METHOD
#define HGCD2_DIV2_METHOD 2
#endif

#if GMP_NAIL_BITS != 0
#error Nails not implemented
#endif

#if HAVE_NATIVE_mpn_div_11

#define div1 mpn_div_11
/* Single-limb division optimized for small quotients.
   Returned value holds d0 = r, d1 = q. */
mp_double_limb_t div1 (mp_limb_t, mp_limb_t);

#elif HGCD2_DIV1_METHOD == 1

static inline mp_double_limb_t
div1 (mp_limb_t n0, mp_limb_t d0)
{
  mp_double_limb_t res;
  res.d1 = n0 / d0;
  res.d0 = n0 - res.d1 * d0;

  return res;
}

#elif HGCD2_DIV1_METHOD == 2

static mp_double_limb_t
div1 (mp_limb_t n0, mp_limb_t d0)
{
  mp_double_limb_t res;
  int ncnt, dcnt, cnt;
  mp_limb_t q;
  mp_limb_t mask;

  ASSERT (n0 >= d0);

  count_leading_zeros (ncnt, n0);
  count_leading_zeros (dcnt, d0);
  cnt = dcnt - ncnt;

  d0 <<= cnt;

  q = -(mp_limb_t) (n0 >= d0);
  n0 -= d0 & q;
  d0 >>= 1;
  q = -q;

  while (--cnt >= 0)
    {
      mask = -(mp_limb_t) (n0 >= d0);
      n0 -= d0 & mask;
      d0 >>= 1;
      q = (q << 1) - mask;
    }

  res.d0 = n0;
  res.d1 = q;
  return res;
}

#elif HGCD2_DIV1_METHOD == 3

static inline mp_double_limb_t
div1 (mp_limb_t n0, mp_limb_t d0)
{
  mp_double_limb_t res;
  if (UNLIKELY ((d0 >> (GMP_LIMB_BITS - 3)) != 0)
      || UNLIKELY (n0 >= (d0 << 3)))
    {
      res.d1 = n0 / d0;
      res.d0 = n0 - res.d1 * d0;
    }
  else
    {
      mp_limb_t q, mask;

      d0 <<= 2;

      mask = -(mp_limb_t) (n0 >= d0);
      n0 -= d0 & mask;
      q = 4 & mask;

      d0 >>= 1;
      mask = -(mp_limb_t) (n0 >= d0);
      n0 -= d0 & mask;
      q += 2 & mask;

      d0 >>= 1;
      mask = -(mp_limb_t) (n0 >= d0);
      n0 -= d0 & mask;
      q -= mask;

      res.d0 = n0;
      res.d1 = q;
    }
  return res;
}
#else
#error Unknown HGCD2_DIV1_METHOD
#endif

#if HAVE_NATIVE_mpn_div_22

#define div2 mpn_div_22
/* Two-limb division optimized for small quotients.  */
mp_limb_t div2 (mp_ptr, mp_limb_t, mp_limb_t, mp_limb_t, mp_limb_t);

#elif HGCD2_DIV2_METHOD == 1

static mp_limb_t
div2 (mp_ptr rp,
      mp_limb_t n1, mp_limb_t n0,
      mp_limb_t d1, mp_limb_t d0)
{
  mp_double_limb_t rq = div1 (n1, d1);
  if (UNLIKELY (rq.d1 > d1))
    {
      mp_limb_t n2, q, t1, t0;
      int c;

      /* Normalize */
      count_leading_zeros (c, d1);
      ASSERT (c > 0);

      n2 = n1 >> (GMP_LIMB_BITS - c);
      n1 = (n1 << c) | (n0 >> (GMP_LIMB_BITS - c));
      n0 <<= c;
      d1 = (d1 << c) | (d0 >> (GMP_LIMB_BITS - c));
      d0 <<= c;

      udiv_qrnnd (q, n1, n2, n1, d1);
      umul_ppmm (t1, t0, q, d0);
      if (t1 > n1 || (t1 == n1 && t0 > n0))
	{
	  ASSERT (q > 0);
	  q--;
	  sub_ddmmss (t1, t0, t1, t0, d1, d0);
	}
      sub_ddmmss (n1, n0, n1, n0, t1, t0);

      /* Undo normalization */
      rp[0] = (n0 >> c) | (n1 << (GMP_LIMB_BITS - c));
      rp[1] = n1 >> c;

      return q;
    }
  else
    {
      mp_limb_t q, t1, t0;
      n1 = rq.d0;
      q = rq.d1;
      umul_ppmm (t1, t0, q, d0);
      if (UNLIKELY (t1 >= n1) && (t1 > n1 || t0 > n0))
	{
	  ASSERT (q > 0);
	  q--;
	  sub_ddmmss (t1, t0, t1, t0, d1, d0);
	}
      sub_ddmmss (rp[1], rp[0], n1, n0, t1, t0);
      return q;
    }
}

#elif HGCD2_DIV2_METHOD == 2

/* Bit-wise div2. Relies on fast count_leading_zeros. */
static mp_limb_t
div2 (mp_ptr rp,
      mp_limb_t nh, mp_limb_t nl,
      mp_limb_t dh, mp_limb_t dl)
{
  mp_limb_t q = 0;
  int ncnt;
  int dcnt;

  count_leading_zeros (ncnt, nh);
  count_leading_zeros (dcnt, dh);
  dcnt -= ncnt;

  dh = (dh << dcnt) + (dl >> 1 >> (GMP_LIMB_BITS - 1 - dcnt));
  dl <<= dcnt;

  do
    {
      mp_limb_t mask;
      q <<= 1;
      if (UNLIKELY (nh == dh))
	mask = -(nl >= dl);
      else
	mask = -(nh > dh);

      q -= mask;

      sub_ddmmss (nh, nl, nh, nl, mask & dh, mask & dl);

      dl = (dh << (GMP_LIMB_BITS - 1)) | (dl >> 1);
      dh = dh >> 1;
    }
  while (dcnt--);

  rp[0] = nl;
  rp[1] = nh;

  return q;
}
#else
#error Unknown HGCD2_DIV2_METHOD
#endif

/* Reduces a,b until |a-b| (almost) fits in one limb + 1 bit. Constructs
   matrix M. Returns 1 if we make progress, i.e. can perform at least
   one subtraction. Otherwise returns zero. */

/* FIXME: Possible optimizations:

   The div2 function starts with checking the most significant bit of
   the numerator. We can maintained normalized operands here, call
   hgcd with normalized operands only, which should make the code
   simpler and possibly faster.

   Experiment with table lookups on the most significant bits.

   This function is also a candidate for assembler implementation.
*/
int
mpn_hgcd2 (mp_limb_t ah, mp_limb_t al, mp_limb_t bh, mp_limb_t bl,
	   struct hgcd_matrix1 *M)
{
  mp_limb_t u00, u01, u10, u11;

  if (ah < 2 || bh < 2)
    return 0;

  if (ah > bh || (ah == bh && al > bl))
    {
      sub_ddmmss (ah, al, ah, al, bh, bl);
      if (ah < 2)
	return 0;

      u00 = u01 = u11 = 1;
      u10 = 0;
    }
  else
    {
      sub_ddmmss (bh, bl, bh, bl, ah, al);
      if (bh < 2)
	return 0;

      u00 = u10 = u11 = 1;
      u01 = 0;
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
	      goto done;
	    }
	  q++;
	  u01 += q * u00;
	  u11 += q * u10;
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
	      goto done;
	    }
	  q++;
	  u00 += q * u01;
	  u10 += q * u11;
	}
    }

  /* NOTE: Since we discard the least significant half limb, we don't get a
     truly maximal M (corresponding to |a - b| < 2^{GMP_LIMB_BITS +1}). */
  /* Single precision loop */
  for (;;)
    {
      ASSERT (ah >= bh);

      ah -= bh;
      if (ah < (CNST_LIMB (1) << (GMP_LIMB_BITS / 2 + 1)))
	break;

      if (ah <= bh)
	{
	  /* Use q = 1 */
	  u01 += u00;
	  u11 += u10;
	}
      else
	{
	  mp_double_limb_t rq = div1 (ah, bh);
	  mp_limb_t q = rq.d1;
	  ah = rq.d0;

	  if (ah < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2 + 1)))
	    {
	      /* A is too small, but q is correct. */
	      u01 += q * u00;
	      u11 += q * u10;
	      break;
	    }
	  q++;
	  u01 += q * u00;
	  u11 += q * u10;
	}
    subtract_a1:
      ASSERT (bh >= ah);

      bh -= ah;
      if (bh < (CNST_LIMB (1) << (GMP_LIMB_BITS / 2 + 1)))
	break;

      if (bh <= ah)
	{
	  /* Use q = 1 */
	  u00 += u01;
	  u10 += u11;
	}
      else
	{
	  mp_double_limb_t rq = div1 (bh, ah);
	  mp_limb_t q = rq.d1;
	  bh = rq.d0;

	  if (bh < (CNST_LIMB(1) << (GMP_LIMB_BITS / 2 + 1)))
	    {
	      /* B is too small, but q is correct. */
	      u00 += q * u01;
	      u10 += q * u11;
	      break;
	    }
	  q++;
	  u00 += q * u01;
	  u10 += q * u11;
	}
    }

 done:
  M->u[0][0] = u00; M->u[0][1] = u01;
  M->u[1][0] = u10; M->u[1][1] = u11;

  return 1;
}

/* Sets (r;b) = (a;b) M, with M = (u00, u01; u10, u11). Vector must
 * have space for n + 1 limbs. Uses three buffers to avoid a copy*/
mp_size_t
mpn_hgcd_mul_matrix1_vector (const struct hgcd_matrix1 *M,
			     mp_ptr rp, mp_srcptr ap, mp_ptr bp, mp_size_t n)
{
  mp_limb_t ah, bh;

  /* Compute (r,b) <-- (u00 a + u10 b, u01 a + u11 b) as

     r  = u00 * a
     r += u10 * b
     b *= u11
     b += u01 * a
  */

#if HAVE_NATIVE_mpn_addaddmul_1msb0
  ah = mpn_addaddmul_1msb0 (rp, ap, bp, n, M->u[0][0], M->u[1][0]);
  bh = mpn_addaddmul_1msb0 (bp, bp, ap, n, M->u[1][1], M->u[0][1]);
#else
  ah =     mpn_mul_1 (rp, ap, n, M->u[0][0]);
  ah += mpn_addmul_1 (rp, bp, n, M->u[1][0]);

  bh =     mpn_mul_1 (bp, bp, n, M->u[1][1]);
  bh += mpn_addmul_1 (bp, ap, n, M->u[0][1]);
#endif
  rp[n] = ah;
  bp[n] = bh;

  n += (ah | bh) > 0;
  return n;
}
