/* mpfr_exp2 -- power of 2 function 2^y 

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

 /* The computation of y=pow(2,z) is done by

    y=exp(z*log(2))=2^z
 */

int
mpfr_exp2 (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
{    

    int inexact =0;

    if (MPFR_IS_NAN(x))
      {
        MPFR_SET_NAN(y);
        MPFR_RET_NAN;
      }

    MPFR_CLEAR_NAN(y);

    if (MPFR_IS_INF(x))
      {
        if (MPFR_SIGN(x) > 0)
          {
            MPFR_SET_INF(y);
          }
        else
          {
            MPFR_CLEAR_INF(y);
            MPFR_SET_ZERO(y);
          }
        MPFR_SET_POS(y);
        MPFR_RET(0);
      }

    /* 2^0 = 1 */
    if (MPFR_IS_ZERO(x))
      return mpfr_set_ui (y, 1, rnd_mode);

    /* since the smallest representable non-zero float is 1/2*2^__mpfr_emin,
       if x < __mpfr_emin - 1, the result is either 1/2*2^__mpfr_emin or 0 */
    if (mpfr_cmp_si_2exp (x, __mpfr_emin - 1, 0) < 0)
      return mpfr_set_underflow (y, rnd_mode, 1);

    /* General case */
    {
    /* Declaration of the intermediary variable */
      mpfr_t t, te;

      /* Declaration of the size variable */
      mp_prec_t Nx = MPFR_PREC(x);   /* Precision of input variable */
      mp_prec_t Ny = MPFR_PREC(y);   /* Precision of input variable */

      mp_prec_t Nt;   /* Precision of the intermediary variable */
      long int err;  /* Precision of error */
                
      /* compute the precision of intermediary variable */
      Nt = MAX(Nx, Ny);
      /* the optimal number of bits : see algorithms.ps */
      Nt = Nt + 5 + _mpfr_ceil_log2 (Nt);


      /* initialise of intermediary	variable */
      mpfr_init (t);
      mpfr_init (te);

      /* First computation */
      do {

	/* reactualisation of the precision */
	mpfr_set_prec (t, Nt);             
	mpfr_set_prec (te, Nt);             

	/* compute   exp(x*ln(2))*/
        mpfr_const_log2 (t, GMP_RNDU);    /* ln(2) */
        mpfr_mul (te, x, t, GMP_RNDU);    /* x*ln(2) */
        mpfr_exp (t, te, GMP_RNDN);       /* exp(x*ln(2))*/

	/* estimate of the error -- see pow function in algorithms.ps*/
	err = Nt - (MPFR_EXP(te) + 2);

	/* actualisation of the precision */
	Nt += _mpfr_isqrt (Nt) + 10;

      } while ((err < 0) || !mpfr_can_round (t, err, GMP_RNDN, rnd_mode, Ny));
 
      inexact = mpfr_set (y, t, rnd_mode);

      mpfr_clear (t);
      mpfr_clear (te);
    }
    return inexact;

}
