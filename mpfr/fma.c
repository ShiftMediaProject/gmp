/* mpfr_fma -- Floating multiply-add

Copyright 2001, 2002 Free Software Foundation, Inc.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute
it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software
Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser
General Public License along with the MPFR Library; see
the file COPYING.LIB.  If not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"


/* The computation of fma of x y and u is done by
    fma(s,x,y,z)= z + x*y = s
*/

int
mpfr_fma (mpfr_ptr s, mpfr_srcptr x, mpfr_srcptr y, mpfr_srcptr z,
          mp_rnd_t rnd_mode)
{
  int inexact;
  mpfr_t u;

  /* particular cases */
  if (MPFR_UNLIKELY( MPFR_IS_SINGULAR(x) ||
		     MPFR_IS_SINGULAR(y) ||
		     MPFR_IS_SINGULAR(z) ))
    {
      if (MPFR_IS_NAN(x) || MPFR_IS_NAN(y) || MPFR_IS_NAN(z))
	{
	  MPFR_SET_NAN(s);
	  MPFR_RET_NAN;
	}
      else if (MPFR_IS_INF(x) || MPFR_IS_INF(y))
	{
	  /* cases Inf*0+z, 0*Inf+z, Inf-Inf */
	  if ((MPFR_IS_ZERO(y)) ||
	      (MPFR_IS_ZERO(x)) ||
	      (MPFR_IS_INF(z) && 
	       ((MPFR_MULT_SIGN(MPFR_SIGN(x), MPFR_SIGN(y))) != MPFR_SIGN(z))))
	    {
	      MPFR_SET_NAN(s);
	      MPFR_RET_NAN;
	    }
	  else if (MPFR_IS_INF(z)) /* case Inf-Inf already checked above */
	    {
	      MPFR_SET_INF(s);
	      MPFR_SET_SAME_SIGN(s, z);
	      MPFR_RET(0);
	    }
	  else /* z is finite */
	    {
	      MPFR_SET_INF(s);
	      MPFR_SET_SIGN(s, MPFR_MULT_SIGN(MPFR_SIGN(x) , MPFR_SIGN(y)));
	      MPFR_RET(0);
	    }
	}
      /* now x and y are finite */
      else if (MPFR_IS_INF(z))
	{
	  MPFR_SET_INF(s);
	  MPFR_SET_SAME_SIGN(s, z);
	  MPFR_RET(0);
	}
      else if (MPFR_IS_ZERO(x) || MPFR_IS_ZERO(y))
	{
	  if (MPFR_IS_ZERO(z))
	    {
	      int sign_p;
	      sign_p = MPFR_MULT_SIGN( MPFR_SIGN(x) , MPFR_SIGN(y) );
	      MPFR_SET_SIGN(s,(rnd_mode != GMP_RNDD ?
			       ((MPFR_IS_NEG_SIGN(sign_p) && MPFR_IS_NEG(z))
				? -1 : 1) :
			       ((MPFR_IS_POS_SIGN(sign_p) && MPFR_IS_POS(z))
				? 1 : -1)));
	      MPFR_SET_ZERO(s);
	      MPFR_RET(0);
	    }
	  else
	    return mpfr_set (s, z, rnd_mode);
	}
      else if (MPFR_IS_ZERO(z))
	return mpfr_mul (s, x, y, rnd_mode);
      else
	MPFR_ASSERTN(0);
    }
  /* Useless since it is done by mpfr_add
   * MPFR_CLEAR_FLAGS(s); */

  /* if we take prec(u) >= prec(x) + prec(y), the product
     u <- x*y is always exact */
  mpfr_init2 (u, MPFR_PREC(x) + MPFR_PREC(y));
  mpfr_mul (u, x, y, GMP_RNDN); /* always exact */
  inexact = mpfr_add (s, z, u, rnd_mode);
  mpfr_clear(u);

  return inexact;
}
