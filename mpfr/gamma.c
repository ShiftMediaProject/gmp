/* mpfr_gamma -- gamma function

Copyright 2001, 2002, 2003 Free Software Foundation.

This file is part of the MPFR Library, and was contributed by Mathieu Dutour.

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

#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* We use the reflection formula 
  Gamma(1+t) Gamma(1-t) = - Pi t / sin(Pi (1 + t))
  in order to treat the case x <= 1,
  i.e. if x = 1-t, then Gamma(x) = -Pi*(1-x)/sin(Pi*(2-x))/GAMMA(2-x)
*/

#define CST   0.38  /* CST=ln(2)/(ln(2*pi)) */
#define zCST  0.26  /* zCST=1/(2*ln(2*pi)) */
#define ecCST  1.84  /* {1+sup_{x\in [0,1]} x*ln((1-x)/x)}/ln(2)  */

int
mpfr_gamma (mpfr_ptr gamma, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  mpfr_t xp;
  mpfr_t product;
  mpfr_t the_pi;
  mpfr_t GammaTrial;
  mpfr_t tmp, tmp2;

  mp_prec_t Prec;
  mp_prec_t prec_gamma;
  mp_prec_t prec_nec;
  int good = 0;
  double C;
  mp_prec_t A, N, estimated_cancel;
  mp_prec_t realprec;
  int compared;
  int k;
  int sign;

  /* Trivial cases */
  if (MPFR_IS_NAN(x))
    {
      MPFR_SET_NAN(gamma);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_INF(x))
    {
      if (MPFR_SIGN(x) < 0)
        {
          MPFR_SET_NAN(gamma);
          MPFR_RET_NAN;
        }
      else
        {
          MPFR_CLEAR_NAN(gamma);
          MPFR_SET_INF(gamma);
          MPFR_SET_POS(gamma);
          return 0;  /* exact */
        }
    }

  if (MPFR_IS_ZERO(x))
    {
      MPFR_CLEAR_NAN(gamma);
      MPFR_SET_INF(gamma);
      MPFR_SET_SAME_SIGN(gamma, x);
      return 0;  /* exact */
    }

  /* Set x_p=x if x> 1 else set x_p=2-x */
  prec_gamma = MPFR_PREC (gamma);
  compared = mpfr_cmp_ui (x, 1);
  if (compared == 0)
    return mpfr_set_ui (gamma, 1, rnd_mode);

  realprec = prec_gamma + 10;

  mpfr_init2 (xp, 2);

  while (!good)
    {
      /* Precision stuff */
      prec_nec = compared < 0 ?
        2 + realprec  /* We will use the reflexion formula! */
        : realprec;
      C = (double)(((double) prec_nec)*CST-0.5);
      A = (mp_prec_t) C;
      N = A - 1;
#ifdef DEBUG
      printf("C=%u", (int)C);
      printf(" A=%u", (int)A);
      printf(" N=%u", (int)N);
      printf("\n");
#endif

      /* estimated_cancel is the amount of bit that will be flushed */
      estimated_cancel= (mp_prec_t) (ecCST * (double) A + 1.0);
      Prec = prec_nec + estimated_cancel + 20;

      mpfr_set_prec (xp, Prec);
      if (compared < 0)
        {
          mpfr_ui_sub (xp, 1, x, GMP_RNDN);
        }
      else
        {
          mpfr_sub_ui (xp, x, 1, GMP_RNDN);
        }

      /* Initialisation    */
      mpfr_init2(tmp, Prec);
      mpfr_init2(tmp2, Prec);
      mpfr_init2(the_pi, Prec);
      mpfr_init2(product, Prec);
      mpfr_init2(GammaTrial, Prec);

      mpfr_set_ui(GammaTrial, 0, GMP_RNDN);
      sign = 1;
      for (k = 1; k <= N; k++)
        {
          mpfr_set_ui(tmp, A-k, GMP_RNDN);
          mpfr_exp(product, tmp, GMP_RNDN);
          mpfr_ui_pow_ui(tmp, A-k, k-1, GMP_RNDN);
          mpfr_mul(product, product, tmp, GMP_RNDN);
          mpfr_sqrt_ui(tmp, A-k, GMP_RNDN);
          mpfr_mul(product, product, tmp, GMP_RNDN);
          mpfr_fac_ui(tmp, k-1, GMP_RNDN);
          mpfr_div(product, product, tmp, GMP_RNDN);
          mpfr_add_ui(tmp, xp, k, GMP_RNDN);
          mpfr_div(product, product, tmp, GMP_RNDN);
          sign = -sign;
          if (sign == 1)
            {
              mpfr_neg(product, product, GMP_RNDN);
#ifdef DEBUG
              /*    printf(" k=%u", k);
                    printf("\n");*/
#endif
            }
          mpfr_add(GammaTrial, GammaTrial, product, GMP_RNDN);
        }
#ifdef DEBUG
      printf("GammaTrial =");
      mpfr_out_str (stdout, 10, 0, GammaTrial, GMP_RNDD);
      printf ("\n");
#endif
      mpfr_const_pi(the_pi, GMP_RNDN);
      mpfr_const_pi(tmp, GMP_RNDN);
      mpfr_mul_2ui(tmp, tmp, 1, GMP_RNDN);
      mpfr_sqrt(tmp, tmp, GMP_RNDN);
      mpfr_add(GammaTrial, GammaTrial, tmp, GMP_RNDN);
      mpfr_add_ui(tmp2, xp, A, GMP_RNDN);
      mpfr_set_ui(tmp, 1, GMP_RNDN);
      mpfr_div_2ui(tmp, tmp, 1, GMP_RNDN);
      mpfr_add(tmp, tmp, xp, GMP_RNDN);
      mpfr_pow(tmp, tmp2, tmp, GMP_RNDN);
      mpfr_mul(GammaTrial, GammaTrial, tmp, GMP_RNDN);
      mpfr_neg(tmp, tmp2, GMP_RNDN);
      mpfr_exp(tmp, tmp, GMP_RNDN);
      mpfr_mul(GammaTrial, GammaTrial, tmp, GMP_RNDN);
      if (compared < 0)
        {
          mpfr_sub_ui (tmp, x, 1, GMP_RNDN);
          mpfr_mul (tmp, the_pi, tmp, GMP_RNDN);
          mpfr_div (GammaTrial, tmp, GammaTrial, GMP_RNDN);
          mpfr_sin (tmp, tmp, GMP_RNDN);
          mpfr_div (GammaTrial, GammaTrial, tmp, GMP_RNDN);
        }
#ifdef DEBUG
      printf("GammaTrial =");
      mpfr_out_str (stdout, 10, 0, GammaTrial, GMP_RNDD);
      printf ("\n");
#endif
      if (mpfr_can_round (GammaTrial, realprec, GMP_RNDD, rnd_mode,
                          MPFR_PREC(gamma)))
        {
          mpfr_set (gamma, GammaTrial, rnd_mode);
          good = 1;
        }
      else
        {
          realprec += __gmpfr_ceil_log2 ((double) realprec);
#ifdef DEBUG
          printf("RETRY\n");
#endif
        }
      mpfr_clear(tmp);
      mpfr_clear(tmp2);
      mpfr_clear(the_pi);
      mpfr_clear(product);
      mpfr_clear(GammaTrial);
    }

  mpfr_clear (xp);

  return 1; /* inexact result */
}
