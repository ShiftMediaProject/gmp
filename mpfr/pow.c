/* mpfr_pow -- power function x^y 

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

/* The computation of z = pow(x,y) is done by
   z = exp(y * log(x)) = x^y */

int
mpfr_pow (mpfr_ptr z, mpfr_srcptr x, mpfr_srcptr y, mp_rnd_t rnd_mode)
{
  int inexact = 0;
 
  if (MPFR_IS_NAN(x) || MPFR_IS_NAN(y))
    {
      MPFR_SET_NAN(z);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_INF(y))
    {
      mpfr_t one;
      int cmp;

      if (MPFR_SIGN(y) > 0)
        {
          if (MPFR_IS_INF(x))
            {
              MPFR_CLEAR_FLAGS(z);
              if (MPFR_SIGN(x) > 0)
                MPFR_SET_INF(z);
              else
                MPFR_SET_ZERO(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          MPFR_CLEAR_FLAGS(z);
          if (MPFR_IS_ZERO(x))
            {
              MPFR_SET_ZERO(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          mpfr_init2(one, BITS_PER_MP_LIMB);
          mpfr_set_ui(one, 1, GMP_RNDN);
          cmp = mpfr_cmp_abs(x, one);
          mpfr_clear(one);
          if (cmp > 0)
            {
              MPFR_SET_INF(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          else if (cmp < 0)
            {
              MPFR_SET_ZERO(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          else
            {
              MPFR_SET_NAN(z);
              MPFR_RET_NAN;
            }
        }
      else
        {
          if (MPFR_IS_INF(x))
            {
              MPFR_CLEAR_FLAGS(z);
              if (MPFR_SIGN(x) > 0)
                MPFR_SET_ZERO(z);
              else
                MPFR_SET_INF(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          if (MPFR_IS_ZERO(x))
            {
              MPFR_SET_INF(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          mpfr_init2(one, BITS_PER_MP_LIMB);
          mpfr_set_ui(one, 1, GMP_RNDN);
          cmp = mpfr_cmp_abs(x, one);
          mpfr_clear(one);
          MPFR_CLEAR_FLAGS(z);
          if (cmp > 0)
            {
              MPFR_SET_ZERO(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          else if (cmp < 0)
            {
              MPFR_SET_INF(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          else
            {
              MPFR_SET_NAN(z);
              MPFR_RET_NAN;
            }
        }
    }

  if (MPFR_IS_ZERO(y))
    {
      return mpfr_set_ui(z,1,GMP_RNDN);
    }

  if(mpfr_isinteger(y))
    {
      mpz_t zi;
      long int zii;
      int exptol;
    
      mpz_init(zi);  
      exptol = mpfr_get_z_exp (zi, y);
        
      if (exptol>0)
        mpz_mul_2exp(zi, zi, exptol);
      else
        mpz_tdiv_q_2exp(zi, zi, (unsigned long int) (-exptol));

      zii=mpz_get_ui(zi);
        
      mpz_clear(zi);
      return mpfr_pow_si(z,x,zii,rnd_mode); 
    }

  if (MPFR_IS_INF(x))
    {
      if (MPFR_SIGN(x) > 0)
        {
          MPFR_CLEAR_FLAGS(z);
          if (MPFR_SIGN(y) > 0)
            MPFR_SET_INF(z);
          else
            MPFR_SET_ZERO(z);
          MPFR_SET_POS(z);
          MPFR_RET(0);
        }
      else
        {
          MPFR_SET_NAN(z);
          MPFR_RET_NAN;
        }
    }

  if (MPFR_IS_ZERO(x))
    {
      MPFR_CLEAR_FLAGS(z);
      MPFR_SET_ZERO(z);
      MPFR_SET_SAME_SIGN(z, x);
      MPFR_RET(0);
    }

  if (MPFR_SIGN(x) < 0)
    {
      MPFR_SET_NAN(z);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_FLAGS(z);

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
      mpfr_init(ti);
      mpfr_init(te);             

      do {

	/* reactualisation of the precision */
	mpfr_set_prec(t,Nt);                    
	mpfr_set_prec(ti,Nt);                    
   	mpfr_set_prec(te,Nt);             

	/* compute   exp(y*ln(x))*/
        mpfr_log(ti,x,GMP_RNDU);         /* ln(n) */
        mpfr_mul(te,y,ti,GMP_RNDU);       /* y*ln(n) */
        mpfr_exp(t,te,GMP_RNDN);         /* exp(x*ln(n))*/

	/* estimation of the error -- see pow function in algorithms.ps*/
	err = Nt - (MPFR_EXP(te)+3);

	/* actualisation of the precision */
	Nt += 10;

      } while (err<0 || !mpfr_can_round(t,err,GMP_RNDN,rnd_mode,Ny));
      inexact = mpfr_set(z,t,rnd_mode);
      mpfr_clear(t);
      mpfr_clear(ti);
      mpfr_clear(te);
    }
    return inexact;
}







