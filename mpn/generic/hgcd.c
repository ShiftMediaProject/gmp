/* hgcd.c.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2003, 2004, 2005, 2008 Free Software Foundation, Inc.

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

/* For input of size n, matrix elements are of size at most ceil(n/2)
   - 1, but we need one limb extra. */
void
mpn_hgcd_matrix_init (struct hgcd_matrix *M, mp_size_t n, mp_ptr p)
{
  mp_size_t s = (n+1)/2;
  M->alloc = s;
  M->n = 1;
  MPN_ZERO (p, 4 * s);
  M->p[0][0] = p;
  M->p[0][1] = p + s;
  M->p[1][0] = p + 2 * s;
  M->p[1][1] = p + 3 * s;
  M->tp = p + 4*s;

  M->p[0][0][0] = M->p[1][1][0] = 1;
}

/* Updated column COL, adding in column (1-COL). */
static void
hgcd_matrix_update_1 (struct hgcd_matrix *M, unsigned col)
{
  mp_limb_t c0, c1;
  ASSERT (col < 2);

  c0 = mpn_add_n (M->p[0][col], M->p[0][0], M->p[0][1], M->n);
  c1 = mpn_add_n (M->p[1][col], M->p[1][0], M->p[1][1], M->n);

  M->p[0][col][M->n] = c0;
  M->p[1][col][M->n] = c1;

  M->n += (c0 | c1) != 0;
  ASSERT (M->n < M->alloc);
}

/* Updated column COL, adding in column Q * (1-COL). */
static void
hgcd_matrix_update_q (struct hgcd_matrix *M, mp_srcptr qp, mp_size_t qn,
		      unsigned col)
{
  ASSERT (col < 2);

  if (qn == 1)
    {
      mp_limb_t q = qp[0];
      mp_limb_t c0, c1;

      c0 = mpn_addmul_1 (M->p[0][col], M->p[0][1-col], M->n, q);
      c1 = mpn_addmul_1 (M->p[1][col], M->p[1][1-col], M->n, q);

      M->p[0][col][M->n] = c0;
      M->p[1][col][M->n] = c1;

      M->n += (c0 | c1) != 0;
    }
  else
    {
      unsigned row;

      /* Carries for the unlikely case that we get both high words
	 from the multiplication and carries from the addition. */
      mp_limb_t c[2];
      mp_size_t n;

      /* The matrix will not necessarily grow in size by qn, so we
	 need normalization in order not to overflow M. */

      for (n = M->n; n + qn > M->n; n--)
	{
	  ASSERT (n > 0);
	  if (M->p[0][1-col][n-1] > 0 || M->p[1][1-col][n-1] > 0)
	    break;
	}
      
      ASSERT (qn + n <= M->alloc);

      for (row = 0; row < 2; row++)
	{
	  if (qn <= n)
	    mpn_mul (M->tp, M->p[row][1-col], n, qp, qn);
	  else
	    mpn_mul (M->tp, qp, qn, M->p[row][1-col], n);

	  ASSERT (n + qn >= M->n);
	  c[row] = mpn_add (M->p[row][col], M->tp, n + qn, M->p[row][col], M->n);
	}
      if (c[0] | c[1])
	{
	  M->n = n + qn + 1;
	  M->p[0][col][n-1] = c[0];
	  M->p[1][col][n-1] = c[1];
	}
      else
	{
	  n += qn;
	  n -= (M->p[0][col][n-1] | M->p[1][col][n-1]) == 0;
	  if (n > M->n)
	    M->n = n;
	}
    }

  ASSERT (M->n < M->alloc);
}

/* Multiply M by M1 from the right. Since the M1 elements fit in
   GMP_NUMB_BITS - 1 bits, M grows by at most one limb. Needs
   temporary space M->n */
static void
hgcd_matrix_mul_1 (struct hgcd_matrix *M, const struct hgcd_matrix1 *M1)
{
  unsigned row;
  mp_limb_t grow;
  for (row = 0, grow = 0; row < 2; row++)
    {
      mp_limb_t c0, c1;

      /* Compute (u, u') <-- (r00 u + r10 u', r01 u + r11 u') as

	  t   = u
	  u  *= r00
	  u  += r10 * u'
	  u' *= r11
	  u' += r01 * t
      */

      MPN_COPY (M->tp, M->p[row][0], M->n);
      c0 =     mpn_mul_1 (M->p[row][0], M->p[row][0], M->n, M1->u[0][0]);
      c0 += mpn_addmul_1 (M->p[row][0], M->p[row][1], M->n, M1->u[1][0]);
      M->p[row][0][M->n] = c0;

      c1 =     mpn_mul_1 (M->p[row][1], M->p[row][1], M->n, M1->u[1][1]);
      c1 += mpn_addmul_1 (M->p[row][1], M->tp,        M->n, M1->u[0][1]);
      M->p[row][1][M->n] = c1;

      grow |= (c0 | c1);
    }
  M->n += (grow != 0);
  ASSERT (M->n < M->alloc);
}

/* Perform a few steps, using some of mpn_hgcd2, subtraction and
   division. Reduces the size by almost one limb or more, but never
   below the given size s. Return new size for a and b, or 0 if no
   more steps are possible. M = NULL is allowed, if M is not needed.
   FIXME: I don't think there's any need to allow M == NULL.

   Needs temporary space for division, n + 1 limbs, and for
   hgcd_mul_matrix1_vector, n limbs. */
mp_size_t
mpn_hgcd_step (mp_size_t n, mp_ptr ap, mp_ptr bp, mp_size_t s,
	       struct hgcd_matrix *M, mp_ptr tp)
{
  struct hgcd_matrix1 M1;
  mp_limb_t mask;
  mp_limb_t ah, al, bh, bl;
  mp_size_t an, bn, qn;
  mp_ptr qp;
  mp_ptr rp;
  int col;

  ASSERT (n > s);

  mask = ap[n-1] | bp[n-1];
  ASSERT (mask > 0);

  if (n == s + 1)
    {
      if (mask < 4)
	goto subtract;

      ah = ap[n-1]; al = ap[n-2];
      bh = bp[n-1]; bl = bp[n-2];
    }
  else if (mask & GMP_NUMB_HIGHBIT)
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

  /* Try an mpn_hgcd2 step */
  if (mpn_hgcd2 (ah, al, bh, bl, &M1))
    {
      /* Multiply M <- M * M1 */
      if (M)
	hgcd_matrix_mul_1 (M, &M1);

      /* Multiply M1^{-1} (a;b) */
      return mpn_hgcd_mul_matrix1_inverse_vector (&M1, n, ap, bp, tp);
    }

 subtract:
  /* There are two ways in which mpn_hgcd2 can fail. Either one of ah and
     bh was too small, or ah, bh were (almost) equal. Perform one
     subtraction step (for possible cancellation of high limbs),
     followed by one division. */

  /* Since we must ensure that #(a-b) > s, we handle cancellation of
     high limbs explicitly up front. (FIXME: Or is it better to just
     subtract, normalize, and use an addition to undo if it turns out
     the the difference is too small?) */
  for (an = n; an > s; an--)
    if (ap[an-1] != bp[an-1])
      break;

  if (an == s)
    return 0;

  /* Maintain a > b. When needed, swap a and b, and let col keep track
     of how to update M. */
  if (ap[an-1] > bp[an-1])
    {
      /* a is largest. In the subtraction step, we need to update
	 column 1 of M */
      col = 1;
    }
  else
    {
      MP_PTR_SWAP (ap, bp);
      col = 0;
    }

  bn = n;
  MPN_NORMALIZE (bp, bn);  
  if (bn <= s)
    return 0;
  
  /* We have #a, #b > s. When is it possible that #(a-b) < s? For
     cancellation to happen, the numbers must be of the form

       a = x + 1, 0,            ..., 0,            al
       b = x    , GMP_NUMB_MAX, ..., GMP_NUMB_MAX, bl

     where al, bl denotes the least significant k limbs. If al < bl,
     then #(a-b) < k, and if also high(al) != 0, high(bl) != GMP_NUMB_MAX,
     then #(a-b) = k. If al >= bl, then #(a-b) = k + 1. */

  if (ap[an-1] == bp[an-1] + 1)
    {
      mp_size_t k;
      int c;
      for (k = an-1; k > s; k--)
	if (ap[k-1] != 0 || bp[k-1] != GMP_NUMB_MAX)
	  break;

      MPN_CMP (c, ap, bp, k);
      if (c < 0)
	{
	  mp_limb_t cy;
	  
	  /* The limbs from k and up are cancelled. */
	  if (k == s)
	    return 0;
	  cy = mpn_sub_n (ap, ap, bp, k);
	  ASSERT (cy == 1);
	  an = k;
	}
      else
	{
	  ASSERT_NOCARRY (mpn_sub_n (ap, ap, bp, k));
	  ap[k] = 1;
	  an = k + 1;
	}
    }
  else
    ASSERT_NOCARRY (mpn_sub_n (ap, ap, bp, an));
  
  ASSERT (an > s);
  ASSERT (ap[an-1] > 0);
  ASSERT (bn > s);
  ASSERT (bp[bn-1] > 0);
  
  if (M)
    hgcd_matrix_update_1 (M, col);

  if (an < bn)
    {
      MPN_PTR_SWAP (ap, an, bp, bn);
      col ^= 1;
    }
  else if (an == bn)
    {
      int c;
      MPN_CMP (c, ap, bp, an);
      if (c < 0)
	{
	  MP_PTR_SWAP (ap, bp);
	  col ^= 1;
	}
    }

  /* Divide a / b. Store first the quotient (qn limbs) and then the
     remainder (bn limbs) starting at tp. */
  qn = an + 1 - bn;
  qp = tp;
  rp = tp + qn;

  /* FIXME: We could use an approximate division, that may return a
     too small quotient, and only guarantess that the size of r is
     almost the size of b. */
  mpn_tdiv_qr (qp, rp, 0, ap, an, bp, bn);
  qn -= (qp[qn -1] == 0);

  /* Normalize remainder */
  an = bn;
  for ( ; an > s; an--)
    if (rp[an-1] > 0)
      break;

  if (an > s)
    /* Include leading zero limbs */
    MPN_COPY (ap, rp, bn);
  else
    {
      /* Quotient is too large */
      mp_limb_t cy;

      cy = mpn_add (ap, bp, bn, rp, an);

      if (cy > 0)
	{
	  ASSERT (bn < n);
	  ap[bn] = cy;
	  bp[bn] = 0;
	  bn++;
	}

      MPN_DECR_U (qp, qn, 1);
      qn -= (qp[qn-1] == 0);
    }

  if (qn > 0 && M)
    hgcd_matrix_update_q (M, qp, qn, col);

  return bn;
}

/* Reduces a,b until |a-b| fits in n/2 + 1 limbs. Constructs matrix M
   with elements of size at most (n+1)/2 - 1. Returns new size of a,
   b, or zero if no reduction is possible. */
mp_size_t
mpn_hgcd_lehmer (mp_ptr ap, mp_ptr bp, mp_size_t n,
		 struct hgcd_matrix *M, mp_ptr tp)
{
  mp_size_t s = n/2 + 1;
  mp_size_t nn;

  ASSERT (n > s);
  ASSERT (ap[n-1] > 0 || bp[n-1] > 0);

  nn = mpn_hgcd_step (n, ap, bp, s, M, tp);
  if (!nn)
    return 0;

  for (;;)
    {
      n = nn;
      ASSERT (n > s);
      nn = mpn_hgcd_step (n, ap, bp, s, M, tp);
      if (!nn )
	return n;      
    }
}

/* Computes r = u0 x0 + u1 x1. rn is the size of the result area, and
   must be at least one larger than the result. Needs temporary space
   of the same size. Returns size of result. Non-normalized inputs,
   including zero, are allowed.

   No overlap between input and output is allowed, since rp is used
   for temporary storage. */

/* FIXME: What's a good name for this function? */
mp_limb_t
mpn_hgcd_addmul2_n (mp_ptr rp, mp_size_t rn,
		    mp_srcptr u0, mp_srcptr u1, mp_size_t un,
		    mp_srcptr x0, mp_srcptr x1, mp_size_t xn,
		    mp_ptr tp)
{
  mp_size_t u0n, u1n, x0n, x1n;
  
  mp_size_t t0n, t1n;
  mp_size_t n;
  mp_limb_t cy;

  /* FIXME: Could omit normalization in the common case that
     un + xn <= rn. */
  u0n = u1n = un;
  MPN_NORMALIZE (u0, u0n);
  MPN_NORMALIZE (u1, u1n);

  x0n = x1n = xn;
  MPN_NORMALIZE (x0, x0n);
  MPN_NORMALIZE (x1, x1n);

  /* t0 = u0 * x0 is stored at rp, and t1 = u1 * x1 at tp. */
  t0n = u0n + x0n;
  t1n = u1n + x1n;

  ASSERT (t0n <= rn);
  ASSERT (t1n <= rn);

  /* Handle zero cases */
  if (x0n == 0 || u0n == 0)
    {
      if (x1n == 0 || u1n == 0)
	return 0;

      tp = rp;
      t0n = 0;
    }
  else if (x1n == 0 || u1n == 0)
    {
      ASSERT (x0n > 0);
      ASSERT (u0n > 0);

      t1n = 0;
    }

  if (t0n > 0)
    {
      if (x0n >= u0n)
	mpn_mul (rp, x0, x0n, u0, u0n);
      else
	mpn_mul (rp, u0, u0n, x0, x0n);

      t0n -= (rp[t0n-1] == 0);
    }

  if (t1n > 0)
    {
      if (x1n >= u1n)
	mpn_mul (tp, x1, x1n, u1, u1n);
      else
	mpn_mul (tp, u1, u1n, x1, x1n);

      t1n -= (tp[t1n-1] == 0);
    }
  else
    return t0n;

  if (t0n == 0)
    return t1n;

  if (t0n >= t1n)
    {
      cy = mpn_add (rp, rp, t0n, tp, t1n);
      n = t0n;
    }
  else
    {
      cy = mpn_add (rp, tp, t1n, rp, t0n);
      n = t1n;
    }
  rp[n] = cy;
  n += (cy != 0);

  ASSERT (n < rn);

  return n;
}

/* Multiply M by M1 from the right. Needs 2*M->n temporary storage
   (and additionally uses M->tp). */
void
mpn_hgcd_matrix_mul (struct hgcd_matrix *M, const struct hgcd_matrix *M1,
		     mp_ptr tp)
{
  unsigned row;

  mp_ptr m00 = M1->p[0][0];
  mp_ptr m01 = M1->p[0][1];
  mp_ptr m10 = M1->p[1][0];
  mp_ptr m11 = M1->p[1][1];

  mp_size_t n;

  mp_ptr up = tp;
  mp_ptr vp = tp + M->n;

  /* About the new size of M:s elements. Since M1's diagonal elements
     are > 0, no element can decrease. The typical case is that the
     new elements are of size M->n + M1->n, one limb more or less. But
     it may be smaller, consider for example (1,x;0,1)(1,y;0,1) =
     (1,x+y;0,1), where size is increased by a single bit no matter how
     large x is. So to avoid writing past the end of M, we need to
     normalize the numbers. */

  /* FIXME: The case (1,x;0,1)(1,y;0,1) with large x and y can't
     happen in hgcd, since it corresponds to a quotient q >= x + y which
     is split in the middle. See if we can get by without
     normalization? */

  /* FIXME: This function could be sped up a little using Strassen
     multiplication, and in FFT multiplication range, it could be sped
     up quite a lot using invariance. */

  n = 0;
  for (row = 0; row < 2; row++)
    {
      mp_size_t un, vn;
      mp_size_t nn;

      MPN_COPY (up, M->p[row][0], M->n);
      MPN_COPY (vp, M->p[row][1], M->n);

      /* Compute (u', v') = (u,v) (r00, r01; r10, r11)
	 = (r00 u + r10 v, r01 u + r11 v) */

      nn = mpn_hgcd_addmul2_n (M->p[row][0], M->alloc, up, vp, M->n, m00, m10, M1->n, M->tp);
      if (nn > n)
	n = nn;
      nn = mpn_hgcd_addmul2_n (M->p[row][1], M->alloc, up, vp, M->n, m01, m11, M1->n, M->tp);
      if (nn > n)
	n = nn;
    }
  ASSERT (n < M->alloc);
  M->n = n;
}

/* Multiplies the least significant p limbs of (a;b) by M^-1.
   Temporary space needed: 2 * (p + M->n)*/
mp_size_t
mpn_hgcd_matrix_adjust (struct hgcd_matrix *M,
			mp_size_t n, mp_ptr ap, mp_ptr bp,
			mp_size_t p, mp_ptr tp)
{
  /* M^-1 (a;b) = (r11, -r01; -r10, r00) (a ; b)
     = (r11 a - r01 b; - r10 a + r00 b */

  mp_ptr t0 = tp;
  mp_ptr t1 = tp + p + M->n;
  mp_limb_t ah, bh;
  mp_limb_t cy;

  ASSERT (p + M->n  < n);

  /* First compute the two values depending on a, before overwriting a */

  if (M->n >= p)
    {
      mpn_mul (t0, M->p[1][1], M->n, ap, p);
      mpn_mul (t1, M->p[1][0], M->n, ap, p);
    }
  else
    {
      mpn_mul (t0, ap, p, M->p[1][1], M->n);
      mpn_mul (t1, ap, p, M->p[1][0], M->n);
    }

  /* Update a */
  MPN_COPY (ap, t0, p);
  ah = mpn_add (ap + p, ap + p, n - p, t0 + p, M->n);

  if (M->n >= p)
    mpn_mul (t0, M->p[0][1], M->n, bp, p);
  else
    mpn_mul (t0, bp, p, M->p[0][1], M->n);

  cy = mpn_sub (ap, ap, n, t0, p + M->n);
  ASSERT (cy <= ah);
  ah -= cy;

  /* Update b */
  if (M->n >= p)
    mpn_mul (t0, M->p[0][0], M->n, bp, p);
  else
    mpn_mul (t0, bp, p, M->p[0][0], M->n);

  MPN_COPY (bp, t0, p);
  bh = mpn_add (bp + p, bp + p, n - p, t0 + p, M->n);
  cy = mpn_sub (bp, bp, n, t1, p + M->n);
  ASSERT (cy <= bh);
  bh -= cy;

  if (ah > 0 || bh > 0)
    {
      ap[n] = ah;
      bp[n] = bh;
      n++;
    }
  else
    {
      /* The subtraction can reduce the size by at most one limb. */
      if (ap[n-1] == 0 && bp[n-1] == 0)
	n--;
    }
  ASSERT (ap[n-1] > 0 || bp[n-1] > 0);
  return n;  
}

#if 0
/* From the old hgcd code. Use something similar to compute the
 * recursion depth below? */
unsigned mpn_hgcd_max_recursion (mp_size_t n) { int count;

  count_leading_zeros (count, (mp_limb_t)
		       (1 + n / (HGCD_SCHOENHAGE_THRESHOLD  - 5)));

  return GMP_LIMB_BITS - count;
}
#endif

/* Size analysis for hgcd:

   For the recursive calls, we have n1 <= ceil(n / 2). Then the
   storage need is determined by the storage for the recursive call
   computing M1, and hgcd_matrix_adjust and hgcd_matrix_mul calls that use M1
   (after this, the storage needed for M1 can be recycled).

   Let S(r) denote the required storage. For M1 we need 5 * ceil(n1/2)
   = 5 * ceil(n/4), and for the hgcd_matrix_adjust call, we need n + 2. In
   total, 5 * ceil(n/4) + n + 2 <= 9 ceil(n/4) + 2.

   For the recursive call, we need S(n1) = S(ceil(n/2)).

   S(n) <= 9*ceil(n/4) + 2 + S(ceil(n/2))
        <= 9*(ceil(n/4) + ... + ceil(n/2^(1+k))) + 2k + S(ceil(n/2^k))
        <= 9*(2 ceil(n/4) + k) + 2k + S(n/2^k)   
	<= 18 ceil(n/4) + 11k + S(n/2^k)
	
*/

mp_size_t
mpn_hgcd_itch (mp_size_t n)
{
  unsigned k;
  mp_size_t nn;

  /* Inefficient way to almost compute
     log_2(n/HGCD_BASE_THRESHOLD) */
  for (k = 0, nn = n;
       ABOVE_THRESHOLD (nn, HGCD_THRESHOLD);
       nn = (nn + 1) / 2)
    k++;

  if (k == 0)
    return MPN_HGCD_LEHMER_ITCH (n);

  return 18 * ((n+3) / 4) + 11 * k
    + MPN_HGCD_LEHMER_ITCH (HGCD_THRESHOLD);
}

/* Reduces a,b until |a-b| fits in n/2 + 1 limbs. Constructs matrix M
   with elements of size at most (n+1)/2 - 1. Returns new size of a,
   b, or zero if no reduction is possible. */

mp_size_t
mpn_hgcd (mp_ptr ap, mp_ptr bp, mp_size_t n,
	  struct hgcd_matrix *M, mp_ptr tp)
{
  mp_size_t s = n/2 + 1;
  mp_size_t n2 = (3*n)/4 + 1;
  
  mp_size_t p, nn;
  int success = 0;

  if (n <= s)
    /* Happens when n <= 2, a fairly uninteresting case but exercised
       by the random inputs of the testsuite. */
    return 0;

  ASSERT ((ap[n-1] | bp[n-1]) > 0);

  ASSERT ((n+1)/2 - 1 < M->alloc);

  if (BELOW_THRESHOLD (n, HGCD_THRESHOLD))
    return mpn_hgcd_lehmer (ap, bp, n, M, tp);

  p = n/2;
  nn = mpn_hgcd (ap + p, bp + p, n - p, M, tp);
  if (nn > 0)
    {
      /* Needs 2*(p + M->n) <= 2*(floor(n/2) + ceil(n/2) - 1)
	 = 2 (n - 1) */
      n = mpn_hgcd_matrix_adjust (M, p + nn, ap, bp, p, tp);
      success = 1;
    }
  while (n > n2)
    {
      /* Needs n + 1 storage */
      nn = mpn_hgcd_step (n, ap, bp, s, M, tp);
      if (!nn)
	return success ? n : 0;
      n = nn;
      success = 1;
    }

  if (n > s + 2)
    {
      struct hgcd_matrix M1;
      mp_size_t scratch;

      p = 2*s - n + 1;
      scratch = MPN_HGCD_MATRIX_INIT_ITCH (n-p);

      mpn_hgcd_matrix_init(&M1, n - p, tp);
      nn = mpn_hgcd (ap + p, bp + p, n - p, &M1, tp + scratch);
      if (nn > 0)
	{
	  /* Needs 2 (p + M->n) <= 2 (2*s - n2 + 1 + n2 - s - 1)
	     = 2*s <= 2*(floor(n/2) + 1) <= n + 2. */
	  n = mpn_hgcd_matrix_adjust (&M1, p + nn, ap, bp, p, tp + scratch);
	  /* Needs M->n <= n2 - s - 1 < n/4 */
	  mpn_hgcd_matrix_mul (M, &M1, tp + scratch);
	  success = 1;
	}
    }

  /* This really is the base case */
  for (;;)
    {
      /* Needs s+3 < n */
      nn = mpn_hgcd_step (n, ap, bp, s, M, tp);
      if (!nn)
	return success ? n : 0;

      n = nn;
      success = 1;
    } 
}
