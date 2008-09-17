/* mpn/gcd.c: mpn_gcd for gcd of two odd integers.

Copyright 1991, 1993, 1994, 1995, 1996, 1997, 1998, 2000, 2001, 2002, 2003,
2004, 2005 Free Software Foundation, Inc.

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

static inline int
mpn_zero_p (mp_srcptr ap, mp_size_t n)
{
  mp_size_t i;
  for (i = n - 1; i >= 0; i--)
    {
      if (ap[i] != 0)
	return 0;
    }
  return 1;
}

/* Uses the HGCD operation described in

     N. Möller, On Schönhage's algorithm and subquadratic integer gcd
     computation, Math. Comp. 77 (2008), 589-607.

  to reduce inputs until they are of size below GCD_DC_THRESHOLD, and
  then uses Lehmer's algorithm.
*/

/* Some reasonable choices are n / 2 (same as in hgcd), and p = (n +
 * 2)/3, which gives a balanced multiplication in
 * mpn_hgcd_matrix_adjust. However, p = 2 n/3 gives slightly better
 * performance. The matrix-vector multiplication is then
 * 4:1-unbalanced, with matrix elements of size n/6, and vector
 * elements of size p = 2n/3. */
#define CHOOSE_P(n) (2*(n) / 3)

mp_size_t
mpn_gcd (mp_ptr gp, mp_ptr up, mp_size_t usize, mp_ptr vp, mp_size_t n)
{
  mp_size_t talloc;
  mp_size_t scratch;
  mp_size_t matrix_scratch;
  
  mp_size_t gn;
  mp_ptr tp;
  TMP_DECL;

  /* FIXME: Check for small sizes first, before setting up temporary
     storage etc. */
  talloc = MPN_GCD_LEHMER_N_ITCH(n);
  
  /* For initial division */
  scratch = usize - n + 1;
  if (scratch > talloc)
    talloc = scratch;

  if (ABOVE_THRESHOLD (n, GCD_DC_THRESHOLD))
    {
      mp_size_t hgcd_scratch;
      mp_size_t update_scratch;
      mp_size_t p = CHOOSE_P (n);
      mp_size_t scratch;
      matrix_scratch = MPN_HGCD_MATRIX_INIT_ITCH (n - p);
      hgcd_scratch = mpn_hgcd_itch (n - p);
      update_scratch = p + n - 1;

      scratch = matrix_scratch + MAX(hgcd_scratch, update_scratch);
      if (scratch > talloc)
	talloc = scratch;
    }

  TMP_MARK;
  tp = TMP_ALLOC_LIMBS(talloc);

  if (usize > n)
    {
      mpn_tdiv_qr (tp, up, 0, up, usize, vp, n);

      if (mpn_zero_p (up, n))
	{
	  MPN_COPY (gp, vp, n);
	  TMP_FREE;
	  return n;
	}
    }
  
  while (ABOVE_THRESHOLD (n, GCD_DC_THRESHOLD))
    {
      struct hgcd_matrix M;
      mp_size_t p = CHOOSE_P (n);
      mp_size_t matrix_scratch = MPN_HGCD_MATRIX_INIT_ITCH (n - p);
      mp_size_t nn;
      mpn_hgcd_matrix_init (&M, n - p, tp);
      nn = mpn_hgcd (up + p, vp + p, n - p, &M, tp + matrix_scratch);
      if (nn > 0)
	{
	  ASSERT (M.n <= (n - p - 1)/2);
	  ASSERT (M.n + p <= (p + n - 1) / 2);
	  /* Temporary storage 2 (p + M->n) <= p + n - 1. */
	  n = mpn_hgcd_matrix_adjust (&M, p + nn, up, vp, p, tp + matrix_scratch);
	}
      else
	{
	  /* Temporary storage n */
	  n = mpn_gcd_subdiv_step (gp, &gn, up, vp, n, tp);
	  if (n == 0)
	    {
	      TMP_FREE;
	      return gn;
	    }
	}
    }

  gn = mpn_gcd_lehmer_n (gp, up, vp, n, tp);
  TMP_FREE;
  return gn;
}
