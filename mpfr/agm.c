/* mpfr_agm -- arithmetic-geometric mean of two floating-point numbers

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

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
#include <math.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"


/*Memory gestion */
#define MON_INIT(xp, x, p, s) xp = (mp_ptr) TMP_ALLOC(s*BYTES_PER_MP_LIMB);    x -> _mp_prec = p; x -> _mp_d = xp; x -> _mp_size = s; x -> _mp_exp = 0; 




void 
#ifdef __STDC__
mpfr_agm(mpfr_ptr r, mpfr_srcptr op2, mpfr_srcptr op1, unsigned char rnd_mode)
#else
mpfr_agm(r, a, b, rnd_mode)
     mpfr_ptr r; 
     mpfr_srcptr a; 
     mpfr_srcptr b; 
     unsigned char rnd_mode; 
#endif
{
  int s, p, q, go_on;
  double uo, vo;
  mp_limb_t *up, *vp, *tmpp, *tmpup, *tmpvp, *ap, *bp;
  mpfr_t u, v, tmp, tmpu, tmpv, a, b;
  TMP_DECL(marker1);
  TMP_DECL(marker2);


  /* If a or b is NaN, the result is NaN */
  if (FLAG_NAN(op1) || FLAG_NAN(op2)) 
    { SET_NAN(r); return; }


  /* If a or b is negative, the result is NaN */
  if ((SIGN(op1)<0)||(SIGN(op2)<0))
    { SET_NAN(r); return; }


  
  /* If a or b is 0, the result is 0 */
  if ((SIGN(op1)==0)||(SIGN(op2)==0)) 
    { SET_ZERO(r);
    return;
    }

 /* precision of the following calculus */
  q = PREC(r);
  p = q + 15;


  /* Initialisations */
  go_on=1;
 
  TMP_MARK(marker1);
  s=(p-1)/BITS_PER_MP_LIMB+1;
  MON_INIT(ap, a, p, s);  
  MON_INIT(bp, b, p, s);
  TMP_MARK(marker2);
  MON_INIT(up, u, p, s);
  MON_INIT(vp, v, p, s);   
  MON_INIT(tmpup, tmpu, p, s);  
  MON_INIT(tmpvp, tmpv, p, s);  
  MON_INIT(tmpp, tmp, p, s);  



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
    
    err=ceil((3.0/2.0*log((double)p)/log(2.0)+1.0)*exp(-(double)p*log(2.0))+3.0*exp(-2.0*(double)p*uo*log(2.0)/(vo-uo)));
    if(p-err-3<=q) {
      p=q+err+4;
      err=ceil((3.0/2.0*log((double)p)/log(2.0)+1.0)*exp(-(double)p*log(2.0))+3.0*exp(-2.0*(double)p*uo*log(2.0)/(vo-uo)));
    }

    /* Calculus of un and vn */
    while (eq<=p-2) {
      mpfr_mul(tmp,u,v,GMP_RNDN);
      mpfr_sqrt(tmpu,tmp,GMP_RNDN); 
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
	  MON_INIT(up, u, p, s);
	  MON_INIT(vp, v, p, s);   
	  MON_INIT(tmpup, tmpu, p, s);  
	  MON_INIT(tmpvp, tmpv, p, s);  
	  MON_INIT(tmpp, tmp, p, s);
	  mpfr_set(u,a,GMP_RNDN);
	  mpfr_set(v,b,GMP_RNDN);
      }
  }
  /* End of while */

  /* Setting of the result */

    mpfr_set(r,v,rnd_mode);


  /* Let's clean */
    TMP_FREE(marker1); 
  
  return ;
}

