/* mpfr_hypot -- Euclidean distance

Copyright 2001, 2002 Free Software Foundation, Inc.

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
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

 /* The computation of hypot of x and y is done by

    hypot(x,y)= sqrt(x^2+y^2) = z
 */

int
mpfr_hypot (mpfr_ptr z, mpfr_srcptr x ,mpfr_srcptr y , mp_rnd_t rnd_mode) 
{
  int inexact;
  /* Flag calcul exacte */
  int not_exact=0;

  /* particular cases */

  if (MPFR_IS_NAN(x) || MPFR_IS_NAN(y))
    {
      MPFR_SET_NAN(z);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(z);

  if (MPFR_IS_INF(x) || MPFR_IS_INF(y))
    {
      MPFR_SET_INF(z);
      MPFR_SET_POS(z);
      MPFR_RET(0);
    }

  MPFR_CLEAR_INF(z);

  if(MPFR_IS_ZERO(x))
    return mpfr_abs (z, y, rnd_mode);

  if(MPFR_IS_ZERO(y))
    return mpfr_abs (z, x, rnd_mode);

  /* General case */

  {
    /* Declaration of the intermediary variable */
    mpfr_t t, te,ti;
    /* Declaration of the size variable */
    mp_prec_t Nx = MPFR_PREC(x);   /* Precision of input variable */
    mp_prec_t Ny = MPFR_PREC(y);   /* Precision of input variable */
    mp_prec_t Nz = MPFR_PREC(z);   /* Precision of input variable */
      
    int  Nt;   /* Precision of the intermediary variable */
    long int err;  /* Precision of error */
      
    /* compute the precision of intermediary variable */
    Nt=MAX(MAX(Nx,Ny),Nz);

    /* compute the size of intermediary variable -- see algorithms.ps */
    Nt=Nt+2+_mpfr_ceil_log2(Nt);

    /* initialise the intermediary variables */
    mpfr_init(t);             
    mpfr_init(te);             
    mpfr_init(ti); 

    /* Hypot */
    do {
      not_exact=0;
      /* reactualisation of the precision */
      mpfr_set_prec(t,Nt);             
      mpfr_set_prec(te,Nt);             
      mpfr_set_prec(ti,Nt);   

      /* computations of hypot */
      if(mpfr_mul(te,x,x,GMP_RNDN))   /* x^2 */
        not_exact=1;

      if(mpfr_mul(ti,y,y,GMP_RNDN))   /* y^2 */
        not_exact=1;          
      
      if(mpfr_add(t,te,ti,GMP_RNDD))  /*x^2+y^2*/
        not_exact=1;

      if(mpfr_sqrt(t,t,GMP_RNDN))     /* sqrt(x^2+y^2)*/
        not_exact=1;
 
      /* estimation of the error */
      err=Nt-(2);
    
      Nt += 10; 
      if(Nt<0)Nt=0;

    } while ((err <0) || ((!mpfr_can_round(t,err,GMP_RNDN,rnd_mode,Nz)) && not_exact));

    inexact = mpfr_set (z, t, rnd_mode);
    mpfr_clear(t);
    mpfr_clear(ti);
    mpfr_clear(te);

  }

  if (not_exact == 0 && inexact == 0)
    return 0;
        
  if (not_exact != 0 && inexact == 0)
    return 1;

  return inexact;
}
