/* mpq_set_d(mpq_t q, double d) -- Set q to d without rounding.

Copyright (C) 2000 Free Software Foundation, Inc.

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

#if BITS_PER_MP_LIMB != 32 && BITS_PER_MP_LIMB != 64
This file does not handle any the used BITS_PER_MP_LIMB.
#endif

void
#if __STDC__
mpq_set_d (mpq_ptr dest, double d)
#else
mpq_set_d (dest, d)
     mpq_ptr dest;
     double d;
#endif
{
  int negative;
  mp_exp_t exp;
  mp_limb_t tp[3];
  mp_ptr np, dp;
  mp_size_t nn, dn;
  int c;

  negative = d < 0;
  d = ABS (d);

  exp = __gmp_extract_double (tp, d);

  /* There are two main version of the conversion.  The `then' arm handles
     things that have a fractional part, while the `else' part handles
     only integers.  */
#if BITS_PER_MP_LIMB == 32
  if (exp <= 1 || (exp == 2 && tp[0] != 0))
#else
  if (exp <= 1)
#endif
    {
      dn = -exp;
      if (dest->_mp_num._mp_alloc < 3)
	_mpz_realloc (&(dest->_mp_num), 3);
      np = PTR(&(dest->_mp_num));
#if BITS_PER_MP_LIMB == 32
      if ((tp[0] | tp[1]) == 0)
	np[0] = tp[2], nn = 1;
      else if (tp[0] == 0)
	np[1] = tp[2], np[0] = tp[1], nn = 2;
      else
	np[2] = tp[2], np[1] = tp[1], np[0] = tp[0], nn = 3;
#else
      if (tp[0] == 0)
	np[0] = tp[1], nn = 1;
      else
	np[1] = tp[1], np[0] = tp[0], nn = 2;
#endif
      dn += nn + 1;
      ASSERT_ALWAYS (dn > 0);
      if (dest->_mp_den._mp_alloc < dn)
	_mpz_realloc (&(dest->_mp_den), dn);
      dp = PTR(&(dest->_mp_den));
      MPN_ZERO (dp, dn - 1);
      dp[dn - 1] = 1;
      SIZ(&(dest->_mp_den)) = dn;
      SIZ(&(dest->_mp_num)) = negative ? -nn : nn;
      count_trailing_zeros (c, np[0] | dp[0]);
      mpn_rshift (np, np, nn, c);
      mpn_rshift (dp, dp, dn, c);
    }
  else
    {
      nn = exp;
      if (dest->_mp_num._mp_alloc < 3)
	_mpz_realloc (&(dest->_mp_num), 3);
      np = PTR(&(dest->_mp_num));
#if BITS_PER_MP_LIMB == 32
      switch (nn)
        {
	default:
          MPN_ZERO (np, nn - 3);
          np += nn - 3;
	  /* fall through */
	case 3:
	  np[2] = tp[2], np[1] = tp[1], np[0] = tp[0];
	  break;
	case 2:
	  np[1] = tp[2], np[0] = tp[1];
	  break;
	case 1:
	  ASSERT_ALWAYS (1);
	}
#else
      switch (nn)
        {
	default:
	  MPN_ZERO (np, nn - 2);
	  np += nn - 2;
	  /* fall through */
	case 2:
	  np[1] = tp[1], np[0] = tp[0];
	  break;
	case 1:
	  ASSERT_ALWAYS (1);
	}
#endif
      dp = PTR(&(dest->_mp_den));
      dp[0] = 1;
      SIZ(&(dest->_mp_den)) = 1;
      SIZ(&(dest->_mp_num)) = negative ? -nn : nn;
    }
}
