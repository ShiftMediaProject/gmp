/* mpfr_agm -- arithmetic-geometric mean of two floating-point numbers

Copyright 1999, 2000, 2001, 2002 Free Software Foundation.

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

void 
mpfr_agm (mpfr_ptr r, mpfr_srcptr op2, mpfr_srcptr op1, mp_rnd_t rnd_mode)
{
  int s, go_on, compare;
  mp_prec_t p, q;
  double uo, vo;
  mp_limb_t *up, *vp, *tmpp, *tmpup, *tmpvp, *ap, *bp;
  mpfr_t u, v, tmp, tmpu, tmpv, a, b;
  TMP_DECL(marker);

  /* If a or b is NaN, the result is NaN */
  if (MPFR_IS_NAN(op1) || MPFR_IS_NAN(op2))
    {
      MPFR_SET_NAN(r);
      __mpfr_flags |= MPFR_FLAGS_NAN;
      return;
    }

  /* If a or b is negative (including -Infinity), the result is NaN */
  if ((MPFR_SIGN(op1) < 0) || (MPFR_SIGN(op2) < 0))
    {
      MPFR_SET_NAN(r);
      __mpfr_flags |= MPFR_FLAGS_NAN;
      return;
    }

  MPFR_CLEAR_NAN(r);

  /* If a or b is +Infinity, the result is +Infinity */
  if (MPFR_IS_INF(op1) || MPFR_IS_INF(op2))
    {
      MPFR_SET_INF(r);
      MPFR_SET_SAME_SIGN(r, op1);
      return;
    }

  MPFR_CLEAR_INF(r);
  
  /* If a or b is 0, the result is 0 */
  if ((MPFR_NOTZERO(op1) && MPFR_NOTZERO(op2)) == 0)
    {
      MPFR_SET_ZERO(r);
      return;
    }

 /* precision of the following calculus */
  q = MPFR_PREC(r);
  p = q + 15;

  /* Initialisations */
  go_on=1;
 
  TMP_MARK(marker);
  s=(p-1)/BITS_PER_MP_LIMB+1;
  MPFR_INIT(ap, a, p, s);  
  MPFR_INIT(bp, b, p, s);
  MPFR_INIT(up, u, p, s);
  MPFR_INIT(vp, v, p, s);   
  MPFR_INIT(tmpup, tmpu, p, s);  
  MPFR_INIT(tmpvp, tmpv, p, s);  
  MPFR_INIT(tmpp, tmp, p, s);  



  /* b and a are the 2 operands but we want b >= a */
  if ((compare = mpfr_cmp (op1,op2)) > 0)
    {
      mpfr_set (b,op1,GMP_RNDN);
      mpfr_set (a,op2,GMP_RNDN);  
    }
  else if (compare == 0)
    {
      mpfr_set (r, op1, rnd_mode);
      TMP_FREE(marker);
      return;
    }
  else
    {
      mpfr_set (b,op2,GMP_RNDN);
      mpfr_set (a,op1,GMP_RNDN);  
    }
    
  vo = mpfr_get_d1 (b);
  uo = mpfr_get_d1 (a);

  mpfr_set(u,a,GMP_RNDN);
  mpfr_set(v,b,GMP_RNDN);

  /* Main loop */

  while (go_on) {
    int err, can_round;
    mp_prec_t eq;
    
    err=1 + (int) ((3.0/2.0*(double)_mpfr_ceil_log2((double)p)+1.0)*_mpfr_ceil_exp2(-(double)p)
	     +3.0*_mpfr_ceil_exp2(-2.0*(double)p*uo/(vo-uo)));
    if(p-err-3<=q) {
      p=q+err+4;
      err= 1 + 
	(int) ((3.0/2.0*_mpfr_ceil_log2((double)p)+1.0)*_mpfr_ceil_exp2(-(double)p)
	       +3.0*_mpfr_ceil_exp2(-2.0*(double)p*uo/(vo-uo)));
    }

    /* Calculus of un and vn */
    do
      {
        mpfr_mul(tmp, u, v, GMP_RNDN);
        mpfr_sqrt (tmpu, tmp, GMP_RNDN); 
        mpfr_add(tmp, u, v, GMP_RNDN);
        mpfr_div_2ui(tmpv, tmp, 1, GMP_RNDN);
        mpfr_set(u, tmpu, GMP_RNDN);
        mpfr_set(v, tmpv, GMP_RNDN);
      }
    while (mpfr_cmp2(u, v, &eq) != 0 && eq <= p - 2);

    /* Roundability of the result */
      can_round=mpfr_can_round(v,p-err-3,GMP_RNDN,rnd_mode,q);
    
      if (can_round)
	go_on=0;

      else {
	  go_on=1;
	  p+=5;
	  s=(p-1)/BITS_PER_MP_LIMB+1;
	  MPFR_INIT(up, u, p, s);
	  MPFR_INIT(vp, v, p, s);   
	  MPFR_INIT(tmpup, tmpu, p, s);  
	  MPFR_INIT(tmpvp, tmpv, p, s);  
	  MPFR_INIT(tmpp, tmp, p, s);
	  mpfr_set(u,a,GMP_RNDN);
	  mpfr_set(v,b,GMP_RNDN);
      }
  }
  /* End of while */

  /* Setting of the result */

  mpfr_set(r,v,rnd_mode);

  /* Let's clean */
  TMP_FREE(marker); 

  return ;
}
