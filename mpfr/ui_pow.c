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
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"


static int mpfr_ui_pow_is_exact _PROTO((unsigned long int, mpfr_srcptr));

/* return non zero iff x^y is exact.
   Assumes y is a non-zero ordinary number (neither NaN nor Inf).
*/
static int
mpfr_ui_pow_is_exact (unsigned long int x, mpfr_srcptr y)
{
  mp_exp_t d;
  unsigned long i, c;
  mp_limb_t *yp;
  mp_size_t ysize;
  
  if (mpfr_sgn (y) < 0) /* exact iff x = 2^k */
    {
      count_leading_zeros(c, x);
      c = BITS_PER_MP_LIMB - c; /* number of bits of x */
      return x == 1UL << (c - 1);
    }

  /* compute d such that y = c*2^d with c odd integer */
  ysize = 1 + (MPFR_PREC(y) - 1) / BITS_PER_MP_LIMB;
  d = MPFR_EXP(y) - ysize * BITS_PER_MP_LIMB;
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

      mpz_init_set_ui (a, x);
      b = 0; /* x = a * 2^b */
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

 /* The computation of y=pow(n,z) is done by

    y=exp(z*log(n))=n^z
 */

int
mpfr_ui_pow (mpfr_ptr y, unsigned long int n, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
    int inexact = 1;

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
      return mpfr_set_ui (y, 1, rnd_mode);

    inexact = mpfr_ui_pow_is_exact (n, x) == 0;

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
      Nt = MAX(Nx,Ny);
      /* the optimal number of bits : see algorithms.ps */
      Nt = Nt + 5 + __gmpfr_ceil_log2 (Nt);

      /* initialise of intermediary	variable */
      mpfr_init2 (t, MPFR_PREC_MIN);
      mpfr_init2 (ti, sizeof(unsigned long int) * 8); /* 8 = CHAR_BIT */
      mpfr_init2 (te, MPFR_PREC_MIN);

      do {

	/* reactualisation of the precision */
	mpfr_set_prec (t, Nt);
   	mpfr_set_prec (te, Nt);             

	/* compute   exp(x*ln(n))*/
        mpfr_set_ui (ti, n, GMP_RNDN);      /* ti <- n*/
        mpfr_log (t, ti, GMP_RNDU);         /* ln(n) */
        mpfr_mul (te, x, t, GMP_RNDU);       /* x*ln(n) */
        mpfr_exp (t, te, GMP_RNDN);         /* exp(x*ln(n))*/

	/* estimation of the error -- see pow function in algorithms.ps*/
	err = Nt - (MPFR_EXP(te) + 3);

	/* actualisation of the precision */
	Nt += 10;

      } while (inexact && (err < 0 || !mpfr_can_round (t, err, GMP_RNDN, rnd_mode, Ny)));
 
      inexact = mpfr_set (y, t, rnd_mode);

      mpfr_clear (t);
      mpfr_clear (ti);
      mpfr_clear (te);
    }
    return inexact;

}
