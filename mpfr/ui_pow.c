/* mpfr_ui_pow -- power of n function n^x

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

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"


 /* The computation of y=pow(n,z) is done by

    y=exp(z*log(n))=n^z
 */

int
mpfr_ui_pow (mpfr_ptr y, unsigned long int n, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
    int inexact;

    if (MPFR_IS_NAN(x))
      {
        MPFR_SET_NAN(y); 
        MPFR_RET_NAN;
      }

    MPFR_CLEAR_NAN(y);

    if (MPFR_IS_INF(x))
      {
        if (MPFR_SIGN(x) < 0)
          {
            MPFR_CLEAR_INF(y);
            MPFR_SET_ZERO(y);
          }
        else
          {
            MPFR_SET_INF(y);
          }
        MPFR_SET_POS(y);
        MPFR_RET(0);
      }

    /* n^0 = 1 */
    if (MPFR_IS_ZERO(x))
      {
        return mpfr_set_ui(y,1,rnd_mode); 
      }

    /* General case */
    {
    /* Declaration of the intermediary variable */
      mpfr_t t, te, ti;

      /* Declaration of the size variable */
      mp_prec_t Nx = MPFR_PREC(x);   /* Precision of input variable */
      mp_prec_t Ny = MPFR_PREC(y);   /* Precision of input variable */

      mp_prec_t Nt;   /* Precision of the intermediary variable */
      long int err;  /* Precision of error */
                
      /* compute the precision of intermediary variable */
      Nt=MAX(Nx,Ny);
      /* the optimal number of bits : see algorithms.ps */
      Nt=Nt+5+_mpfr_ceil_log2(Nt);

      /* initialise of intermediary	variable */
      mpfr_init(t);
      mpfr_init2(ti,sizeof(unsigned long int)*8); /* 8 = CHAR_BIT */                          
      mpfr_init(te);             

      do {

	/* reactualisation of the precision */
	mpfr_set_prec(t,Nt);                    
   	mpfr_set_prec(te,Nt);             

	/* compute   exp(x*ln(n))*/
        mpfr_set_ui(ti,n,GMP_RNDN);      /* ti <- n*/
        mpfr_log(t,ti,GMP_RNDU);         /* ln(n) */
        mpfr_mul(te,x,t,GMP_RNDU);       /* x*ln(n) */
        mpfr_exp(t,te,GMP_RNDN);         /* exp(x*ln(n))*/

	/* estimation of the error -- see pow function in algorithms.ps*/
	err = Nt - (MPFR_EXP(te)+3);

	/* actualisation of the precision */
	Nt += 10;

      } while (err<0 || !mpfr_can_round(t,err,GMP_RNDN,rnd_mode,Ny));
 
      inexact = mpfr_set(y,t,rnd_mode);
      mpfr_clear(t);
      mpfr_clear(ti);
      mpfr_clear(te);
    }
    return inexact;

}
