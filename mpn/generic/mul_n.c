/* mpn_mul_n -- Multiply two natural numbers of length n.

Copyright (C) 1991, 1992, 1993, 1994, 1996 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"

/* Multiply the natural numbers u (pointed to by UP) and v (pointed to by VP),
   both with SIZE limbs, and store the result at PRODP.  2 * SIZE limbs are
   always stored.  Return the most significant limb.

   Argument constraints:
   1. PRODP != UP and PRODP != VP, i.e. the destination
      must be distinct from the multiplier and the multiplicand.  */

static inline void
mpn_incr (mp_ptr p, mp_limb_t incr)
{
  mp_limb_t x;

  x = *p + incr;
  *p = x;
  if (x >= incr)
    return;
  while (++(*(++p)) == 0)
    ;
}

void
#if __STDC__
__gmpn_mul_n (mp_ptr prodp, mp_srcptr up, mp_srcptr vp, mp_size_t n, mp_ptr tspace)
#else
__gmpn_mul_n (prodp, up, vp, n, tspace)
     mp_ptr prodp;
     mp_srcptr up;
     mp_srcptr vp;
     mp_size_t n;
     mp_ptr tspace;
#endif
{
  if (n & 1)
    {
      /* Odd length. */
      mp_size_t n2, n3, np1, nm1;
      int sign;

      n2 = n >> 1;
      n3 = n - n2; /* = n2 + 1 */

      {
	mp_limb_t w, w0, w1; mp_srcptr x, y;
	/* Subtract smaller half of up[] from larger half. */
	sign = 0;
	w = up[n2];
	if (w != 0)
	  w -= mpn_sub_n (prodp, up, up + n3, n2);
	else
	  {
	    mp_size_t i;
	    i = n2 - 1;
	    while ((w0 = up[i]) == (w1 = up[n3 + i]) && i != 0)
	      i--;
	    if (w0 < w1)
	      {
		y = up;
		x = up + n3;
		sign = 1;
	      }
	    else
	      {
		x = up;
		y = up + n3;
	      }
	    mpn_sub_n (prodp, x, y, n2);
	  }
	prodp[n2] = w;
      }

      {
	mp_limb_t w, w0, w1; mp_srcptr x, y;
	/* Subtract smaller half of vp[] from larger half. */
	w = vp[n2];
	if (w != 0)
	  w -= mpn_sub_n (prodp + n3, vp, vp + n3, n2);
	else
	  {
	    mp_size_t i;
	    i = n2 - 1;
	    while ((w0 = vp[i]) == (w1 = vp[n3 + i]) && i != 0)
	      i--;
	    if (w0 < w1)
	      {
		y = vp;
		x = vp + n3;
		sign ^= 1;
	      }
	    else
	      {
		x = vp;
		y = vp + n3;
	      }
	    mpn_sub_n (prodp + n3, x, y, n2);
	  }
	prodp[n] = w;
      }

      np1 = n + 1;

      if (n3 < KARATSUBA_MUL_THRESHOLD)
	{
	  __gmpn_mul_basecase (tspace, prodp, n3, prodp + n3, n3);
	  __gmpn_mul_basecase (prodp, up, n3, vp, n3);
	  __gmpn_mul_basecase (prodp + np1, up + n3, n2, vp + n3, n2);
	}
      else
	{
	  __gmpn_mul_n (tspace, prodp, prodp + n3, n3, tspace + np1);
	  __gmpn_mul_n (prodp, up, vp, n3, tspace + np1);
	  __gmpn_mul_n (prodp + np1, up + n3, vp + n3, n2, tspace + np1);
	}

      if (sign)
	mpn_add_n (tspace, prodp, tspace, np1);
      else
	mpn_sub_n (tspace, prodp, tspace, np1);
      nm1 = n - 1;
      if (mpn_add_n (tspace, tspace, prodp + np1, nm1))
	{
	  mp_limb_t x = tspace[nm1] + 1;
	  tspace[nm1] = x;
	  if (x == 0)
	    ++tspace[n];
	}
      if (mpn_add_n (prodp + n3, prodp + n3, tspace, np1))
	mpn_incr (prodp + np1 + n3, (mp_limb_t) 1);
    }
  else
    {
      /* Even length. */
      mp_limb_t cy;
      mp_size_t n2 = n >> 1;
      int sign;

      {
	mp_limb_t w0, w1; mp_size_t i; mp_srcptr x, y;
	/* Subtract smaller half of up[] from larger half. */
	i = n2 - 1;
	while ((w0 = up[i]) == (w1 = up[n2 + i]) && i != 0)
	  i--;
	if (w0 < w1)
	  {
	    y = up;
	    x = up + n2;
	    sign = 1;
	  }
	else
	  {
	    x = up;
	    y = up + n2;
	    sign = 0;
	  }
	mpn_sub_n (prodp, x, y, n2);
      }

      {
	mp_limb_t w0, w1; mp_size_t i; mp_srcptr x, y;
	/* Subtract smaller half of vp[] from larger half. */
	i = n2-1UL;
	while ((w0 = vp[i]) == (w1 = vp[n2+i]) && i != 0)
	  i--;
	if (w0 < w1)
	  {
	    y = vp;
	    x = vp + n2;
	    sign ^= 1;
	  }
	else
	  {
	    x = vp;
	    y = vp + n2;
	  }
	mpn_sub_n (prodp + n2, x, y, n2);
      }

      if (n2 < KARATSUBA_MUL_THRESHOLD)
	{
	  __gmpn_mul_basecase (tspace, prodp, n2, prodp + n2, n2);
	  __gmpn_mul_basecase (prodp, up, n2, vp, n2);
	  __gmpn_mul_basecase (prodp + n, up + n2, n2, vp + n2, n2);
	}
      else
	{
	  __gmpn_mul_n (tspace, prodp, prodp + n2, n2, tspace + n);
	  __gmpn_mul_n (prodp, up, vp, n2, tspace + n);
	  __gmpn_mul_n (prodp + n, up + n2, vp + n2, n2, tspace + n);
	}

      if (sign)
	cy = mpn_add_n (tspace, prodp, tspace, n);
      else
	cy = -mpn_sub_n (tspace, prodp, tspace, n);
      cy += mpn_add_n (tspace, tspace, prodp + n, n);
      cy += mpn_add_n (prodp + n2, prodp + n2, tspace, n);
      mpn_incr (prodp + n + n2, cy);
    }
}

void
#if __STDC__
__gmpn_sqr (mp_ptr prodp, mp_srcptr up, mp_size_t n, mp_ptr tspace)
#else
__gmpn_sqr (prodp, up, n, tspace)
     mp_ptr prodp;
     mp_srcptr up;
     mp_size_t n;
     mp_ptr tspace;
#endif
{
  if (n & 1)
    {
      /* Odd length. */
      mp_size_t n2, n3, np1, nm1;

      n2 = n >> 1;
      n3 = n - n2; /* = n2 + 1 */

      {
	mp_limb_t w, w0, w1; mp_size_t i; mp_srcptr x, y;
	/* Subtract smaller half of up[] from larger half. */
	w = up[n2];
	if (w)
	  w -= mpn_sub_n (prodp, up, up + n3, n2);
	else
	  {
	    i = n2 - 1;
	    while ((w0 = up[i]) == (w1 = up[n3 + i]) && i != 0)
	      i--;
	    if (w0 < w1)
	      {
		y = up;
		x = up + n3;
	      }
	    else
	      {
		x = up;
		y = up + n3;
	      }
	    mpn_sub_n (prodp, x, y, n2);
	  }
	prodp[n2] = w;
      }

      np1 = n + 1;

      if (n3 < KARATSUBA_SQR_THRESHOLD)
	{
	  __gmpn_sqr_basecase (tspace, prodp, n3);
	  __gmpn_sqr_basecase (prodp, up, n3);
	  __gmpn_sqr_basecase (prodp + np1, up + n3, n2);
	}
      else
	{
	  __gmpn_sqr (tspace, prodp, n3, tspace + np1);
	  __gmpn_sqr (prodp, up, n3, tspace + np1);
	  __gmpn_sqr (prodp + np1, up + n3, n2, tspace + np1);
	}

      mpn_sub_n (tspace, prodp, tspace, np1);
      nm1 = n - 1;
      if (mpn_add_n (tspace, tspace, prodp + np1, nm1))
	{
	  mp_limb_t x = tspace[nm1] + 1;
	  tspace[nm1] = x;
	  if (x == 0)
	    ++tspace[n];
	}
      if (mpn_add_n (prodp + n3, prodp + n3, tspace, np1))
	mpn_incr (prodp + np1 + n3, (mp_limb_t) 1);
    }
  else
    {
      /* Even length. */
      mp_limb_t cy;
      mp_size_t n2 = n >> 1;

      {
	mp_limb_t w0, w1; mp_size_t i; mp_srcptr x, y;
	/* Subtract smaller half of up[] from larger half. */
	i = n2 - 1;
	while ((w0 = up[i]) == (w1 = up[n2 + i]) && i != 0)
	  i--;
	if (w0 < w1)
	  {
	    y = up;
	    x = up + n2;
	  }
	else
	  {
	    x = up;
	    y = up + n2;
	  }
	mpn_sub_n (prodp, x, y, n2);
      }

    if (n2 < KARATSUBA_SQR_THRESHOLD)
      {
	__gmpn_sqr_basecase (tspace, prodp, n2);
	__gmpn_sqr_basecase (prodp, up, n2);
	__gmpn_sqr_basecase (prodp + n, up + n2, n2);
      }
    else
      {
	__gmpn_sqr (tspace, prodp, n2, tspace + n);
	__gmpn_sqr (prodp, up, n2, tspace + n);
	__gmpn_sqr (prodp + n, up + n2, n2, tspace + n);
      }

      cy = -mpn_sub_n (tspace, prodp, tspace, n);
      cy += mpn_add_n (tspace, tspace, prodp + n, n);
      cy += mpn_add_n (prodp + n2, prodp + n2, tspace, n);
      mpn_incr (prodp + n + n2, cy);
    }
}

/* This should be made into an inline function in gmp.h.  */
inline void
#if __STDC__
mpn_mul_n (mp_ptr prodp, mp_srcptr up, mp_srcptr vp, mp_size_t n)
#else
mpn_mul_n (prodp, up, vp, n)
     mp_ptr prodp;
     mp_srcptr up;
     mp_srcptr vp;
     mp_size_t n;
#endif
{
  TMP_DECL (marker);
  TMP_MARK (marker);
  if (up == vp)
    {
      if (n < KARATSUBA_SQR_THRESHOLD)
	{
	  __gmpn_sqr_basecase (prodp, up, n);
	}
      else
	{
	  mp_ptr tspace;
	  tspace = (mp_ptr) TMP_ALLOC (2 * (n + BITS_PER_MP_LIMB) * BYTES_PER_MP_LIMB);
	  __gmpn_sqr (prodp, up, n, tspace);
	}
    }
  else
    {
      if (n < KARATSUBA_MUL_THRESHOLD)
	{
	  __gmpn_mul_basecase (prodp, up, n, vp, n);
	}
      else
	{
	  mp_ptr tspace;
	  tspace = (mp_ptr) TMP_ALLOC (2 * (n + BITS_PER_MP_LIMB) * BYTES_PER_MP_LIMB);
	  __gmpn_mul_n (prodp, up, vp, n, tspace);
	}
    }
  TMP_FREE (marker);
}
