/* hgcd.c.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2003, 2004, 2005, 2008, 2011 Free Software Foundation, Inc.

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


static void
hgcd_hook (void *p, mp_srcptr gp, mp_size_t gn,
	   mp_srcptr qp, mp_size_t qn, int d)
{
  ASSERT (gp == NULL);
  ASSERT (d >= 0);
  ASSERT (d <= 1);

  MPN_NORMALIZE (qp, qn);
  if (qn > 0)
    {
      struct hgcd_matrix *M = (struct hgcd_matrix *) p;
      /* NOTES: This is a bit ugly. A tp area is passed to
	 gcd_subdiv_step, which stores q at the start of that area. We
	 now use the rest. */
      mp_ptr tp = (mp_ptr) qp + qn;
      mpn_hgcd_matrix_update_q (M, qp, qn, d, tp);
    }
}

/* Perform a few steps, using some of mpn_hgcd2, subtraction and
   division. Reduces the size by almost one limb or more, but never
   below the given size s. Return new size for a and b, or 0 if no
   more steps are possible.

   If hgcd2 succeds, needs temporary space for hgcd_matrix_mul_1, M->n
   limbs, and hgcd_mul_matrix1_inverse_vector, n limbs. If hgcd2
   fails, needs space for the quotient, qn <= n - s + 1 limbs, for and
   hgcd_matrix_update_q, qn + (size of the appropriate column of M) <=
   resulting size of M.

   If N is the input size to the calling hgcd, then s = floor(N/2) +
   1, M->n < N, qn + matrix size <= n - s + 1 + n - s = 2 (n - s) + 1
   < N, so N is sufficient.
*/

static mp_size_t
hgcd_step (mp_size_t n, mp_ptr ap, mp_ptr bp, mp_size_t s,
	   struct hgcd_matrix *M, mp_ptr tp)
{
  struct hgcd_matrix1 M1;
  mp_limb_t mask;
  mp_limb_t ah, al, bh, bl;
  mp_size_t an, bn, qn;
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
      mpn_hgcd_matrix_mul_1 (M, &M1, tp);

      /* Can't swap inputs, so we need to copy. */
      MPN_COPY (tp, ap, n);
      /* Multiply M1^{-1} (a;b) */
      return mpn_matrix22_mul1_inverse_vector (&M1, ap, tp, bp, n);
    }

 subtract:

  return mpn_gcd_subdiv_step (ap, bp, n, s, hgcd_hook, M, tp);
}

/* Size analysis for hgcd:

   For the recursive calls, we have n1 <= ceil(n / 2). Then the
   storage need is determined by the storage for the recursive call
   computing M1, and hgcd_matrix_adjust and hgcd_matrix_mul calls that use M1
   (after this, the storage needed for M1 can be recycled).

   Let S(r) denote the required storage. For M1 we need 4 * (ceil(n1/2) + 1)
   = 4 * (ceil(n/4) + 1), for the hgcd_matrix_adjust call, we need n + 2,
   and for the hgcd_matrix_mul, we may need 3 ceil(n/2) + 8. In total,
   4 * ceil(n/4) + 3 ceil(n/2) + 12 <= 10 ceil(n/4) + 12.

   For the recursive call, we need S(n1) = S(ceil(n/2)).

   S(n) <= 10*ceil(n/4) + 12 + S(ceil(n/2))
	<= 10*(ceil(n/4) + ... + ceil(n/2^(1+k))) + 12k + S(ceil(n/2^k))
	<= 10*(2 ceil(n/4) + k) + 12k + S(ceil(n/2^k))
	<= 20 ceil(n/4) + 22k + S(ceil(n/2^k))
*/

mp_size_t
mpn_hgcd_itch (mp_size_t n)
{
  unsigned k;
  int count;
  mp_size_t nscaled;

  if (BELOW_THRESHOLD (n, HGCD_THRESHOLD))
    return n;

  /* Get the recursion depth. */
  nscaled = (n - 1) / (HGCD_THRESHOLD - 1);
  count_leading_zeros (count, nscaled);
  k = GMP_LIMB_BITS - count;

  return 20 * ((n+3) / 4) + 22 * k + HGCD_THRESHOLD;
}

/* Reduces a,b until |a-b| fits in n/2 + 1 limbs. Constructs matrix M
   with elements of size at most (n+1)/2 - 1. Returns new size of a,
   b, or zero if no reduction is possible. */

mp_size_t
mpn_hgcd (mp_ptr ap, mp_ptr bp, mp_size_t n,
	  struct hgcd_matrix *M, mp_ptr tp)
{
  mp_size_t s = n/2 + 1;

  mp_size_t nn;
  int success = 0;

  if (n <= s)
    /* Happens when n <= 2, a fairly uninteresting case but exercised
       by the random inputs of the testsuite. */
    return 0;

  ASSERT ((ap[n-1] | bp[n-1]) > 0);

  ASSERT ((n+1)/2 - 1 < M->alloc);

  if (ABOVE_THRESHOLD (n, HGCD_THRESHOLD))
    {
      mp_size_t n2 = (3*n)/4 + 1;
      mp_size_t p = n/2;

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
	  nn = hgcd_step (n, ap, bp, s, M, tp);
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
	      /* We always have max(M) > 2^{-(GMP_NUMB_BITS + 1)} max(M1) */
	      ASSERT (M->n + 2 >= M1.n);

	      /* Furthermore, assume M ends with a quotient (1, q; 0, 1),
		 then either q or q + 1 is a correct quotient, and M1 will
		 start with either (1, 0; 1, 1) or (2, 1; 1, 1). This
		 rules out the case that the size of M * M1 is much
		 smaller than the expected M->n + M1->n. */

	      ASSERT (M->n + M1.n < M->alloc);

	      /* Needs 2 (p + M->n) <= 2 (2*s - n2 + 1 + n2 - s - 1)
		 = 2*s <= 2*(floor(n/2) + 1) <= n + 2. */
	      n = mpn_hgcd_matrix_adjust (&M1, p + nn, ap, bp, p, tp + scratch);

	      /* We need a bound for of M->n + M1.n. Let n be the original
		 input size. Then

		 ceil(n/2) - 1 >= size of product >= M.n + M1.n - 2

		 and it follows that

		 M.n + M1.n <= ceil(n/2) + 1

		 Then 3*(M.n + M1.n) + 5 <= 3 * ceil(n/2) + 8 is the
		 amount of needed scratch space. */
	      mpn_hgcd_matrix_mul (M, &M1, tp + scratch);
	      success = 1;
	    }
	}
    }

  for (;;)
    {
      /* Needs s+3 < n */
      nn = hgcd_step (n, ap, bp, s, M, tp);
      if (!nn)
	return success ? n : 0;

      n = nn;
      success = 1;
    }
}
