/* mpfr_const_log2 -- compute natural logarithm of 2

Copyright 1999, 2001 Free Software Foundation, Inc.

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
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

mpfr_t __mpfr_const_log2; /* stored value of log(2) */
mp_prec_t __mpfr_const_log2_prec=0; /* precision of stored value */
mp_rnd_t __mpfr_const_log2_rnd; /* rounding mode of stored value */

static int mpfr_aux_log2 _PROTO ((mpfr_ptr, mpz_srcptr, int, int));
static int mpfr_const_aux_log2 _PROTO ((mpfr_ptr, mp_rnd_t));

#define A
#define A1 1
#define A2 1
#undef B
#define C
#define C1 2
#define C2 1
#define NO_FACTORIAL
#undef R_IS_RATIONAL
#define GENERIC mpfr_aux_log2
#include "generic.c" 
#undef A
#undef A1
#undef A2
#undef NO_FACTORIAL
#undef GENERIC
#undef C
#undef C1
#undef C2

static int
mpfr_const_aux_log2 (mpfr_ptr mylog, mp_rnd_t rnd_mode)
{
  mp_prec_t prec;
  mpfr_t tmp1, tmp2, result,tmp3; 
  mpz_t cst;
  int good = 0;
  int logn;
  mp_prec_t prec_i_want = MPFR_PREC(mylog);
  mp_prec_t prec_x;

  mpz_init(cst);
  logn =  _mpfr_ceil_log2 ((double) MPFR_PREC(mylog));
  prec_x = prec_i_want + logn;
  while (!good){
    prec = _mpfr_ceil_log2 ((double) prec_x);
    mpfr_init2(tmp1, prec_x);
    mpfr_init2(result, prec_x);
    mpfr_init2(tmp2, prec_x);
    mpfr_init2(tmp3, prec_x);
    mpz_set_ui(cst, 1);
    mpfr_aux_log2(tmp1, cst, 4, prec-2);
    mpfr_div_2ui(tmp1, tmp1, 4, GMP_RNDD);
    mpfr_mul_ui(tmp1, tmp1, 15, GMP_RNDD);

    mpz_set_ui(cst, 3);
    mpfr_aux_log2(tmp2, cst, 7, prec-2);
    mpfr_div_2ui(tmp2, tmp2, 7, GMP_RNDD);
    mpfr_mul_ui(tmp2, tmp2, 5*3, GMP_RNDD);
    mpfr_sub(result, tmp1, tmp2, GMP_RNDD);

    mpz_set_ui(cst, 13);
    mpfr_aux_log2(tmp3, cst, 8, prec-2);
    mpfr_div_2ui(tmp3, tmp3, 8, GMP_RNDD);
    mpfr_mul_ui(tmp3, tmp3, 3*13, GMP_RNDD);
    mpfr_sub(result, result, tmp3, GMP_RNDD);

    mpfr_clear(tmp1);
    mpfr_clear(tmp2);
    mpfr_clear(tmp3);
    if (mpfr_can_round(result, prec_x, GMP_RNDD, rnd_mode, prec_i_want)){
      mpfr_set(mylog, result, rnd_mode);
      good = 1;
    } else
      {
	prec_x += logn;
      }
    mpfr_clear(result);
  }
  mpz_clear(cst);
  return 0;
}

/* Cross-over point from nai"ve Taylor series to binary splitting,
   obtained experimentally on a Pentium II. Optimal value for
   target machine should be determined by tuneup. */
#define LOG2_THRESHOLD 25000

/* set x to log(2) rounded to precision MPFR_PREC(x) with direction rnd_mode 

   use formula log(2) = sum(1/k/2^k, k=1..infinity)

   whence 2^N*log(2) = S(N) + R(N)

   where S(N) = sum(2^(N-k)/k, k=1..N-1)
   and   R(N) = sum(1/k/2^(k-N), k=N..infinity) < 2/N

   Let S'(N) = sum(floor(2^(N-k)/k), k=1..N-1)

   Then 2^N*log(2)-S'(N) <= N-1+2/N <= N for N>=2.
*/
void 
mpfr_const_log2 (mpfr_ptr x, mp_rnd_t rnd_mode)
{
  mp_prec_t N, k, precx;
  mpz_t s, t, u;

  precx = MPFR_PREC(x);
  MPFR_CLEAR_FLAGS(x); 

  /* has stored value enough precision ? */
  if (precx <= __mpfr_const_log2_prec)
    {
      if ((rnd_mode == __mpfr_const_log2_rnd) ||
          mpfr_can_round (__mpfr_const_log2, __mpfr_const_log2_prec - 1,
                          __mpfr_const_log2_rnd, rnd_mode, precx))
        {
          mpfr_set (x, __mpfr_const_log2, rnd_mode);
          return;
        }
    }
  
  /* need to recompute */
  if (precx < LOG2_THRESHOLD) /* use nai"ve Taylor series evaluation */
    {
      /* the following was checked by exhaustive search to give a correct
         result for all 4 rounding modes up to precx = 13500 */
      N = precx + 2 * _mpfr_ceil_log2 ((double) precx) + 1;

      mpz_init (s); /* set to zero */
      mpz_init (u);
      mpz_init_set_ui (t, 1);

      /* use log(2) = sum((6*k-1)/(2*k^2-k)/2^(2*k+1), k=1..infinity) */
      mpz_mul_2exp (t, t, N-1);
      for (k=1; k<=N/2; k++)
        {
          mpz_div_2exp (t, t, 2);
          mpz_mul_ui (u, t, 6*k-1);
          mpz_fdiv_q_ui (u, u, k*(2*k-1));
          mpz_add (s, s, u);
        }

      mpfr_set_z (x, s, rnd_mode);
      MPFR_EXP(x) -= N;
      mpz_clear (s);
      mpz_clear (t);
      mpz_clear (u);
    }
  else /* use binary splitting method */
    mpfr_const_aux_log2(x, rnd_mode);

  /* store computed value */
  if (__mpfr_const_log2_prec == 0)
    mpfr_init2 (__mpfr_const_log2, precx);
  else
    mpfr_set_prec (__mpfr_const_log2, precx);

  mpfr_set (__mpfr_const_log2, x, rnd_mode);
  __mpfr_const_log2_prec = precx;
  __mpfr_const_log2_rnd = rnd_mode;
}
