/* hgcd_appr.c.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2011 Free Software Foundation, Inc.

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

/* Computes R -= A * B. Result must be non-negative. Normalized down
   to size an, and resulting size is returned. */
static mp_size_t
submul (mp_ptr rp, mp_size_t rn,
	mp_srcptr ap, mp_size_t an, mp_srcptr bp, mp_size_t bn)
{
  mp_ptr tp;
  TMP_DECL;

  ASSERT (bn > 0);
  ASSERT (an >= bn);
  ASSERT (rn >= an);
  ASSERT (an + bn <= rn + 1);
  
  TMP_MARK;
  tp = TMP_ALLOC_LIMBS (an + bn);

  mpn_mul (tp, ap, an, bp, bn);
  if (an + bn > rn)
    {
      ASSERT (tp[rn] == 0);
      bn--;
    }
  ASSERT_NOCARRY (mpn_sub (rp, rp, rn, tp, an + bn));
  TMP_FREE;

  while (rn > an && (rp[rn-1] == 0))
    rn--;

  return rn;
}

/* Computes (a, b)  <--  M^{-1} (a; b) */
/* FIXME:
    x Take scratch parameter, and figure out scratch need.

    x Use some fallback for small M->n?    
*/
static mp_size_t
hgcd_matrix_apply (const struct hgcd_matrix *M,
		   mp_ptr ap, mp_ptr bp,
		   mp_size_t n)
{
  mp_size_t an, bn, un, vn, nn;
  mp_size_t mn[2][2];
  mp_size_t modn;
  mp_ptr tp, sp, scratch;
  mp_limb_t cy;
  unsigned i, j;

  TMP_DECL;

  ASSERT ( (ap[n-1] | bp[n-1]) > 0);

  an = n;
  MPN_NORMALIZE (ap, an);
  bn = n;
  MPN_NORMALIZE (bp, bn);
  
  for (i = 0; i < 2; i++)
    for (j = 0; j < 2; j++)
      {
	mp_size_t k;
	k = M->n;
	MPN_NORMALIZE (M->p[i][j], k);
	mn[i][j] = k;
      }

  ASSERT (mn[0][0] > 0);
  ASSERT (mn[1][1] > 0);
  ASSERT ( (mn[0][1] | mn[1][0]) > 0);

  TMP_MARK;

  if (mn[0][1] == 0)
    {
      mp_size_t qn;
      
      /* A unchanged, M = (1, 0; q, 1) */
      ASSERT (mn[0][0] == 1);
      ASSERT (M->p[0][0][0] == 1);
      ASSERT (mn[1][1] == 1);
      ASSERT (M->p[1][1][0] == 1);

      /* Put B <-- B - q A */
      nn = submul (bp, bn, ap, an, M->p[1][0], mn[1][0]);
    }
  else if (mn[1][0] == 0)
    {
      /* B unchanged, M = (1, q; 0, 1) */
      ASSERT (mn[0][0] == 1);
      ASSERT (M->p[0][0][0] == 1);
      ASSERT (mn[1][1] == 1);
      ASSERT (M->p[1][1][0] == 1);

      /* Put A  <-- A - q * B */
      nn = submul (ap, an, bp, bn, M->p[0][1], mn[0][1]);      
    }
  else
    {
      /* A = m00 a + m01 b  ==> a <= A / m00, b <= A / m01.
	 B = m10 a + m11 b  ==> a <= B / m10, b <= B / m11. */
      un = MIN (an - mn[0][0], bn - mn[1][0]) + 1;
      vn = MIN (an - mn[0][1], bn - mn[1][1]) + 1;

      nn = MAX (un, vn);
      /* In the range of interest, mulmod_bnm1 should always beat mullo. */
      modn = mpn_mulmod_bnm1_next_size (nn + 1);

      scratch = TMP_ALLOC_LIMBS (mpn_mulmod_bnm1_itch (modn, modn, M->n));
      tp = TMP_ALLOC_LIMBS (modn);
      sp = TMP_ALLOC_LIMBS (modn);

      ASSERT (n <= 2*modn);

      if (n > modn)
	{
	  cy = mpn_add (ap, ap, modn, ap + modn, n - modn);
	  MPN_INCR_U (ap, modn, cy);

	  cy = mpn_add (bp, bp, modn, bp + modn, n - modn);
	  MPN_INCR_U (bp, modn, cy);

	  n = modn;
	}

      mpn_mulmod_bnm1 (tp, modn, ap, n, M->p[1][1], mn[1][1], scratch);
      mpn_mulmod_bnm1 (sp, modn, bp, n, M->p[0][1], mn[0][1], scratch);

      /* FIXME: Handle the small n case in some better way. */
      if (n + mn[1][1] < modn)
	MPN_ZERO (tp + n + mn[1][1], modn - n - mn[1][1]);
      if (n + mn[0][1] < modn)
	MPN_ZERO (sp + n + mn[0][1], modn - n - mn[0][1]);
  
      cy = mpn_sub_n (tp, tp, sp, modn);
      MPN_DECR_U (tp, modn, cy);

      ASSERT (mpn_zero_p (tp + nn, modn - nn));

      mpn_mulmod_bnm1 (sp, modn, ap, n, M->p[1][0], mn[1][0], scratch);
      MPN_COPY (ap, tp, nn);
      mpn_mulmod_bnm1 (tp, modn, bp, n, M->p[0][0], mn[0][0], scratch);

      if (n + mn[1][0] < modn)
	MPN_ZERO (sp + n + mn[1][0], modn - n - mn[1][0]);
      if (n + mn[0][0] < modn)
	MPN_ZERO (tp + n + mn[0][0], modn - n - mn[0][0]);

      cy = mpn_sub_n (tp, tp, sp, modn);
      MPN_DECR_U (tp, modn, cy);

      ASSERT (mpn_zero_p (tp + nn, modn - nn));
      MPN_COPY (bp, tp, nn);

      while ( (ap[nn-1] | bp[nn-1]) == 0)
	{
	  nn--;
	  ASSERT (nn > 0);
	}
    }
  TMP_FREE;

  return nn;
}

/* Identical to mpn_hgcd_itch. FIXME: Do we really need to add
   HGCD_THRESHOLD at the end? */
mp_size_t
mpn_hgcd_appr_itch (mp_size_t n)
{
  if (BELOW_THRESHOLD (n, HGCD_APPR_THRESHOLD))
    return n;
  else
    {
      unsigned k;
      int count;
      mp_size_t nscaled;

      /* Get the recursion depth. */
      nscaled = (n - 1) / (HGCD_APPR_THRESHOLD - 1);
      count_leading_zeros (count, nscaled);
      k = GMP_LIMB_BITS - count;

      return 20 * ((n+3) / 4) + 22 * k + HGCD_THRESHOLD;
    }
}

/* Destroys inputs. */
int
mpn_hgcd_appr (mp_ptr ap, mp_ptr bp, mp_size_t n,
	       struct hgcd_matrix *M, mp_ptr tp)
{
  mp_size_t s;
  mp_limb_t mask;
  int success = 0;

  ASSERT (n > 0);
  mask = ap[n-1] | bp[n-1];

  ASSERT (mask > 0);

  if (n <= 2)
    /* Implies s = n. A fairly uninteresting case but exercised by the
       random inputs of the testsuite. */
    return 0;

  ASSERT ((n+1)/2 - 1 < M->alloc);

  /* We aim for reduction of to GMP_NUMB_BITS * s bits. But each time
     we discard some of the least significant limbs, we must keep one
     additional bit to account for the truncation error. We maintain
     the GMP_NUMB_BITS * s - extra_bits as the current target size. */
     
  s = n/2 + 1;
  if (BELOW_THRESHOLD (n, HGCD_APPR_THRESHOLD))
    {
      unsigned extra_bits = 0;

      while (n > 2)
	{
	  mp_size_t nn;

	  ASSERT (n > s);
	  ASSERT (n <= 2*s);

	  nn = mpn_hgcd_step (n, ap, bp, s, M, tp);
	  if (!nn)
	    break;

	  n = nn;
	  success = 1;

	  /* We can truncate and discard the lower p bits whenever nbits <=
	     2*sbits - p. To account for the truncation error, we must
	     adjust

	     sbits <-- sbits + 1 - p,

	     rather than just sbits <-- sbits - p. This adjustment makes
	     the produced matrix sligthly smaller than it could be. */

	  if (GMP_NUMB_BITS * (n + 1) + 2 * extra_bits <= 2*GMP_NUMB_BITS * s)
	    {
	      mp_size_t p = (GMP_NUMB_BITS * (2*s - n) - 2*extra_bits) / GMP_NUMB_BITS;

	      if (extra_bits == 0)
		{
		  /* We cross a limb boundary and bump s. We can't do that
		     if the result is that it makes makes min(U, V)
		     smaller than 2^{GMP_NUMB_BITS} s. */
		  if (s + 1 == n
		      || mpn_zero_p (ap + s + 1, n - s - 1)
		      || mpn_zero_p (bp + s + 1, n - s - 1))
		    continue;

		  extra_bits = GMP_NUMB_BITS - 1;
		  s++;
		}
	      else
		{
		  extra_bits--;
		}

	      /* Drop the p least significant limbs */
	      ap += p; bp += p; n -= p; s -= p;
	    }
	}

      ASSERT (s > 0);

      if (extra_bits > 0)
	{
	  /* We can get here only of we have dropped at least one of the
	     least significant bits, so we can decrement ap and bp. We can
	     then shift left extra bits using mpn_shiftr. */
	  /* NOTE: In the unlikely case that n is large, it would be
	     preferable to do an initial subdiv step to reduce the size
	     before shifting, but that would mean daplicating
	     mpn_gcd_subdiv_step with a bit count rather than a limb
	     count. */
	  ap--; bp--;
	  ap[0] = mpn_rshift (ap+1, ap+1, n, GMP_NUMB_BITS - extra_bits);
	  bp[0] = mpn_rshift (bp+1, bp+1, n, GMP_NUMB_BITS - extra_bits);
	  n += (ap[n] | bp[n]) > 0;

	  ASSERT (success);

	  while (n > 2)
	    {
	      mp_size_t nn;

	      ASSERT (n > s);
	      ASSERT (n <= 2*s);

	      nn = mpn_hgcd_step (n, ap, bp, s, M, tp);
	  
	      if (!nn)
		return 1;

	      n = nn;
	    }
	}

      if (n == 2)
	{
	  struct hgcd_matrix1 M1;
	  ASSERT (s == 1);

	  if (mpn_hgcd2 (ap[1], ap[0], bp[1], bp[0], &M1))
	    {
	      /* Multiply M <- M * M1 */
	      mpn_hgcd_matrix_mul_1 (M, &M1, tp);
	      success = 1;
	    }
	}
      return success;
    }
  else
    {
      mp_size_t n2 = (3*n)/4 + 1;
      mp_size_t p = n/2;
      mp_size_t input_n = n;

      MPN_COPY (tp, ap + p, n - p);
      MPN_COPY (tp + n - p, bp + p, n - p);
      if (mpn_hgcd_appr (tp, tp + n - p, n - p, M, tp + 2*(n-p)))
	{
	  n = hgcd_matrix_apply (M, ap, bp, n);
	  /* FIXME: Discard some of the low limbs immediately? */
	  success = 1;
	}

      while (n > n2)
	{
	  mp_size_t nn;

	  /* Needs n + 1 storage */
	  nn = mpn_hgcd_step (n, ap, bp, s, M, tp);
	  if (!nn)
	    return success;

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
	  if (mpn_hgcd_appr (ap + p, bp + p, n - p, &M1, tp + scratch))
	    {
	      /* We always have max(M) > 2^{-(GMP_NUMB_BITS + 1)} max(M1) */
	      ASSERT (M->n + 2 >= M1.n);

	      /* Furthermore, assume M ends with a quotient (1, q; 0, 1),
		 then either q or q + 1 is a correct quotient, and M1 will
		 start with either (1, 0; 1, 1) or (2, 1; 1, 1). This
		 rules out the case that the size of M * M1 is much
		 smaller than the expected M->n + M1->n. */

	      ASSERT (M->n + M1.n < M->alloc);

	      /* We need a bound for of M->n + M1.n. Let n be the original
		 input size. Then

		 ceil(n/2) - 1 >= size of product >= M.n + M1.n - 2

		 and it follows that

		 M.n + M1.n <= ceil(n/2) + 1

		 Then 3*(M.n + M1.n) + 5 <= 3 * ceil(n/2) + 8 is the
		 amount of needed scratch space. */
	      mpn_hgcd_matrix_mul (M, &M1, tp + scratch);
	      return 1;
	    }
	}

      for(;;)
	{
	  mp_size_t nn;

	  ASSERT (n > s);
	  ASSERT (n <= 2*s);

	  nn = mpn_hgcd_step (n, ap, bp, s, M, tp);
	  
	  if (!nn)
	    return success;

	  n = nn;
	  success = 1;
	}
    }
}
