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
    mp_size_t nhalf;							\
    nhalf = (n) >> 1;							\
    MPN_SQR_DIAGONAL ((rp), (up), nhalf);				\
    if (((n) & 1) != 0)							\
      {									\
	mp_limb_t op;							\
	op = (up)[nhalf];						\
	(rp)[(n) - 1] = (op * op) & GMP_NUMB_MASK;			\
      }									\
    mpn_addlsh1_n_ip1 ((rp) + 1, (tp), (n) - 1);			\
  } while (0)
#else
#define MPN_SQRLO_DIAG_ADDLSH1(rp, tp, up, n)				\
  do {									\
    mp_size_t nhalf;							\
    nhalf = (n) >> 1;							\
    MPN_SQR_DIAGONAL ((rp), (up), nhalf);				\
    if (((n) & 1) != 0)							\
      {									\
	mp_limb_t op;							\
	op = (up)[nhalf];						\
	(rp)[(n) - 1] = (op * op) & GMP_NUMB_MASK;			\
      }									\
    mpn_lshift ((tp), (tp), (n) - 1, 1);				\
    mpn_add_n ((rp) + 1, (rp) + 1, (tp), (n) - 1);			\
  } while (0)
#endif


/* Default mpn_sqrlo_basecase using mpn_addmul_1.  */
#ifndef SQRLO_SPECIAL_CASES
#define SQRLO_SPECIAL_CASES 2
#endif
void
mpn_sqrlo_basecase (mp_ptr rp, mp_srcptr up, mp_size_t n)
{
  mp_limb_t ul;

  ASSERT (n >= 1);
  ASSERT (! MPN_OVERLAP_P (rp, n, up, n));

  ul = up[0];

  if (n <= SQRLO_SPECIAL_CASES)
    {
#if SQRLO_SPECIAL_CASES == 1
      rp[0] = (ul * ul) & GMP_NUMB_MASK;
#else
      if (n == 1)
	rp[0] = (ul * ul) & GMP_NUMB_MASK;
      else
	{
	  mp_limb_t hi, lo, ul1;
	  umul_ppmm (hi, lo, ul, ul << GMP_NAIL_BITS);
	  rp[0] = lo >> GMP_NAIL_BITS;
	  ul1 = up[1];
#if SQRLO_SPECIAL_CASES == 2
	  rp[1] = (hi + ul * ul1 * 2) & GMP_NUMB_MASK;
#else
	  if (n == 2)
	    rp[1] = (hi + ul * ul1 * 2) & GMP_NUMB_MASK;
	  else
	    {
	      mp_limb_t hi1;
#if GMP_NAIL_BITS != 0
	      ul <<= 1;
#endif
	      umul_ppmm (hi1, lo, ul1 << GMP_NAIL_BITS, ul);
	      hi1 += ul * up[2];
#if GMP_NAIL_BITS == 0
	      hi1 = (hi1 << 1) | (lo >> (GMP_LIMB_BITS - 1));
	      add_ssaaaa(rp[2], rp[1], hi1, lo << 1, ul1 * ul1, hi);
#else
	      hi += lo >> GMP_NAIL_BITS;
	      rp[1] = hi & GMP_NUMB_MASK;
	      rp[2] = (hi1 + ul1 * ul1 + (hi >> GMP_NUMB_BITS)) & GMP_NUMB_MASK;	      
#endif
	    }
#endif
	}
#endif
    }
  else
    {
      mp_limb_t tp[2 * SQR_TOOM2_THRESHOLD - 1];
      mp_size_t i;

      /* must fit n-1 limbs in tp */
      ASSERT (n <= 2 * SQR_TOOM2_THRESHOLD);

      --n;
#ifdef SQRLO_SHORTCUT_MULTIPLICATIONS
      {
	mp_limb_t cy;

	cy = mpn_mul_1 (tp, up + 1, n - 1, ul) + ul * up[n];
	for (i = 1; 2 * i + 1 < n; ++i)
	  {
	    ul = up[i];
	    cy += mpn_addmul_1 (tp + 2 * i, up + i + 1, n - 2 * i - 1, ul) + ul * up[n - i];
	  }
	tp [n-1] = (cy + ((n & 1)?up[i] * up[i + 1]:0)) & GMP_NUMB_MASK;
      }
#else
      mpn_mul_1 (tp, up + 1, n, ul);
      for (i = 1; 2 * i < n; ++i)
	mpn_addmul_1 (tp + 2 * i, up + i + 1, n - 2 * i, up[i]);
#endif

      MPN_SQRLO_DIAG_ADDLSH1 (rp, tp, up, n + 1);
    }
}
#undef SQRLO_SPECIAL_CASES
