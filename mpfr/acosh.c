/* mpfr_acosh -- inverse hyperbolic cosine

Copyright 2001, 2002, 2003 Free Software Foundation.

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

 /* The computation of acosh is done by

    acosh= ln(x + sqrt(x^2-1))
 */

int
mpfr_acosh (mpfr_ptr y, mpfr_srcptr x , mp_rnd_t rnd_mode) 
{
    
  int inexact = 0;
  int comp;

  if (MPFR_IS_NAN(x) || (comp = mpfr_cmp_ui (x, 1)) < 0)
    {
      MPFR_SET_NAN(y); 
      MPFR_RET_NAN;
    }
    
  MPFR_CLEAR_NAN(y);

  if (comp == 0)
    {
      MPFR_SET_ZERO(y); /* acosh(1) = 0 */
      MPFR_SET_POS(y);
      MPFR_RET(0);
    }
  
  if (MPFR_IS_INF(x))
    { 
      MPFR_SET_INF(y);
      MPFR_SET_POS(y);
      MPFR_RET(0);
    }

  MPFR_CLEAR_INF(y);

  /* General case */
  {
    /* Declaration of the intermediary variables */
    mpfr_t t, te, ti;
    
    /* Declaration of the size variables */
    mp_prec_t Nx = MPFR_PREC(x);   /* Precision of input variable */
    mp_prec_t Ny = MPFR_PREC(y);   /* Precision of output variable */
    
    mp_prec_t Nt;   /* Precision of the intermediary variable */
    int err;  /* Precision of error */
                
    /* compute the precision of intermediary variable */
    Nt = MAX(Nx, Ny);
    /* the optimal number of bits : see algorithms.ps */
    Nt = Nt + 4 + __gmpfr_ceil_log2 (Nt);

    /* initialization of intermediary variables */
    mpfr_init (t);
    mpfr_init (te);
    mpfr_init (ti);

    mpfr_save_emin_emax ();

    /* First computation of acosh */
    do
      {
        /* reactualisation of the precision */
        mpfr_set_prec (t, Nt);
        mpfr_set_prec (te, Nt);
        mpfr_set_prec (ti, Nt);

        /* compute acosh */
        mpfr_mul (te, x, x, GMP_RNDD);     /* x^2 */
        mpfr_sub_ui (ti, te, 1, GMP_RNDD); /* x^2-1 */
        mpfr_sqrt (t, ti, GMP_RNDN);       /* sqrt(x^2-1) */
        mpfr_add (t, t, x, GMP_RNDN);      /* sqrt(x^2-1)+x */
        mpfr_log (t, t, GMP_RNDN);         /* ln(sqrt(x^2-1)+x)*/

        /* error estimate -- see algorithms.ps */
        err = Nt - (-1 + 2 * MAX(2 + MAX(2 - MPFR_GET_EXP (t),
                                         1 + MPFR_GET_EXP (te)
                                         - MPFR_GET_EXP (ti)
                                         - MPFR_GET_EXP (t)), 0));

        /* actualisation of the precision */
        Nt += 10;
      }
    while ((err < 0) || !mpfr_can_round (t, err, GMP_RNDN, rnd_mode, Ny));
 
    inexact = mpfr_set (y, t, rnd_mode);

    mpfr_clear (t);
    mpfr_clear (ti);
    mpfr_clear (te);
  }

  mpfr_restore_emin_emax ();

  return mpfr_check_range (y, inexact, rnd_mode);
}











