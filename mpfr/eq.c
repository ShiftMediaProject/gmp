/* mpfr_eq -- Compare two floats up to a specified bit #.

Copyright 1999, 2001, 2003 Free Software Foundation, Inc.
(Copied from GNU MP, file mpf_eq.)

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* return non-zero if the first n_bits bits of u, v are equal,
   0 otherwise */
int
mpfr_eq (mpfr_srcptr u, mpfr_srcptr v, unsigned long int n_bits)
{
  mp_srcptr up, vp;
  mp_size_t usize, vsize, size, i;
  mp_exp_t uexp, vexp;
  int usign, k;

  if (MPFR_IS_NAN(u) || MPFR_IS_NAN(v))
    return 0; /* non equal */

  usign = MPFR_SIGN(u);

  if (MPFR_IS_INF(u))
    return MPFR_IS_INF(v) && usign == MPFR_SIGN(v); /* +Inf = +Inf */
  else if (MPFR_IS_INF(v))
    return 0; /* +Inf != -Inf */

  /* 1. Are the signs different?  */
  if (usign == MPFR_SIGN(v))
    {
      /* U and V are both non-negative or both negative.  */
      if (MPFR_IS_ZERO(u))
        return MPFR_IS_ZERO(v); /* 0 = 0 */
      if (MPFR_IS_ZERO(v))
        return MPFR_IS_ZERO(u); /* 0 = 0 */

      /* Fall out.  */
    }
  else
    {
      /* Either U or V is negative, but not both.  */
      return MPFR_IS_ZERO(u) && MPFR_IS_ZERO(v);
    }

  uexp = MPFR_GET_EXP (u);
  vexp = MPFR_GET_EXP (v);

  /* 2. Are the exponents different?  */
  if (uexp != vexp)
    return 0; /* no bit agree */

  usize = (MPFR_PREC(u) - 1) / BITS_PER_MP_LIMB + 1;
  vsize = (MPFR_PREC(v) - 1) / BITS_PER_MP_LIMB + 1;

  up = MPFR_MANT(u);
  vp = MPFR_MANT(v);

  if (usize > vsize)
    {
      if (vsize * BITS_PER_MP_LIMB < n_bits)
	{
	  k = usize - vsize - 1; 
	  while (k >= 0 && !up[k]) --k; 
	  if (k >= 0) 
	    return 0;		/* surely too different */
	}
      size = vsize;
    }
  else if (vsize > usize)
    {
      if (usize * BITS_PER_MP_LIMB < n_bits)
	{
	  k = vsize - usize - 1; 
	  while (k >= 0 && !vp[k]) --k; 
	  if (k >= 0) 
	    return 0;		/* surely too different */
	}
      size = usize;
    }
  else
    {
      size = usize;
    }

  /* now size = min (usize, vsize) */

  if (size > (n_bits + BITS_PER_MP_LIMB - 1) / BITS_PER_MP_LIMB)
    size = (n_bits + BITS_PER_MP_LIMB - 1) / BITS_PER_MP_LIMB;

  up += usize - size;
  vp += vsize - size;

  for (i = size - 1; i > 0 && n_bits >= BITS_PER_MP_LIMB; i--)
    {
      if (up[i] != vp[i])
	return 0;
      n_bits -= BITS_PER_MP_LIMB;
    }

  /* now either i=0 or n_bits<BITS_PER_MP_LIMB */

  if (n_bits > BITS_PER_MP_LIMB)
    return mpfr_cmp (u, v) == 0;

  if (n_bits & (BITS_PER_MP_LIMB - 1))
    return (up[i] >> (BITS_PER_MP_LIMB - (n_bits & (BITS_PER_MP_LIMB - 1))) == 
	    vp[i] >> (BITS_PER_MP_LIMB - (n_bits & (BITS_PER_MP_LIMB - 1)))); 
  else
    return (up[i] == vp[i]); 
}
