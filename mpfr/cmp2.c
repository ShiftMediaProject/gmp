/* mpfr_cmp2 -- exponent shift when subtracting two numbers.

Copyright (C) 1999-2001 Free Software Foundation.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* returns the number of cancelled bits when one subtracts abs(c) from abs(b). 
   Assumes |b| >= |c|, which implies MPFR_EXP(b)>=MPFR_EXP(c).
   if |b| = |c|, returns prec(b).

   Assumes neither of b or c is NaN or +/- infinity.

   In other terms mpfr_cmp2 (b, c) returns EXP(b) - EXP(b-c).
*/
mp_prec_t
mpfr_cmp2 (mpfr_srcptr b, mpfr_srcptr c)
{
  mp_limb_t *bp, *cp, bb, cc = 0, lastc = 0, dif, high_dif = 0;
  mp_size_t bn, cn;
  mp_exp_unsigned_t diff_exp;
  mp_prec_t res = 0;

  MPFR_ASSERTN(MPFR_IS_FP(b));
  MPFR_ASSERTN(MPFR_IS_FP(c));

  if (MPFR_IS_ZERO(c))
    return 0;

  MPFR_ASSERTN(MPFR_NOTZERO(b));

  bp = MPFR_MANT(b);
  cp = MPFR_MANT(c);

  bn = (MPFR_PREC(b) - 1) / BITS_PER_MP_LIMB;
  cn = (MPFR_PREC(c) - 1) / BITS_PER_MP_LIMB;

  MPFR_ASSERTN(MPFR_EXP(b) >= MPFR_EXP(c));
  diff_exp = (mp_exp_unsigned_t) MPFR_EXP(b) - MPFR_EXP(c);
  
  if (diff_exp == 0) /* otherwise the shifted most significant limb of c
			cannot match bp[bn] */
    {
      while (bn>=0 && cn>=0 && bp[bn] == cp[cn])
	{
	  bn--;
	  cn--;
	  res += BITS_PER_MP_LIMB;
	}

      if (bn < 0) /* b = c */
	return MPFR_PREC(b);

      if (cn < 0) /* c discards exactly the upper part of b */
	{
          unsigned int z;

	  while (bn>=0 && bp[bn]==0)
	    {
	      bn--;
	      res += BITS_PER_MP_LIMB;
	    }

	  if (bn < 0) /* b = c */
	    return MPFR_PREC(b);

	  count_leading_zeros(z, bp[bn]); /* bp[bn] <> 0 */
	  return res + z;
	}
    }

  /* now we have removed the identical upper limbs of b and c
     (can happen only when diff_exp = 0): bp[bn] > cc, bn>=0, cn>=0 */

  if (diff_exp < BITS_PER_MP_LIMB)
    {
      cc = cp[cn] >> diff_exp;
      /* warning: a shift by BITS_PER_MP_LIMB may give wrong results */
      if (diff_exp)
        lastc = cp[cn] << (BITS_PER_MP_LIMB - diff_exp);
      cn--;
    }
  else
    diff_exp -= BITS_PER_MP_LIMB;

  dif = bp[bn--] - cc; /* necessarily dif >= 1 */

  while ((cn >= 0 || lastc != 0) && (high_dif == 0) && (dif == 1))
    { /* dif=1 implies diff_exp = 0 or 1 */
      bb = (bn >= 0) ? bp[bn] : 0;
      cc = lastc;
      if (cn >= 0)
	{
          if (diff_exp == 0)
            {
              cc += cp[cn];
            }
          else /* diff_exp = 1 */
            {
              cc += cp[cn] >> 1;
              lastc = cp[cn] << (BITS_PER_MP_LIMB - 1);
            }
	}
      else
	lastc = 0;
      high_dif = 1 - mpn_sub_n (&dif, &bb, &cc, 1);
      bn--;
      cn--;
      res += BITS_PER_MP_LIMB;
    }

  /* (cn<0 and lastc=0) or (high_dif,dif)<>(0,1) */

  if (high_dif != 0) /* necessarily high_dif = 1 */
    {
      res--;
      if (dif != 0)
	return res;
    }
  else /* high_dif = 0 */
    {
      unsigned int z;

      count_leading_zeros(z, dif); /* dif > 1 here */
      res += z;
      if (dif != (MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - z - 1)))
	return res; /* dif is not a power of two */
    }

  /* now result is res + (low(b) < low(c)) */
  while (bn >= 0 && (cn >= 0 || lastc != 0))
    {
      if (diff_exp >= BITS_PER_MP_LIMB)
	diff_exp -= BITS_PER_MP_LIMB;
      else
	{
	  cc = lastc;
	  if (cn >= 0)
	    {
	      cc += cp[cn] >> diff_exp;
	      if (diff_exp != 0)
                lastc = cp[cn] << (BITS_PER_MP_LIMB - diff_exp);
	    }
	  else
	    lastc = 0;
	  cn--;
	}
      if (bp[bn] != cc)
	return res + (bp[bn] < cc);
      bn--;
    }

  if (bn < 0)
    {
      if (lastc != 0)
	return res + 1;
      while (cn >= 0 && cp[cn] == 0)
        cn--;
      return res + (cn >= 0);
    }

  return res; /* remainder from c is 0 */
}
