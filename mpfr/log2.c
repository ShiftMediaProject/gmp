/* mpfr_log2 -- compute natural logarithm of 2

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <math.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"

mpfr_t __mpfr_log2; /* stored value of log(2) with rnd_mode=GMP_RNDZ */
int __mpfr_log2_prec=0; /* precision of stored value */

/* set x to log(2) rounded to precision PREC(x) with direction rnd_mode 

   use formula log(2) = sum(1/k/2^k, k=1..infinity)

   whence 2^N*log(2) = S(N) + R(N)

   where S(N) = sum(2^(N-k)/k, k=1..N-1)
   and   R(N) = sum(1/k/2^(k-N), k=N..infinity) < 2/N

   Let S'(N) = sum(floor(2^(N-k)/k), k=1..N-1)

   Then 2^N*log(2)-S'(N) <= N-1+2/N <= N for N>=2.
*/
void 
#if __STDC__
mpfr_log2(mpfr_ptr x, unsigned char rnd_mode)
#else
mpfr_log2(x, rnd_mode) mpfr_ptr x; unsigned char rnd_mode;
#endif
{
  int N, oldN, k, precx; mpz_t s, t, u;

  precx = PREC(x);

  /* has stored value enough precision ? */
  if (precx <= __mpfr_log2_prec) {
    if (rnd_mode==GMP_RNDZ || rnd_mode==GMP_RNDD ||
	mpfr_can_round(__mpfr_log2, __mpfr_log2_prec, GMP_RNDZ, rnd_mode, precx))
      {
	mpfr_set(x, __mpfr_log2, rnd_mode); return; 
      }
  }

  /* need to recompute */
  N=2;
  do {
    oldN = N;
    N = precx + (int)ceil(log((double)N)/log(2.0));
  } while (N != oldN);
  mpz_init_set_ui(s,0);
  mpz_init(u);
  mpz_init_set_ui(t,1); 
#if 0
  /* use log(2) = sum(1/k/2^k, k=1..infinity) */
  mpz_mul_2exp(t, t, N);
  for (k=1;k<N;k++) {
    mpz_div_2exp(t, t, 1);
    mpz_fdiv_q_ui(u, t, k);
    mpz_add(s, s, u);
  }
#else
  /* use log(2) = sum((6*k-1)/(2*k^2-k)/2^(2*k+1), k=1..infinity) */
  mpz_mul_2exp(t, t, N-1);
  for (k=1;k<N/2;k++) {
    mpz_div_2exp(t, t, 2);
    mpz_mul_ui(u, t, 6*k-1);
    mpz_fdiv_q_ui(u, u, k*(2*k-1));
    mpz_add(s, s, u);
  }
#endif
  mpfr_set_z(x, s, rnd_mode);
  EXP(x) -= N;

  /* stored computed value */
  if (__mpfr_log2_prec==0) mpfr_init2(__mpfr_log2, precx);
  else mpfr_set_prec(__mpfr_log2, precx);
  mpfr_set(__mpfr_log2, x, GMP_RNDZ);
  __mpfr_log2_prec=precx;

  mpz_clear(s); mpz_clear(t); mpz_clear(u);
}
