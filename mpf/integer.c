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

#include "gmp.h"
#include "gmp-impl.h"

#if MPF_FLOOR
#define _MPF_FLOOR_OR_CEIL
#define FUNC_NAME mpf_floor
#undef MPF_FLOOR
#define MPF_FLOOR 1
#define MPF_CEIL 0
#endif

#if MPF_CEIL
#define _MPF_FLOOR_OR_CEIL
#define FUNC_NAME mpf_ceil
#undef MPF_CEIL
#define MPF_CEIL 1
#define MPF_FLOOR 0
#endif

#if MPF_TRUNC
#undef FUNC_NAME
#define FUNC_NAME mpf_trunc
#endif

#ifdef _MPF_FLOOR_OR_CEIL
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
FUNC_NAME (mpf_ptr r, mpf_srcptr u)
#else
FUNC_NAME (r, u)
     mpf_ptr r;
     mpf_srcptr u;
#endif
{
  mp_ptr rp, up;
  mp_size_t size, asize;
  mp_size_t prec;
  mp_size_t ignored_n;
  mp_exp_t exp;

  size = u->_mp_size;
  rp = r->_mp_d;
  exp = u->_mp_exp;

  /* Single out the case where |u| < 1.  */
  if (exp <= 0)
    {
#ifdef _MPF_FLOOR_OR_CEIL
      if ((MPF_FLOOR && size < 0) || (MPF_CEIL && size >= 0))
	{
	  rp[0] = 1;
	  r->_mp_size = MPF_FLOOR ? -1 : 1;
	  r->_mp_exp = 1;
	  return;
	}
#endif
      r->_mp_size = 0;
      return;
    }

  prec = r->_mp_prec /* + 1 */;
  asize = ABS (size);

#ifdef _MPF_FLOOR_OR_CEIL
  ignored_n = 0;
#endif
  up = u->_mp_d;

  if (asize > prec)
    {
#ifdef _MPF_FLOOR_OR_CEIL
      ignored_n = asize - prec;
#endif
      up += asize - prec;
      asize = prec;
    }

  if (asize > exp)
    {
      long diff = asize - exp;
#ifdef _MPF_FLOOR_OR_CEIL
      ignored_n += diff;
#endif
      up += diff;
      asize = exp;
    }

#ifdef _MPF_FLOOR_OR_CEIL
  if (((MPF_FLOOR && size < 0) || (MPF_CEIL && size >= 0))
      && ! mpn_zero_p (up - ignored_n, ignored_n))
    {
      mp_limb_t cy;
      cy = mpn_add_1 (rp, up, asize, (mp_limb_t) 1);
      if (cy != 0)
	{
	  rp[asize++] = cy;
	  exp++;
	}
    }
  else
#endif
  MPN_COPY_INCR (rp, up, asize);

  r->_mp_exp = exp;
  r->_mp_size = size >= 0 ? asize : -asize;
}
