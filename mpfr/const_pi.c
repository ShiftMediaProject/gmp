/* mpfr_const_pi -- compute Pi

Copyright 1999, 2000, 2001 Free Software Foundation.

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
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

static int mpfr_aux_pi _PROTO ((mpfr_ptr, mpz_srcptr, int, int));
static int mpfr_pi_machin3 _PROTO ((mpfr_ptr, mp_rnd_t));

#define A
#define A1 1
#define A2 2
#undef B
#define C
#define C1 3
#define C2 2
#define GENERIC mpfr_aux_pi
#define R_IS_RATIONAL
#define NO_FACTORIAL
#include "generic.c" 


static int
mpfr_pi_machin3 (mpfr_ptr mylog, mp_rnd_t rnd_mode) 
{
  int prec, logn, prec_x;
  int prec_i_want=MPFR_PREC(mylog);
  int good = 0;
  mpfr_t tmp1, tmp2, result,tmp3,tmp4,tmp5,tmp6; 
  mpz_t cst;

  MPFR_CLEAR_FLAGS(mylog); 
  logn =  _mpfr_ceil_log2 ((double) MPFR_PREC(mylog));
  prec_x = prec_i_want + logn + 5;
  mpz_init(cst);  
  while (!good){
  prec = _mpfr_ceil_log2 ((double) prec_x);

  mpfr_init2(tmp1, prec_x);
  mpfr_init2(tmp2, prec_x);
  mpfr_init2(tmp3, prec_x);
  mpfr_init2(tmp4, prec_x);
  mpfr_init2(tmp5, prec_x);
  mpfr_init2(tmp6, prec_x);
  mpfr_init2(result, prec_x);
  mpz_set_si(cst, -1);

  mpfr_aux_pi(tmp1, cst, 268*268, prec - 4);
  mpfr_div_ui(tmp1, tmp1, 268, GMP_RNDD);
  mpfr_mul_ui(tmp1, tmp1, 61, GMP_RNDD);

  mpfr_aux_pi(tmp2, cst, 343*343, prec - 4);
  mpfr_div_ui(tmp2, tmp2, 343, GMP_RNDD);
  mpfr_mul_ui(tmp2, tmp2, 122, GMP_RNDD);

  mpfr_aux_pi(tmp3, cst, 557*557, prec - 4);
  mpfr_div_ui(tmp3, tmp3, 557, GMP_RNDD);
  mpfr_mul_ui(tmp3, tmp3, 115, GMP_RNDD);

  mpfr_aux_pi(tmp4, cst, 1068*1068, prec - 4);
  mpfr_div_ui(tmp4, tmp4, 1068, GMP_RNDD);
  mpfr_mul_ui(tmp4, tmp4, 32, GMP_RNDD);

  mpfr_aux_pi(tmp5, cst, 3458*3458, prec - 4);
  mpfr_div_ui(tmp5, tmp5, 3458, GMP_RNDD);
  mpfr_mul_ui(tmp5, tmp5, 83, GMP_RNDD);

  mpfr_aux_pi(tmp6, cst, 27493*27493, prec - 4);
  mpfr_div_ui(tmp6, tmp6, 27493, GMP_RNDD);
  mpfr_mul_ui(tmp6, tmp6, 44, GMP_RNDD);

  mpfr_add(result, tmp1, tmp2, GMP_RNDD);
  mpfr_add(result, result, tmp3, GMP_RNDD);
  mpfr_sub(result, result, tmp4, GMP_RNDD);
  mpfr_add(result, result, tmp5, GMP_RNDD);
  mpfr_add(result, result, tmp6, GMP_RNDD);
  mpfr_mul_2ui(result, result, 2, GMP_RNDD);
  mpfr_clear(tmp1);
  mpfr_clear(tmp2);
  mpfr_clear(tmp3);
  mpfr_clear(tmp4);
  mpfr_clear(tmp5);
  mpfr_clear(tmp6);
  if (mpfr_can_round(result, prec_x - 5, GMP_RNDD, rnd_mode, prec_i_want)){
    mpfr_set(mylog, result, rnd_mode);
    mpfr_clear(result);
    good = 1;
  } else
    {
      mpfr_clear(result);
      prec_x += logn;
      }
  }
  mpz_clear(cst);
  return 0;    
}

/*
Set x to the value of Pi to precision MPFR_PREC(x) rounded to direction rnd_mode.
Use the formula giving the binary representation of Pi found by Simon Plouffe
and the Borwein's brothers:

                   infinity    4         2         1         1
                    -----   ------- - ------- - ------- - -------
                     \      8 n + 1   8 n + 4   8 n + 5   8 n + 6
              Pi =    )     -------------------------------------
                     /                         n
                    -----                    16
                    n = 0

i.e. Pi*16^N = S(N) + R(N) where
S(N) = sum(16^(N-n)*(4/(8*n+1)-2/(8*n+4)-1/(8*n+5)-1/(8*n+6)), n=0..N-1)
R(N) = sum((4/(8*n+1)-2/(8*n+4)-1/(8*n+5)-1/(8*n+6))/16^(n-N), n=N..infinity)

Let f(n) = 4/(8*n+1)-2/(8*n+4)-1/(8*n+5)-1/(8*n+6), we can show easily that
f(n) < 15/(64*n^2), so R(N) < sum(15/(64*n^2)/16^(n-N), n=N..infinity)
                            < 15/64/N^2*sum(1/16^(n-N), n=N..infinity)
			    = 1/4/N^2

Now let S'(N) = sum(floor(16^(N-n)*(120*n^2+151*n+47),
  (512*n^4+1024*n^3+712*n^2+194*n+15)), n=0..N-1)

S(N)-S'(N) <= sum(1, n=0..N-1) = N

so Pi*16^N-S'(N) <= N+1 (as 1/4/N^2 < 1)
*/

mpfr_t __mpfr_const_pi; /* stored value of Pi */
int __mpfr_const_pi_prec=0; /* precision of stored value */
mp_rnd_t __mpfr_const_pi_rnd; /* rounding mode of stored value */

void 
mpfr_const_pi (mpfr_ptr x, mp_rnd_t rnd_mode) 
{
  int N, oldN, n, prec; mpz_t pi, num, den, d3, d2, tmp; mpfr_t y;

  prec=MPFR_PREC(x);

  /* has stored value enough precision ? */
  if ((prec==__mpfr_const_pi_prec && rnd_mode==__mpfr_const_pi_rnd) ||
      (prec<=__mpfr_const_pi_prec &&
      mpfr_can_round(__mpfr_const_pi, __mpfr_const_pi_prec, 
		     __mpfr_const_pi_rnd, rnd_mode, prec)))
    {
      mpfr_set(x, __mpfr_const_pi, rnd_mode); return; 
    }

  if (prec < 20000){
    /* need to recompute */
    N=1; 
    do {
      oldN = N;
      N = (prec+3)/4 + _mpfr_ceil_log2((double) N + 1.0);
    } while (N != oldN);
    mpz_init(pi); mpz_init(num); mpz_init(den); mpz_init(d3); mpz_init(d2);
    mpz_init(tmp);
    mpz_set_ui(pi, 0);
    mpz_set_ui(num, 16); /* num(-1) */
    mpz_set_ui(den, 21); /* den(-1) */
    mpz_set_si(d3, -2454);
    mpz_set_ui(d2, 14736);
  /* invariants: num=120*n^2+151*n+47, den=512*n^4+1024*n^3+712*n^2+194*n+15
     d3 = 2048*n^3+400*n-6, d2 = 6144*n^2-6144*n+2448
   */
    for (n=0; n<N; n++) {
      /* num(n)-num(n-1) = 240*n+31 */
      mpz_add_ui(num, num, 240*n+31); /* no overflow up to MPFR_PREC=71M */
      /* d2(n) - d2(n-1) = 12288*(n-1) */
      if (n>0) mpz_add_ui(d2, d2, 12288*(n-1));
      else mpz_sub_ui(d2, d2, 12288);
      /* d3(n) - d3(n-1) = d2 */
      mpz_add(d3, d3, d2);
      /* den(n)-den(n-1) = 2048*n^3 + 400n - 6 = d3 */
      mpz_add(den, den, d3);
      mpz_mul_2exp(tmp, num, 4*(N-n));
      mpz_fdiv_q(tmp, tmp, den);
      mpz_add(pi, pi, tmp);
    }
    mpfr_set_z(x, pi, rnd_mode);
    mpfr_init2(y, mpfr_get_prec(x));
    mpz_add_ui(pi, pi, N+1);
    mpfr_set_z(y, pi, rnd_mode);
    if (mpfr_cmp(x, y) != 0) {
      fprintf(stderr, "does not converge\n"); exit(1);
    }
    MPFR_EXP(x) -= 4*N;
    mpz_clear(pi); mpz_clear(num); mpz_clear(den); mpz_clear(d3); mpz_clear(d2);
    mpz_clear(tmp); mpfr_clear(y);
  } else
     mpfr_pi_machin3(x, rnd_mode);
  /* store computed value */
  if (__mpfr_const_pi_prec==0) mpfr_init2(__mpfr_const_pi, prec);
  else mpfr_set_prec(__mpfr_const_pi, prec);
  mpfr_set(__mpfr_const_pi, x, rnd_mode);
  __mpfr_const_pi_prec=prec;
  __mpfr_const_pi_rnd=rnd_mode;
}
