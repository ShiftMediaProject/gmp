/* mpfr_tanh -- hyperbolic tangent

Copyright 2001, 2002 Free Software Foundation.

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

 /* The computation of cosh is done by

    tanh= [e^(x)^2-1]/+[e^(x)^2+1]
 */

int
#if __STDC__
mpfr_tanh (mpfr_ptr y, mpfr_srcptr xt , mp_rnd_t rnd_mode) 
#else
mpfr_tanh (y, xt, rnd_mode)
     mpfr_ptr y;
     mpfr_srcptr xt;
     mp_rnd_t rnd_mode;
#endif
{

    /****** Declaration ******/
    mpfr_t x;
    mp_prec_t Nxt = MPFR_PREC(xt);
    int flag_neg=0, inexact=0;
    
    /* Special value checking */

    if (MPFR_IS_NAN(xt)) 
      {
        MPFR_SET_NAN(y); 
        MPFR_RET_NAN;
      }
    MPFR_CLEAR_NAN(y);

    if (MPFR_IS_INF(xt))
      {
        if (MPFR_SIGN(xt) > 0)
          return mpfr_set_si(y,1,rnd_mode); /* tanh(inf) = 1 */
        else
          return mpfr_set_si(y,-1,rnd_mode); /* tanh(-inf) = -1 */
      }
    MPFR_CLEAR_INF(y);

    /* tanh(0) = 0 */
    if (MPFR_IS_ZERO(xt))
      {              
        MPFR_SET_ZERO(y);
        MPFR_SET_SAME_SIGN(y,xt);
        MPFR_RET(0);
      }

    mpfr_init2(x,Nxt);
    mpfr_set(x,xt,GMP_RNDN);

    if (MPFR_SIGN(x) < 0)
      {
        MPFR_CHANGE_SIGN(x);
        flag_neg=1;
      }

    /* General case */
    {
      /* Declaration of the intermediary variable */
      mpfr_t t, te, ta,tb;       
      int d;

      /* Declaration of the size variable */
      mp_prec_t Nx = Nxt;   /* Precision of input variable */
      mp_prec_t Ny = MPFR_PREC(y);   /* Precision of input variable */

      mp_prec_t Nt;   /* Precision of the intermediary variable */
      long int err;  /* Precision of error */
      
      /* compute the precision of intermediary variable */
      Nt=MAX(Nx,Ny);
      /* the optimal number of bits : see algorithms.ps */
      Nt = Nt+_mpfr_ceil_log2(9)+_mpfr_ceil_log2(Nt);

      /* initialise of intermediary	variable */
      mpfr_init(t); 
      mpfr_init(te);             
      mpfr_init(ta);             
      mpfr_init(tb);                    


      /* First computation of cosh */
      do {

	/* reactualisation of the precision */
	mpfr_set_prec(t,Nt);
	mpfr_set_prec(te,Nt);             
	mpfr_set_prec(ta,Nt);             
	mpfr_set_prec(tb,Nt);             

        /* compute tanh */
        mpfr_mul_2ui(te,x,1,GMP_RNDN);  /* 2x */
        mpfr_exp(te,te,GMP_RNDN);       /* exp(2x) */
        mpfr_add_ui(ta,te,1,GMP_RNDD);  /* exp(2x) + 1*/
        mpfr_sub_ui(tb,te,1,GMP_RNDU);  /* exp(2x) - 1*/
        mpfr_div(t,tb,ta,GMP_RNDN);     /* (exp(2x)-1)/(exp(2x)+1)*/


        /* calculation of the error*/
        d = MPFR_EXP(te)-MPFR_EXP(t);
	
	/* estimation of the error */
        /*err = Nt-(_mpfr_ceil_log2(7+pow(2,d+1)));*/
        err = Nt-(MAX(d+1,3)+1);

	/* actualisation of the precision */
        Nt += 10; 

      } while ((err <0)||!mpfr_can_round(t,err,GMP_RNDN,rnd_mode,Ny));
 
      if (flag_neg==1)
          MPFR_CHANGE_SIGN(t);
 
      inexact = mpfr_set(y,t,rnd_mode);
      mpfr_clear(t);
      mpfr_clear(te);
      mpfr_clear(ta);
      mpfr_clear(tb);
    }
    mpfr_clear(x);
    return inexact;

}

