/* mpfr_pow -- power function x^y 

Copyright 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* return non zero iff x^y is exact.
   Assumes x and y are ordinary numbers (neither NaN nor Inf),
   and y is not zero.
*/
static int
mpfr_pow_is_exact (mpfr_srcptr x, mpfr_srcptr y)
{
  mp_exp_t d;
  unsigned long i, c;
  mp_limb_t *yp;
  mp_size_t ysize;
  
  if ((mpfr_sgn (x) < 0) && (mpfr_integer_p (y) == 0))
    return 0;

  if (mpfr_sgn (y) < 0)
    return mpfr_cmp_si_2exp (x, MPFR_SIGN(x), MPFR_GET_EXP (x) - 1) == 0;

  /* compute d such that y = c*2^d with c odd integer */
  ysize = 1 + (MPFR_PREC(y) - 1) / BITS_PER_MP_LIMB;
  d = MPFR_GET_EXP (y) - ysize * BITS_PER_MP_LIMB;
  /* since y is not zero, necessarily one of the mantissa limbs is not zero,
     thus we can simply loop until we find a non zero limb */
  yp = MPFR_MANT(y);
  for (i = 0; yp[i] == 0; i++, d += BITS_PER_MP_LIMB);
  /* now yp[i] is not zero */
  count_trailing_zeros (c, yp[i]);
  d += c;
  
  if (d < 0)
    {
      mpz_t a;
      mp_exp_t b;

      mpz_init (a);
      b = mpfr_get_z_exp (a, x); /* x = a * 2^b */
      c = mpz_scan1 (a, 0);
      mpz_div_2exp (a, a, c);
      b += c;
      /* now a is odd */
      while (d != 0)
        {
          if (mpz_perfect_square_p (a))
            {
              d++;
              mpz_sqrt (a, a);
            }
          else
            {
              mpz_clear (a);
              return 0;
            }
        }
      mpz_clear (a);
    }

    return 1;
}

/* Return 1 if y is an odd integer, 0 otherwise. */
static int
is_odd (mpfr_srcptr y)
{
  mp_exp_t expo;
  mp_prec_t prec;
  mp_size_t yn;
  mp_limb_t *yp;

  MPFR_ASSERTN(MPFR_IS_FP(y));

  if (MPFR_IS_ZERO(y))
    return 0;

  expo = MPFR_GET_EXP (y);
  if (expo <= 0)
    return 0;  /* |y| < 1 and not 0 */

  prec = MPFR_PREC(y);
  if (expo > prec)
    return 0;  /* y is a multiple of 2^(expo-prec), thus not odd */

  /* 0 < expo <= prec */

  yn = (prec - 1) / BITS_PER_MP_LIMB;  /* index of last limb */
  yn -= (mp_size_t) (expo / BITS_PER_MP_LIMB);
  MPFR_ASSERTN(yn >= 0);
  /* now the index of the last limb containing bits of the fractional part */

  yp = MPFR_MANT(y);
  if (expo % BITS_PER_MP_LIMB == 0 ? (yp[yn+1] & 1) == 0 || yp[yn] != 0
      : yp[yn] << ((expo % BITS_PER_MP_LIMB) - 1) != MPFR_LIMB_HIGHBIT)
    return 0;
  while (--yn >= 0)
    if (yp[yn] != 0)
      return 0;
  return 1;
}

/* The computation of z = pow(x,y) is done by
   z = exp(y * log(x)) = x^y
   For the special cases, see Section F.9.4.4 of the C standard:
     _ pow(±0, y) = ±inf for y an odd integer < 0.
     _ pow(±0, y) = +inf for y < 0 and not an odd integer.
     _ pow(±0, y) = ±0 for y an odd integer > 0.
     _ pow(±0, y) = +0 for y > 0 and not an odd integer.
     _ pow(-1, ±inf) = 1.
     _ pow(+1, y) = 1 for any y, even a NaN.
     _ pow(x, ±0) = 1 for any x, even a NaN.
     _ pow(x, y) = NaN for finite x < 0 and finite non-integer y.
     _ pow(x, -inf) = +inf for |x| < 1.
     _ pow(x, -inf) = +0 for |x| > 1.
     _ pow(x, +inf) = +0 for |x| < 1.
     _ pow(x, +inf) = +inf for |x| > 1.
     _ pow(-inf, y) = -0 for y an odd integer < 0.
     _ pow(-inf, y) = +0 for y < 0 and not an odd integer.
     _ pow(-inf, y) = -inf for y an odd integer > 0.
     _ pow(-inf, y) = +inf for y > 0 and not an odd integer.
     _ pow(+inf, y) = +0 for y < 0.
     _ pow(+inf, y) = +inf for y > 0. */
int
mpfr_pow (mpfr_ptr z, mpfr_srcptr x, mpfr_srcptr y, mp_rnd_t rnd_mode)
{
  int inexact = 0;

  /* pow(x, ±0) returns 1 for any x, even a NaN. */
  if (MPFR_IS_FP(y) && MPFR_IS_ZERO(y))
    return mpfr_set_ui (z, 1, GMP_RNDN);

  if (MPFR_IS_NAN(x))
    {
      MPFR_SET_NAN(z);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_NAN(y))
    {
      /* pow(+1, NaN) returns 1. */
      if (MPFR_IS_FP(x) && mpfr_cmp_ui(x, 1) == 0)
        return mpfr_set_ui (z, 1, GMP_RNDN);

      MPFR_SET_NAN(z);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_INF(y))
    {
      if (MPFR_IS_INF(x))
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
          mpfr_t one;
          int cmp;

          mpfr_init2(one, BITS_PER_MP_LIMB);
          mpfr_set_ui(one, 1, GMP_RNDN);
          cmp = mpfr_cmpabs(x, one) * MPFR_SIGN(y);
          mpfr_clear(one);

          if (cmp > 0)
            {
              /* Return +inf. */
              MPFR_CLEAR_NAN(z);
              MPFR_SET_INF(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          else if (cmp < 0)
            {
              /* Return +0. */
              MPFR_CLEAR_FLAGS(z);
              MPFR_SET_ZERO(z);
              MPFR_SET_POS(z);
              MPFR_RET(0);
            }
          else
            {
              /* Return 1. */
              return mpfr_set_ui (z, 1, GMP_RNDN);
            }
        }
    }

  MPFR_ASSERTN(MPFR_IS_FP(y));

  if (MPFR_IS_INF(x))
    {
      int negative;
      /* Determine the sign now, in case y and z are the same object */
      negative = MPFR_SIGN(x) < 0 && is_odd(y);
      MPFR_CLEAR_FLAGS(z);
      if (MPFR_SIGN(y) > 0)
        MPFR_SET_INF(z);
      else
        MPFR_SET_ZERO(z);
      if (negative)
        MPFR_SET_NEG(z);
      else
        MPFR_SET_POS(z);
      MPFR_RET(0);
    }

  MPFR_ASSERTN(MPFR_IS_FP(x));

  if (MPFR_IS_ZERO(x))
    {
      int negative;
      /* Determine the sign now, in case y and z are the same object */
      negative = MPFR_SIGN(x) < 0 && is_odd (y);
      MPFR_CLEAR_FLAGS(z);
      if (MPFR_SIGN(y) < 0)
        MPFR_SET_INF(z);
      else
        MPFR_SET_ZERO(z);
      if (negative)
        MPFR_SET_NEG(z);
      else
        MPFR_SET_POS(z);
      MPFR_RET(0);
    }
  else if (mpfr_cmp_ui (x, 1) == 0) /* 1^y is always 1 */
    {
      mpfr_set_ui (z, 1, GMP_RNDN);
      MPFR_RET(0);
    }

  /* detect overflows: |x^y| >= 2^EMAX when (EXP(x)-1) * y >= EMAX for y > 0,
                                       or   EXP(x) * y     >= EMAX for y < 0 */
  {
    double exy;
    int negative;

    exy = (double) (mpfr_sgn (y) > 0) ? MPFR_EXP(x) - 1 : MPFR_EXP(x);
    exy *= mpfr_get_d (y, GMP_RNDZ);
    if (exy >= (double) __gmpfr_emax)
      {
        negative = MPFR_SIGN(x) < 0 && is_odd (y);
        return mpfr_set_overflow (z, rnd_mode, negative ? -1 : 1);
      }
  }  

  if (mpfr_integer_p (y))
    {
      mpz_t zi;
      long int zii;
      int exptol;
    
      mpz_init (zi);
      exptol = mpfr_get_z_exp (zi, y);
        
      if (exptol > 0)
        mpz_mul_2exp (zi, zi, exptol);
      else
        mpz_tdiv_q_2exp (zi, zi, (unsigned long int) (-exptol));

      zii = mpz_get_si (zi);

      mpz_clear (zi);
      return mpfr_pow_si (z, x, zii, rnd_mode);
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
    int loop = 0, ok;

    /* Declaration of the size variable */
    mp_prec_t Nx = MPFR_PREC(x);   /* Precision of input variable */
    mp_prec_t Ny = MPFR_PREC(y);   /* Precision of input variable */
    mp_prec_t Nz = MPFR_PREC(z);   /* Precision of output variable */

    mp_prec_t Nt;   /* Precision of the intermediary variable */
    long int err;  /* Precision of error */

    /* compute the precision of intermediary variable */
    Nt = MAX(Nx, Ny);
    /* the optimal number of bits : see algorithms.ps */
    Nt = Nt + 5 + __gmpfr_ceil_log2 (Nt);

    /* initialise of intermediary variable */
    mpfr_init (t);
    mpfr_init (ti);
    mpfr_init (te);

    do
      {
        loop ++;

        /* reactualisation of the precision */
        mpfr_set_prec (t, Nt);
        mpfr_set_prec (ti, Nt);
        mpfr_set_prec (te, Nt);

        /* compute exp(y*ln(x)) */
        mpfr_log (ti, x, GMP_RNDU);         /* ln(x) */
        mpfr_mul (te, y, ti, GMP_RNDU);       /* y*ln(x) */
        mpfr_exp (t, te, GMP_RNDN);         /* exp(y*ln(x))*/

        MPFR_ASSERTN(MPFR_IS_FP(te));
        MPFR_ASSERTN(MPFR_NOTZERO(te));
        /* otherwise MPFR_EXP(te) below doesn't exist */

	/* estimate of the error -- see pow function in algorithms.ps */
        err = Nt - (MPFR_GET_EXP (te) + 3);

	/* actualisation of the precision */
        Nt += 10;

        ok = mpfr_can_round (t, err, GMP_RNDN, GMP_RNDZ,
                             Nz + (rnd_mode == GMP_RNDN));

        /* check exact power */
        if (ok == 0 && loop == 1)
          ok = mpfr_pow_is_exact (x, y);
      }
    while (err < 0 || ok == 0);

    inexact = mpfr_set (z, t, rnd_mode);

    mpfr_clear (t);
    mpfr_clear (ti);
    mpfr_clear (te);
  }

  return inexact;
}
