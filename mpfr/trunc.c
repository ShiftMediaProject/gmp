/* mpf_trunc, mpf_floor, mpf_ceil -- Assign a float from another float while
   rounding it to an integer.

Copyright (C) 1997, 1998 Free Software Foundation, Inc.

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

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#ifdef OPERATION_floor
#define _MPFR_FLOOR_OR_CEIL
#define FUNC_NAME mpfr_floor
#define MPFR_FLOOR 1
#define MPFR_CEIL 0
#endif

#ifdef OPERATION_ceil
#define _MPFR_FLOOR_OR_CEIL
#define FUNC_NAME mpfr_ceil
#define MPFR_CEIL 1
#define MPFR_FLOOR 0
#endif

#ifdef OPERATION_trunc
#undef FUNC_NAME
#define FUNC_NAME mpfr_trunc
#endif

#ifdef _MPFR_FLOOR_OR_CEIL
static int
mpn_zero_p (p, n)
     mp_ptr p;
     mp_size_t n;
{
  mp_size_t i;

  for (i = 0; i < n; i++)
    {
      if (p[i] != 0)
	return 0;
    }

  return 1;
}
#endif

void
#if __STDC__
FUNC_NAME (mpfr_ptr r, mpfr_srcptr u)
#else
FUNC_NAME (r, u)
     mpfr_ptr r;
     mpfr_srcptr u;
#endif
{
  mp_ptr rp, up;
  mp_size_t asize;
  mp_size_t prec, rw;
#ifdef _MPFR_FLOOR_OR_CEIL
  mp_size_t ignored_n;
#endif
  mp_exp_t exp;
  int signu; long diff;

  if (MPFR_IS_NAN(u)) {
    MPFR_SET_NAN(r);
    return;
  }

  MPFR_CLEAR_NAN(r);

  if (MPFR_IS_INF(u)) {
    MPFR_SET_INF(r);
    if (MPFR_SIGN(r) != MPFR_SIGN(u)) MPFR_CHANGE_SIGN(r);
    return;
  }

  MPFR_CLEAR_INF(r);

  if (!MPFR_NOTZERO(u)) {
    MPFR_SET_ZERO(r);
    return;
  }

  signu = MPFR_SIZE(u);
  rp = MPFR_MANT(r);
  exp = MPFR_EXP(u);
  prec = (MPFR_PREC(r) - 1)/BITS_PER_MP_LIMB + 1;

  /* Single out the case where |u| < 1.  */
  if (exp <= 0)
    {
#ifdef _MPFR_FLOOR_OR_CEIL
      if ((MPFR_FLOOR && signu < 0) || (MPFR_CEIL && signu >= 0))
	{
	  rp[prec-1] = (mp_limb_t) 1 << (BITS_PER_MP_LIMB-1);
	  MPN_ZERO(rp, prec-1);
	  /* sign of result is that of u */
	  if (MPFR_SIGN(r) * signu < 0) MPFR_CHANGE_SIGN(r);
	  MPFR_EXP(r) = 1;
	  return;
	}
#endif
      MPFR_SET_ZERO(r);
      return;
    }

  asize = (MPFR_PREC(u) - 1)/BITS_PER_MP_LIMB + 1;

#ifdef _MPFR_FLOOR_OR_CEIL
  ignored_n = 0;
#endif
  up = MPFR_MANT(u);

  if (asize > prec)
    {
#ifdef _MPFR_FLOOR_OR_CEIL
      ignored_n = asize - prec;
#endif
      up += asize - prec;
      asize = prec;
    }

  diff = BITS_PER_MP_LIMB * asize - exp;
  if (diff > 0)
    {
      diff = diff/BITS_PER_MP_LIMB;
#ifdef _MPFR_FLOOR_OR_CEIL
      ignored_n += diff;
#endif
      up += diff;
      asize -= diff;
    }

  /* number of non significant bits in low limb of r */
  rw = asize * BITS_PER_MP_LIMB - exp;
  MPN_ZERO(rp, prec-asize);
  rp += prec-asize;

#ifdef _MPFR_FLOOR_OR_CEIL
  if (((MPFR_FLOOR && signu < 0) || (MPFR_CEIL && signu >= 0))
      && (!mpn_zero_p (up - ignored_n, ignored_n)
	  || (rw && (up[0] << (BITS_PER_MP_LIMB-rw)))))
    {
      mp_limb_t cy;
      cy = mpn_add_1 (rp, up, asize, (mp_limb_t) 1 << rw);
      if (cy != 0)
	{
	  mpn_rshift(rp, rp, asize, 1);
	  rp[asize-1] = (mp_limb_t) 1 << (BITS_PER_MP_LIMB - 1);
	  exp++;
	}
    }
  else
#endif
    MPN_COPY (rp, up, asize);

  /* Put to 0 the remaining bits */
  if (rw) rp[0] &=
	    ~((((mp_limb_t)1)<<rw) - (mp_limb_t)1);

  MPFR_EXP(r) = exp;
  if (MPFR_SIGN(r) * signu < 0) MPFR_CHANGE_SIGN(r);
}
