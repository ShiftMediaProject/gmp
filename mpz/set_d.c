/* mpz_set_d(integer, val) -- Assign INTEGER with a double value VAL.

Copyright (C) 1995 Free Software Foundation, Inc.

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
#include <math.h>

#define MP_BASE_AS_DOUBLE (2.0 * ((mp_limb_t) 1 << (BITS_PER_MP_LIMB - 1)))

void
#if __STDC__
mpz_set_d (mpz_ptr dest, double val)
#else
mpz_set_d (dest, val)
     mpz_ptr dest;
     double val;
#endif
{
  int negative;
  double base;
  int mexp;
  unsigned exp;
  mp_size_t size;

  negative = val < 0;
  val = fabs (val);

  /* Zero is not handled correctly by the code below.  */
  if (val < 1)
    {
      dest->_mp_size = 0;
      return;
    }
  /* Handle small arguments quickly.  */
  if (val < MP_BASE_AS_DOUBLE)
    {
      dest->_mp_d[0] = val;
      dest->_mp_size = negative ? -1 : 1;
      return;
    }

  base = frexp (val, &mexp);
  exp = mexp;

#if BITS_PER_MP_LIMB >= 64
  {
    double dlo, dhi;
    mp_size_t lexp, bexp;
    mp_limb_t l0, l1, l2;
    mp_ptr rp = dest->_mp_d;

    l0 = base * MP_BASE_AS_DOUBLE;
    lexp = exp / BITS_PER_MP_LIMB;
    bexp = exp % BITS_PER_MP_LIMB;
    if (bexp != 0)
      {
	l1 = l0 >> (BITS_PER_MP_LIMB - bexp);
	l0 = l0 << bexp;
      }
    else
      {
	l1 = l0;
	l0 = 0;
	lexp--;
      }
    size = lexp + 1;
    if (dest->_mp_alloc < size)
      _mpz_realloc (dest, size);
    MPN_ZERO (rp, lexp - 1);
    rp[lexp - 1] = l0;
    rp[lexp] = l1;
  }
#endif
#if BITS_PER_MP_LIMB < 64
  {
    double dlo, dhi;
    mp_size_t lexp, bexp;
    mp_limb_t l0, l1, l2;
    mp_ptr rp = dest->_mp_d;

    dlo = modf (base * MP_BASE_AS_DOUBLE, &dhi);
    l0 = dlo * MP_BASE_AS_DOUBLE;
    l1 = dhi;
    lexp = exp / BITS_PER_MP_LIMB;
    bexp = exp % BITS_PER_MP_LIMB;
    if (bexp != 0)
      {
	l2 = l1 >> (BITS_PER_MP_LIMB - bexp);
	l1 = (l1 << bexp) | (l0 >> (BITS_PER_MP_LIMB - bexp));
	l0 = l0 << bexp;
      }
    else
      {
	l2 = l1;
	l1 = l0;
	l0 = 0;
	lexp--;
      }
    if (exp <= 2 * BITS_PER_MP_LIMB)
      {
	rp[0] = l1;
	rp[1] = l2;
	size = 2;
      }
    else
      {
	size = lexp + 1;
	if (dest->_mp_alloc < size)
	  _mpz_realloc (dest, size);
	MPN_ZERO (rp, lexp - 2);
	rp[lexp - 2] = l0;
	rp[lexp - 1] = l1;
	rp[lexp] = l2;
      }
  }
#endif

  dest->_mp_size = negative ? -size : size;
}
