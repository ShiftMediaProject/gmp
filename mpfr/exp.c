/* mpfr_exp -- exponential of a floating-point number

Copyright (C) 1999 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* #define DEBUG */

extern int mpfr_exp2 _PROTO((mpfr_ptr, mpfr_srcptr, mp_rnd_t)); 
extern int mpfr_exp3 _PROTO((mpfr_ptr, mpfr_srcptr, mp_rnd_t)); 


/* use Brent's formula exp(x) = (1+r+r^2/2!+r^3/3!+...)^(2^K)*2^n
   where x = n*log(2)+(2^K)*r
   number of operations = O(K+prec(r)/K)
*/
int 
#if __STDC__
mpfr_exp (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
#else
mpfr_exp (y, x, rnd_mode)
     mpfr_ptr y;
     mpfr_srcptr x;
     mp_rnd_t rnd_mode;
#endif
{
  int expx, precy; 

  if (MPFR_IS_NAN(x)) { MPFR_SET_NAN(y); return 1; }

  MPFR_CLEAR_NAN(y);

  if (MPFR_IS_INF(x)) 
    { 
      if (MPFR_SIGN(x) > 0) {
	MPFR_SET_INF(y);
	if (MPFR_SIGN(y) < 0) MPFR_CHANGE_SIGN(y);
      }
      else {
	MPFR_CLEAR_INF(y);
	MPFR_SET_ZERO(y);
	if (MPFR_SIGN(y) < 0) MPFR_CHANGE_SIGN(y);
      }
      /*    TODO: conflits entre infinis et zeros ? */
      return 1;
    }

  MPFR_CLEAR_INF(y);
      
  if (!MPFR_NOTZERO(x)) { mpfr_set_ui(y, 1, GMP_RNDN); return 0; }

  expx = MPFR_EXP(x);
  precy = MPFR_PREC(y);

  /* if x > (2^31-1)*ln(2), then exp(x) > 2^(2^31-1) i.e. gives +infinity */
  if (expx > 30) {
    if (MPFR_SIGN(x)>0) { printf("+infinity"); return 1; }
    else { MPFR_SET_ZERO(y); return 1; }
  }

  /* if x < 2^(-precy), then exp(x) i.e. gives 1 +/- 1 ulp(1) */
  if (expx < -precy) { int signx = MPFR_SIGN(x);
    mpfr_set_ui(y, 1, rnd_mode);
    if (signx>0 && rnd_mode==GMP_RNDU) mpfr_add_one_ulp(y);
    else if (signx<0 && (rnd_mode==GMP_RNDD || rnd_mode==GMP_RNDZ)) 
      mpfr_sub_one_ulp(y);
    return 1; }

  if (precy > 13000) mpfr_exp3(y, x, rnd_mode); /* O(M(n) log(n)^2) */
  else mpfr_exp2(y, x, rnd_mode); /* O(n^(1/3) M(n)) */
  return 1;
}

