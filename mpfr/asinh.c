/* mpfr_asinh -- inverse hyperbolic sine

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

 /* The computation of asinh is done by

    asinh = ln(x + sqrt(x^2 + 1))
 */
int
mpfr_asinh (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  int inexact;
  int neg = 0;
  mp_prec_t Nx, Ny, Nt;
  mpfr_t t, te, ti; /* auxiliary variables */
  long int err;

  if (MPFR_IS_NAN(x))
    {  
      MPFR_SET_NAN(y); 
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(y);

  if (MPFR_IS_INF(x))
    { 
      MPFR_SET_INF(y);
      MPFR_SET_SAME_SIGN(y, x);
      MPFR_RET(0);
    }

  MPFR_CLEAR_INF(y);

  if (MPFR_IS_ZERO(x))
    {
      MPFR_SET_ZERO(y);   /* asinh(0) = 0 */
      MPFR_SET_SAME_SIGN(y, x);
      MPFR_RET(0);
    }

  Nx = MPFR_PREC(x);   /* Precision of input variable */
  Ny = MPFR_PREC(y);   /* Precision of output variable */

  neg = MPFR_SIGN(x) < 0;

  /* General case */
    
  /* compute the precision of intermediary variable */
  Nt = MAX(Nx, Ny);

  /* the optimal number of bits : see algorithms.ps */
  Nt = Nt + 4 + __gmpfr_ceil_log2 (Nt);

  /* initialize intermediary variables */
  mpfr_init2 (t, 2);
  mpfr_init2 (te, 2);
  mpfr_init2 (ti, 2);

  mpfr_save_emin_emax ();

  /* First computation of asinh */
  do
    {
      /* reactualisation of the precision */
      mpfr_set_prec (t, Nt);
      mpfr_set_prec (te, Nt);
      mpfr_set_prec (ti, Nt);

      /* compute asinh */
      mpfr_mul (te, x, x, GMP_RNDD);                   /* x^2 */
      mpfr_add_ui (ti, te, 1, GMP_RNDD);               /* x^2+1 */
      mpfr_sqrt (t, ti, GMP_RNDN);                     /* sqrt(x^2+1) */
      (neg ? mpfr_sub : mpfr_add) (t, t, x, GMP_RNDN); /* sqrt(x^2+1)+x */
      mpfr_log (t, t, GMP_RNDN);                       /* ln(sqrt(x^2+1)+x)*/

      /* error estimate -- see algorithms.ps */
      err = Nt - (MAX(3 - MPFR_GET_EXP (t), 0) + 1);

      /* actualisation of the precision */
      Nt += 10;
    }
  while ((err < 0) || (!mpfr_can_round (t, err, GMP_RNDN, GMP_RNDZ,
                                        Ny + (rnd_mode == GMP_RNDN))
                       || MPFR_IS_ZERO(t)));

  mpfr_restore_emin_emax ();
  
  if (neg)
    MPFR_CHANGE_SIGN(t);

  inexact = mpfr_set (y, t, rnd_mode);

  mpfr_clear (t);
  mpfr_clear (ti);
  mpfr_clear (te);

  return mpfr_check_range (y, inexact, rnd_mode);
}
