/* mpfr_const_euler -- Euler's constant

Copyright 2001 Free Software Foundation.

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

static void mpfr_const_euler_S _PROTO ((mpfr_ptr, unsigned long));
static void mpfr_const_euler_R _PROTO ((mpfr_ptr, unsigned long));

int
mpfr_const_euler (mpfr_t x, mp_rnd_t rnd)
{
  mp_prec_t prec = MPFR_PREC(x), m, log2m;
  mpfr_t y, z;
  unsigned long n;

  log2m = _mpfr_ceil_log2 ((double) prec);
  m = prec + log2m;

  mpfr_init (y);
  mpfr_init (z);

  do
    {
      m += BITS_PER_MP_LIMB;
      n = 1 + (unsigned long)((double) m * LOG2 / 2.0);
      if (n < 9)
	n = 9;
      MPFR_ASSERTD (n >= 9);
      mpfr_set_prec (y, m + log2m);
      mpfr_set_prec (z, m + log2m);
      mpfr_const_euler_S (y, n);
      mpfr_set_ui (z, n, GMP_RNDN);
      mpfr_log (z, z, GMP_RNDD);
      mpfr_sub (y, y, z, GMP_RNDN); /* S'(n) - log(n) */
      mpfr_set_prec (z, m);
      mpfr_const_euler_R (z, n);
      mpfr_sub (y, y, z, GMP_RNDN);
    }
  while (!mpfr_can_round (y, m - 3, GMP_RNDN, rnd, prec));

  mpfr_set (x, y, rnd);

  mpfr_clear (y);
  mpfr_clear (z);

  return 1; /* always inexact */
}

/* computes S(n) = sum(n^k*(-1)^(k-1)/k!/k, k=1..ceil(4.319136566 * n))
   with an error of at most ulp(x).
   [S(n) >= 2 for n >= 5]
 */
void
mpfr_const_euler_S (mpfr_t x, unsigned long n)
{
  unsigned long N, k, m;
  mpz_t a, s, t;

  N = (long) (ALPHA * (double) n + 1.0); /* ceil(alpha * n) */

  m = MPFR_PREC(x) + (unsigned long) ((double) n / LOG2)
    + _mpfr_ceil_log2 ((double) N) + 1;

  mpz_init_set_ui (a, 1);
  mpz_mul_2exp (a, a, m); /* a=-2^m where m is the precision of x */
  mpz_init_set_ui (s, 0);
  mpz_init (t);

  /* here, a and s are exact */
  for (k = 1; k <= N; k++)
    {
      mpz_mul_ui (a, a, n);
      mpz_div_ui (a, a, k);
      mpz_div_ui (t, a, k);
      if (k % 2)
	mpz_add (s, s, t);
      else
	mpz_sub (s, s, t);
    }

  /* the error on s is at most N (e^n + 1),
     thus that the error on x is at most one ulp */

  mpfr_set_z (x, s, GMP_RNDD);
  mpfr_div_2ui (x, x, m, GMP_RNDD);

  mpz_clear (a);
  mpz_clear (s);
  mpz_clear (t);
}

/* computes R(n) = exp(-n)/n * sum(k!/(-n)^k, k=0..n-2) 
   with error at most 4*ulp(x). Assumes n>=2.
   Since x <= exp(-n)/n <= 1/8, then 4*ulp(x) <= ulp(1).
*/
void
mpfr_const_euler_R (mpfr_t x, unsigned long n)
{
  unsigned long k, m;
  mpz_t a, s;
  mpfr_t y;

  MPFR_ASSERTN (n >= 2); /* ensures sum(k!/(-n)^k, k=0..n-2) >= 2/3 */

  /* as we multiply the sum by exp(-n), we need only PREC(x) - n/LOG2 bits */
  m = MPFR_PREC(x) - (unsigned long) ((double) n / LOG2);

  mpz_init_set_ui (a, 1);
  mpz_mul_2exp (a, a, m);
  mpz_init_set (s, a);

  for (k = 1; k <= n; k++)
    {
      mpz_mul_ui (a, a, k); 
      mpz_div_ui (a, a, n);
      /* the error e(k) on a is e(k) <= 1 + k/n*e(k-1) with e(0)=0,
	 i.e. e(k) <= k */
      if (k % 2)
	mpz_sub (s, s, a);
      else
	mpz_add (s, s, a);
    }
  /* the error on s is at most 1+2+...+n = n*(n+1)/2 */
  mpz_div_ui (s, s, n); /* err <= 1 + (n+1)/2 */
  if (MPFR_PREC(x) < mpz_sizeinbase(s, 2))
    {
      fprintf (stderr, "prec(x) is too small in mpfr_const_euler_R\n");
      exit (1);
    }
  mpfr_set_z (x, s, GMP_RNDD); /* exact */
  mpfr_div_2ui (x, x, m, GMP_RNDD);
  /* now x = 1/n * sum(k!/(-n)^k, k=0..n-2) <= 1/n */
  /* err(x) <= (n+1)/2^m <= (n+1)*exp(n)/2^PREC(x) */

  mpfr_init2 (y, m);
  mpfr_set_si (y, -n, GMP_RNDD); /* assumed exact */
  mpfr_exp (y, y, GMP_RNDD); /* err <= ulp(y) <= exp(-n)*2^(1-m) */
  mpfr_mul (x, x, y, GMP_RNDD);
  /* err <= ulp(x) + (n + 1 + 2/n) / 2^prec(x)
     <= ulp(x) + (n + 1 + 2/n) ulp(x)/x since x*2^(-prec(x)) < ulp(x)
     <= ulp(x) + (n + 1 + 2/n) 3/(2n) ulp(x) since x >= 2/3*n for n >= 2
     <= 4 * ulp(x) for n >= 2 */
  mpfr_clear (y);

  mpz_clear (a);
  mpz_clear (s);
}
