/* mpfr_agm -- arithmetic-geometric mean of two floating-point numbers

Copyright (C) 1999, 2001 Free Software Foundation.

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

/* returns ceil(log(d)/log(2)) */
long
_mpfr_ceil_log2 (double d)
{
  long exp;
  union ieee_double_extract x;

  x.d = d;
  exp = x.s.exp - 1023;
  x.s.exp = 1023; /* value for 1 <= d < 2 */
  if (x.d != 1.0) exp++;
  return exp;
}

/* returns floor(log(d)/log(2)) */
long
_mpfr_floor_log2 (double d)
{
  union ieee_double_extract x;

  x.d = d;
  return (long) x.s.exp - 1023;
}

/* returns y >= 2^d */
double
_mpfr_ceil_exp2 (double d)
{
  long exp;
  union ieee_double_extract x;

  exp = (long) d;
  if (d != (double) exp) exp++;
  /* now exp = ceil(d) */
  x.d = 1.0;
  if (exp < -1022) exp = -1022;
  x.s.exp = 1023 + exp;
  return x.d;
}

void 
#ifdef __STDC__
mpfr_agm (mpfr_ptr r, mpfr_srcptr op2, mpfr_srcptr op1, mp_rnd_t rnd_mode)
#else
mpfr_agm (r, op2, op1, rnd_mode)
     mpfr_ptr r;
     mpfr_srcptr op2;
     mpfr_srcptr op1;
     mp_rnd_t rnd_mode;
#endif
{
  int s, go_on;
  mp_prec_t p, q;
  double uo, vo;
  mp_limb_t *up, *vp, *tmpp, *tmpup, *tmpvp, *ap, *bp;
  mpfr_t u, v, tmp, tmpu, tmpv, a, b;
  TMP_DECL(marker1);

  /* If a or b is NaN, the result is NaN */
  if (MPFR_IS_NAN(op1) || MPFR_IS_NAN(op2)) 
    { MPFR_SET_NAN(r); return; }

  /* If a or b is negative (including -Infinity), the result is NaN */
  if ((MPFR_SIGN(op1) < 0) || (MPFR_SIGN(op2) < 0))
    { MPFR_SET_NAN(r); return; }

  MPFR_CLEAR_NAN(r);

  /* If a or b is +Infinity, the result is +Infinity */
  if (MPFR_IS_INF(op1) || MPFR_IS_INF(op2))
    { MPFR_SET_INF(r); MPFR_SET_SAME_SIGN(r, op1); return; }

  MPFR_CLEAR_INF(r);
  
  /* If a or b is 0, the result is 0 */
  if ((MPFR_NOTZERO(op1) && MPFR_NOTZERO(op2)) == 0)
    { MPFR_SET_ZERO(r);
    return;
    }

 /* precision of the following calculus */
  q = MPFR_PREC(r);
  p = q + 15;

  /* Initialisations */
  go_on=1;
 
  TMP_MARK(marker1);
  s=(p-1)/BITS_PER_MP_LIMB+1;
  MPFR_INIT(ap, a, p, s);  
  MPFR_INIT(bp, b, p, s);

  {
  TMP_DECL(marker2);
  TMP_MARK(marker2);
  MPFR_INIT(up, u, p, s);
  MPFR_INIT(vp, v, p, s);   
  MPFR_INIT(tmpup, tmpu, p, s);  
  MPFR_INIT(tmpvp, tmpv, p, s);  
  MPFR_INIT(tmpp, tmp, p, s);  



  /* b and a will be the 2 operands but I want b>= a */
  if (mpfr_cmp(op1,op2) > 0) {
    mpfr_set(b,op1,GMP_RNDN); mpfr_set(a,op2,GMP_RNDN);  
  }
  else {
    mpfr_set(b,op2,GMP_RNDN); mpfr_set(a,op1,GMP_RNDN);  
  }
    
  vo=mpfr_get_d(b);
  uo=mpfr_get_d(a);

  mpfr_set(u,a,GMP_RNDN);
  mpfr_set(v,b,GMP_RNDN);
 

  /* Main loop */

  while (go_on) {
    int err,  eq, can_round;
    
    eq=0;
    
    err=1 + (int) ((3.0/2.0*(double)_mpfr_ceil_log2((double)p)+1.0)*_mpfr_ceil_exp2(-(double)p)
	     +3.0*_mpfr_ceil_exp2(-2.0*(double)p*uo/(vo-uo)));
    if(p-err-3<=q) {
      p=q+err+4;
      err= 1 + 
	(int) ((3.0/2.0*_mpfr_ceil_log2((double)p)+1.0)*_mpfr_ceil_exp2(-(double)p)
	       +3.0*_mpfr_ceil_exp2(-2.0*(double)p*uo/(vo-uo)));
    }

    /* Calculus of un and vn */
    while (eq<=p-2) {
      mpfr_mul(tmp,u,v,GMP_RNDN);
      mpfr_sqrt (tmpu, tmp, GMP_RNDN); 
      mpfr_add(tmp,u,v,GMP_RNDN);
      mpfr_div_2exp(tmpv,tmp,1,GMP_RNDN);
      mpfr_set(u,tmpu,GMP_RNDN);
      mpfr_set(v,tmpv,GMP_RNDN);
      if (mpfr_cmp(v,u)>=0)
	eq=mpfr_cmp2(v,u);
      else
	eq=mpfr_cmp2(u,v);
    }

    /*  printf("avant can_round %i bits faux\n v : ",err+3);  
	mpfr_print_raw(v); printf("\n u : ");
	mpfr_print_raw(u);printf("\n");*/ 
    

    /* Roundability of the result */
      can_round=mpfr_can_round(v,p-err-3,GMP_RNDN,rnd_mode,q);
    
      if (can_round)
	go_on=0;

      else {
	  go_on=1;
	  p+=5;
	  TMP_FREE(marker2); 
	  TMP_MARK(marker2);
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

  TMP_FREE(marker2);
  }

  /* Let's clean */
    TMP_FREE(marker1); 
  
  return ;
}

