/* mpn_get_d -- limbs to double conversion.

   THE FUNCTIONS IN THIS FILE ARE FOR INTERNAL USE ONLY.  THEY'RE ALMOST
   CERTAIN TO BE SUBJECT TO INCOMPATIBLE CHANGES OR DISAPPEAR COMPLETELY IN
   FUTURE GNU MP RELEASES.

Copyright 1996, 1997, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"


/* HPPA 8000, 8200, 8500, and 8600 traps FCNV,UDW,DBL for values >= 2^63.  This
   makes it slow.  Worse, the Linux kernel apparently uses untested code in its
   trap handling routines, and gets the sign wrong.  Their compiler port
   doesn't define __hppa as it should.  Here is a workaround:  */
#if (defined (__hppa) || defined (__hppa__)) && GMP_LIMB_BITS == 64
#define limb2dbl(limb) \
    ((limb) >> (GMP_LIMB_BITS - 1) != 0  				\
     ? 2.0 * (double) (mp_limb_signed_t) (((limb) >> 1) | ((limb) & 1))	\
     : (double) (mp_limb_signed_t) (limb))
#else
#define limb2dbl(limb) \
    (double) (limb)
#endif


/* FIXME: Would prefer to inline this on all compilers, not just those with
   "inline".  */
static inline int
mpn_zero_p (mp_srcptr p, mp_size_t n)
{
  mp_size_t i;

  for (i = 0; i < n; i++)
    if (p[i] != 0)
      return 0;

  return 1;
}


/* Return the value {ptr,size}*2^exp, negative if sign<0.
   Must have size>=1, and non-zero high limb ptr[size-1].  */

double
mpn_get_d (mp_srcptr ptr, mp_size_t size, mp_size_t sign, long exp)
{
  double res;
  mp_limb_t hz, lz;
  int cnt;

  ASSERT (size > 0);
  ASSERT (ptr[size-1] != 0);
  ASSERT_MPN (ptr, size);

  /* Check for a large exp first, to avoid overflow from adding the "size"
     bits when near LONG_MAX. */
#if _GMP_IEEE_FLOATS
  if (UNLIKELY (exp >= 2047))
    goto infinity;
#endif

  if (size == 1)
    {
      res = limb2dbl (ptr[0]);
    }
  else if (size == 2)
    {
      res = MP_BASE_AS_DOUBLE * limb2dbl (ptr[1]) + limb2dbl (ptr[0]);
    }
  else
    {
      count_leading_zeros (cnt, ptr[size - 1]);
      cnt -= GMP_NAIL_BITS;

#if BITS_PER_MP_LIMB == 32
      if (cnt == 0)
	{
	  hz = ptr[size - 1];
	  lz = ptr[size - 2];
	}
      else
	{
	  hz = ((ptr[size - 1] << cnt) | (ptr[size - 2] >> GMP_NUMB_BITS - cnt)) & GMP_NUMB_MASK;
	  lz = ((ptr[size - 2] << cnt) | (ptr[size - 3] >> GMP_NUMB_BITS - cnt)) & GMP_NUMB_MASK;
	}
#if _GMP_IEEE_FLOATS
      /* Take bits from less significant limbs, but only if they may affect
	 the result.  */
      if ((lz & 0x7ff) == 0x400)
	{
	  if (cnt != 0)
	    lz += (((ptr[size - 3] << cnt) & GMP_NUMB_MASK) != 0
		   || ! mpn_zero_p (ptr, size - 3));
	  else
	    lz += (! mpn_zero_p (ptr, size - 2));
	}
#endif
      res = MP_BASE_AS_DOUBLE * limb2dbl (hz) + limb2dbl (lz);
      exp += (size - 2) * GMP_NUMB_BITS - cnt;
#endif
#if BITS_PER_MP_LIMB == 64
      if (cnt == 0)
	hz = ptr[size - 1];
      else
	hz = ((ptr[size - 1] << cnt) | (ptr[size - 2] >> GMP_NUMB_BITS - cnt)) & GMP_NUMB_MASK;
#if _GMP_IEEE_FLOATS
      if ((hz & 0x7ff) == 0x400)
	{
	  if (cnt != 0)
	    hz += (((ptr[size - 2] << cnt) & GMP_NUMB_MASK) != 0
		   || ! mpn_zero_p (ptr, size - 2));
	  else
	    hz += (! mpn_zero_p (ptr, size - 1));
	}
#endif
      res = limb2dbl (hz);
      exp += (size - 1) * GMP_NUMB_BITS - cnt;
#endif
    }


#if _GMP_IEEE_FLOATS
  {
#if defined (__alpha) && __GNUC__ == 2 && __GNUC_MINOR__ == 8
    /* Work around alpha-specific bug in GCC 2.8.x.  */
    volatile
#endif
      union ieee_double_extract x;
    x.d = res;
    exp += x.s.exp;
    x.s.exp = exp;
    if (exp >= 2047)
      {
	/* Return +-infinity */
      infinity:
	x.s.sig = 0;
	x.s.exp = 2047;
	x.s.manl = x.s.manh = 0;
      }
    else if (exp < 1)
      {
	x.s.exp = 1;		/* smallest exponent (biased) */
	/* Divide result by 2 until we have scaled it to the right IEEE
	   denormalized number, but stop if it becomes zero.  */
	while (exp < 1 && x.d != 0)
	  {
	    x.d *= 0.5;
	    exp++;
	  }
      }
    res = x.d;
  }
#else
  {
    double factor, r;

    factor = 2.0;
    if (exp < 0)
      {
	factor = 0.5;
	exp = -exp;
      }
    if (exp != 0)
      {
	if ((exp & 1) != 0)
	  res *= factor;
	exp >>= 1;
	while (exp != 0)
	  {
	    factor *= factor;
	    if ((exp & 1) != 0)
	      res *= factor;
	    exp >>= 1;
	  }
      }
  }
#endif

  return (sign >= 0 ? res : -res);
}
