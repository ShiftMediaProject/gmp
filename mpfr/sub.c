/* mpfr_sub -- subtract two floating-point numbers

Copyright 2001, 2002, 2003 Free Software Foundation.
Contributed by the Spaces project, INRIA Lorraine.

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
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_sub (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, mp_rnd_t rnd_mode)
{
  if (MPFR_ARE_SINGULAR(b,c))
    {
      if (MPFR_IS_NAN(b) || MPFR_IS_NAN(c))
	{
	  MPFR_SET_NAN(a);
	  MPFR_RET_NAN;
	}
      else if (MPFR_IS_INF(b))
	{
	  if (!MPFR_IS_INF(c) || MPFR_SIGN(b) != MPFR_SIGN(c))
	    {
	      MPFR_SET_INF(a);
	      MPFR_SET_SAME_SIGN(a, b);
	      MPFR_RET(0); /* exact */
	    }
	  else
	    {
	      MPFR_SET_NAN(a);
	      MPFR_RET_NAN;
	    }
	}
      else if (MPFR_IS_INF(c))
	{
	  MPFR_SET_INF(a);
	  if (MPFR_SIGN(c) == MPFR_SIGN(a))
	    MPFR_CHANGE_SIGN(a);
	  MPFR_RET(0); /* exact */
	}
      else if (MPFR_IS_ZERO(b))
	{
	  if (MPFR_IS_ZERO(c))
	    {
	      if (MPFR_SIGN(a) !=
		  (rnd_mode != GMP_RNDD ?
		   ((MPFR_IS_NEG(b) && MPFR_IS_POS(c)) ? -1 : 1) :
		   ((MPFR_IS_POS(b) && MPFR_IS_NEG(c)) ? 1 : -1)))
		MPFR_CHANGE_SIGN(a);
	      MPFR_SET_ZERO(a);
	      MPFR_RET(0); /* 0 - 0 is exact */
	    }
	  return mpfr_neg (a, c, rnd_mode);
	}
      else if (MPFR_IS_ZERO(c))
	{
	  return mpfr_set (a, b, rnd_mode);
	}
      /* Should never reach here */
      MPFR_ASSERTN(0);
    }
  MPFR_CLEAR_FLAGS(a);
  MPFR_ASSERTD(MPFR_IS_PURE_FP(b) && MPFR_IS_PURE_FP(c));

  if (MPFR_LIKELY(MPFR_SIGN(b) == MPFR_SIGN(c)))
    { /* signs are equal, it's a real subtraction */
      if (MPFR_LIKELY(MPFR_PREC(a) == MPFR_PREC(b)
                      && MPFR_PREC(b) == MPFR_PREC(c)))
        return mpfr_sub1sp(a,b,c,rnd_mode);
      else
	return mpfr_sub1(a, b, c, rnd_mode);
    }
  else
    { /* signs differ, it's an addition */
      if (MPFR_GET_EXP(b) < MPFR_GET_EXP(c))
         { /* exchange rounding modes towards +/- infinity */
          int inexact;
          if (rnd_mode == GMP_RNDU)
            rnd_mode = GMP_RNDD;
          else if (rnd_mode == GMP_RNDD)
            rnd_mode = GMP_RNDU;
          inexact = mpfr_add1(a, c, b, rnd_mode);
          MPFR_CHANGE_SIGN(a);
          return -inexact;
        }
      else
        {
          return mpfr_add1(a, b, c, rnd_mode);
        }
    }
}
