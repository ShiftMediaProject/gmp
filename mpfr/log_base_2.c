/* mpfr_log2 -- log base 2

Copyright (C) 2001 Free Software Foundation, Inc.

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

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

 /* The computation of r=log2(a)

    r=log2(a)=log(a)/log(2)
 */

int
mpfr_log2 (mpfr_ptr r, mpfr_srcptr a , mp_rnd_t rnd_mode) 
{

  int inexact = 0;

  /* If a is NaN, the result is NaN */
  if (MPFR_IS_NAN(a))
    {  
      MPFR_SET_NAN(r);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(r);

  /* If a is negative, the result is NaN */
  if (MPFR_SIGN(a) < 0)
    {
      if (!MPFR_IS_INF(a) && MPFR_IS_ZERO(a)) 
      {
        MPFR_SET_INF(r); 
        if (MPFR_SIGN(r) > 0)
          MPFR_CHANGE_SIGN(r);
        return 0; 
      }
      else
      {
        MPFR_SET_NAN(r);
        MPFR_RET_NAN;
      }
    }

  /* check for infinity before zero */
  if (MPFR_IS_INF(a))
    {      
      MPFR_SET_INF(r);
      if(MPFR_SIGN(r) < 0)
        MPFR_CHANGE_SIGN(r);
      return 0;
    }

  /* Now we can clear the flags without damage even if r == a */

  MPFR_CLEAR_INF(r); 

  if (MPFR_IS_ZERO(a)) 
    {
      MPFR_CLEAR_FLAGS(r);
      MPFR_SET_INF(r); 
      if (MPFR_SIGN(r) > 0)
	MPFR_CHANGE_SIGN(r);
       /* Execption GMP*/
      return 0; 
    }

  /* If a is 1, the result is 0 */
  if (mpfr_cmp_ui(a,1) == 0)
    {
      MPFR_CLEAR_FLAGS(r);
      MPFR_SET_SAME_SIGN(r,a);
      MPFR_SET_ZERO(r);
      return 0; 
    }

  /* If a is integer, log2(a) is exact*/
  if (mpfr_cmp_ui_2exp(a,1,MPFR_EXP(a)-1) == 0)
      return mpfr_set_si(r,MPFR_EXP(a)-1,rnd_mode); 


 /* General case */
  {
    /* Declaration of the intermediary variable */
    mpfr_t t, tt;

    /* Declaration of the size variable */
    mp_prec_t Nx = MPFR_PREC(a);   /* Precision of input variable */
    mp_prec_t Ny = MPFR_PREC(r);   /* Precision of input variable */
    
    mp_prec_t Nt;   /* Precision of the intermediary variable */
    long int err;  /* Precision of error */
                

    /* compute the precision of intermediary variable */
    Nt=MAX(Nx,Ny);
    /* the optimal number of bits : see algorithms.ps */
    Nt=Nt+3+_mpfr_ceil_log2(Nt);

    /* initialise of intermediary	variable */
    mpfr_init(t);             
    mpfr_init(tt);             

    
    /* First computation of log2 */
    do {

      /* reactualisation of the precision */
      mpfr_set_prec(t,Nt);             
      mpfr_set_prec(tt,Nt);             
      
      /* compute log2 */
      mpfr_const_log2(t,GMP_RNDD); /* log(2) */
      mpfr_log(tt,a,GMP_RNDN);     /* log(a) */
      mpfr_div(t,tt,t,GMP_RNDN); /* log(a)/log(2) */


      /* estimation of the error */
      err=Nt-3;

      /* actualisation of the precision */
      Nt += 10;
    } while ((err<0) || !mpfr_can_round(t,err,GMP_RNDN,rnd_mode,Ny));
 
      inexact = mpfr_set(r,t,rnd_mode);

      mpfr_clear(t);
      mpfr_clear(tt);
  }
  return inexact;
}
