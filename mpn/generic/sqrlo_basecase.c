/* mpn_sqrlo_basecase -- Internal routine to square a natural number
   of length n.

   THIS IS AN INTERNAL FUNCTION WITH A MUTABLE INTERFACE.  IT IS ONLY
   SAFE TO REACH THIS FUNCTION THROUGH DOCUMENTED INTERFACES.


Copyright 1991-1994, 1996, 1997, 2000-2005, 2008, 2010, 2011, 2015
Free Software Foundation, Inc.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


#if HAVE_NATIVE_mpn_sqr_diagonal
#define MPN_SQR_DIAGONAL(rp, up, n)					\
  mpn_sqr_diagonal (rp, up, n)
#else
#define MPN_SQR_DIAGONAL(rp, up, n)					\
  do {									\
    mp_size_t _i;							\
    for (_i = 0; _i < (n); _i++)					\
      {									\
	mp_limb_t ul, lpl;						\
	ul = (up)[_i];							\
	umul_ppmm ((rp)[2 * _i + 1], lpl, ul, ul << GMP_NAIL_BITS);	\
	(rp)[2 * _i] = lpl >> GMP_NAIL_BITS;				\
      }									\
  } while (0)
#endif

#if HAVE_NATIVE_mpn_addlsh1_n_ip1
#define MPN_SQRLO_DIAG_ADDLSH1(rp, tp, up, n)				\
  do {									\
    MPN_SQR_DIAGONAL (rp, up, n>>1);					\
    if ((n & 1) != 0)							\
      (rp)[n - 1] = ((up)[n>>1] * (up)[n>>1]) & GMP_NUMB_MASK;		\
    mpn_addlsh1_n_ip1 (rp + 1, tp, n - 1);				\
  } while (0)
#else
#define MPN_SQRLO_DIAG_ADDLSH1(rp, tp, up, n)				\
  do {									\
    MPN_SQR_DIAGONAL (rp, up, n>>1);					\
    if ((n & 1) != 0)							\
      (rp)[n - 1] = ((up)[n>>1] * (up)[n>>1]) & GMP_NUMB_MASK;		\
    mpn_lshift (tp, tp, n - 1, 1);					\
    mpn_add_n (rp + 1, rp + 1, tp, n - 1);				\
  } while (0)
#endif


/* Default mpn_sqrlo_basecase using mpn_addmul_1.  */

void
mpn_sqrlo_basecase (mp_ptr rp, mp_srcptr up, mp_size_t n)
{
  mp_size_t i;

  ASSERT (n >= 1);
  ASSERT (! MPN_OVERLAP_P (rp, n, up, n));

  if (n <= 1)
    {
      mp_limb_t ul;
      ul = up[0];
      rp[0] = (ul * ul) & GMP_NUMB_MASK;
    }
  else
    {
      mp_limb_t tp[2 * SQR_TOOM2_THRESHOLD - 1];

      /* must fit n-1 limbs in tp */
      ASSERT (n <= 2 * SQR_TOOM2_THRESHOLD);

      mpn_mul_1 (tp, up + 1, n - 1, up[0]);
      for (i = 2; 2 * i - 1 < n; ++i)
	mpn_addmul_1 (tp + 2 * i - 2, up + i, n - 2 * i + 1, up[i - 1]);

      MPN_SQRLO_DIAG_ADDLSH1 (rp, tp, up, n);
    }
}
