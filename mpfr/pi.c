/* mpfr_pi -- compute Pi

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

/*
Set x to the value of Pi to precision PREC(x) rounded to direction rnd_mode.
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

mpfr_t __mpfr_pi; /* stored value of Pi */
int __mpfr_pi_prec=0; /* precision of stored value */
char __mpfr_pi_rnd; /* rounding mode of stored value */

void 
#if __STDC__
mpfr_pi(mpfr_ptr x, unsigned char rnd_mode) 
#else
mpfr_pi(x, rnd_mode) 
     mpfr_ptr x; 
     unsigned char rnd_mode;
#endif
{
  int N, oldN, n, prec; mpz_t pi, num, den, d3, d2, tmp; mpfr_t y;

  prec=PREC(x);

  /* has stored value enough precision ? */
  if ((prec==__mpfr_pi_prec && rnd_mode==__mpfr_pi_rnd) ||
      (prec<=__mpfr_pi_prec &&
      mpfr_can_round(__mpfr_pi, __mpfr_pi_prec, __mpfr_pi_rnd, rnd_mode, prec)))
    {
      mpfr_set(x, __mpfr_pi, rnd_mode); return; 
    }

  /* need to recompute */
  N=1; 
  do {
    oldN = N;
    N = (prec+3)/4 + (int)ceil(log((double)N+1.0)/log(2.0));
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
    mpz_add_ui(num, num, 240*n+31); /* no overflow up to PREC=71M */
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
  EXP(x) -= 4*N;
  mpz_clear(pi); mpz_clear(num); mpz_clear(den); mpz_clear(d3); mpz_clear(d2);
  mpz_clear(tmp); mpfr_clear(y);

  /* store computed value */
  if (__mpfr_pi_prec==0) mpfr_init2(__mpfr_pi, prec);
  else mpfr_set_prec(__mpfr_pi, prec);
  mpfr_set(__mpfr_pi, x, rnd_mode);
  __mpfr_pi_prec=prec;
  __mpfr_pi_rnd=rnd_mode;
}
