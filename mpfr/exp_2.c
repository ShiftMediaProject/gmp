/* mpfr_exp_2 -- exponential of a floating-point number 
                using Brent's algorithms in O(n^(1/2)*M(n)) and O(n^(1/3)*M(n))

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include "longlong.h" /* for count_leading_zeros */
#include "mpfr.h"
#include "mpfr-impl.h"

static int mpfr_exp2_aux      _PROTO ((mpz_t, mpfr_srcptr, int, int*));
static int mpfr_exp2_aux2     _PROTO ((mpz_t, mpfr_srcptr, int, int*));
static mp_exp_t mpz_normalize _PROTO ((mpz_t, mpz_t, int));
static int mpz_normalize2     _PROTO ((mpz_t, mpz_t, int, int));

/* returns floor(sqrt(n)) */
unsigned long
__gmpfr_isqrt (unsigned long n)
{
  unsigned long s;

  s = 1;
  do {
    s = (s + n / s) / 2;
  } while (!(s*s <= n && n <= s*(s+2)));
  return s;
}

/* returns floor(n^(1/3)) */
unsigned long
__gmpfr_cuberoot (unsigned long n)
{
  double s, is;

  s = 1.0;
  do {
    s = (2*s*s*s + (double) n) / (3*s*s);
    is = (double) ((int) s);
  } while (!(is*is*is <= (double) n && (double) n < (is+1)*(is+1)*(is+1)));
  return (unsigned long) is;
}

#define SWITCH 100 /* number of bits to switch from O(n^(1/2)*M(n)) method
		      to O(n^(1/3)*M(n)) method */

#define MY_INIT_MPZ(x, s) { \
   (x)->_mp_alloc = (s); \
   PTR(x) = (mp_ptr) TMP_ALLOC((s)*BYTES_PER_MP_LIMB); \
   (x)->_mp_size = 0; }

/* #define DEBUG */

/* if k = the number of bits of z > q, divides z by 2^(k-q) and returns k-q.
   Otherwise do nothing and return 0.
 */
static mp_exp_t
mpz_normalize (mpz_t rop, mpz_t z, int q)
{
  int k;

  k = mpz_sizeinbase(z, 2);
  if (k > q) {
    mpz_div_2exp(rop, z, k-q);
    return k-q;
  }
  else {
    if (rop != z) mpz_set(rop, z);
    return 0;
  }
}

/* if expz > target, shift z by (expz-target) bits to the left.
   if expz < target, shift z by (target-expz) bits to the right.
   Returns target.
*/
static int
mpz_normalize2 (mpz_t rop, mpz_t z, int expz, int target)
{
  if (target > expz) mpz_div_2exp(rop, z, target-expz);
  else mpz_mul_2exp(rop, z, expz-target);
  return target;
}

/* use Brent's formula exp(x) = (1+r+r^2/2!+r^3/3!+...)^(2^K)*2^n
   where x = n*log(2)+(2^K)*r
   together with Brent-Kung O(t^(1/2)) algorithm for the evaluation of
   power series. The resulting complexity is O(n^(1/3)*M(n)).
*/
int 
mpfr_exp_2 (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
{
  int n, K, precy, q, k, l, err, exps, inexact, error_r;
  mpfr_t r, s, t; mpz_t ss;
  TMP_DECL(marker);

  precy = MPFR_PREC(y);

  n = (int) (mpfr_get_d1 (x) / LOG2);

  /* error bounds the cancelled bits in x - n*log(2) */
  if (n == 0)
    error_r = 0;
  else
    count_leading_zeros (error_r, (mp_limb_t) (n < 0) ? -n : n);
  error_r = BITS_PER_MP_LIMB - error_r + 2;

  /* for the O(n^(1/2)*M(n)) method, the Taylor series computation of
     n/K terms costs about n/(2K) multiplications when computed in fixed
     point */
  K = (precy<SWITCH) ? __gmpfr_isqrt((precy + 1) / 2) : __gmpfr_cuberoot (4*precy);
  l = (precy - 1) / K + 1;
  err = K + (int) __gmpfr_ceil_log2 (2.0 * (double) l + 18.0);
  /* add K extra bits, i.e. failure probability <= 1/2^K = O(1/precy) */
  q = precy + err + K + 3;
  mpfr_init2 (r, q + error_r);
  mpfr_init2 (s, q + error_r);
  mpfr_init2 (t, q);
  /* the algorithm consists in computing an upper bound of exp(x) using
     a precision of q bits, and see if we can round to MPFR_PREC(y) taking
     into account the maximal error. Otherwise we increase q. */
  do {
#ifdef DEBUG
  printf("n=%d K=%d l=%d q=%d\n",n,K,l,q);
#endif

  /* if n<0, we have to get an upper bound of log(2)
     in order to get an upper bound of r = x-n*log(2) */
  mpfr_const_log2 (s, (n >= 0) ? GMP_RNDZ : GMP_RNDU);
  /* s is within 1 ulp of log(2) */
#ifdef DEBUG
  printf ("n=%d log(2)=", n);
  mpfr_print_binary (s);
  putchar ('\n');
#endif
  mpfr_mul_ui (r, s, (n < 0) ? -n : n, (n >= 0) ? GMP_RNDZ : GMP_RNDU);
  /* r is within 3 ulps of n*log(2) */
  if (n < 0)
    mpfr_neg (r, r, GMP_RNDD); /* exact */
  /* r = floor(n*log(2)), within 3 ulps */

#ifdef DEBUG
  printf ("x="); mpfr_print_binary (x); putchar ('\n');
  printf ("r="); mpfr_print_binary (r); putchar ('\n');
#endif
  mpfr_sub (r, x, r, GMP_RNDU);
  /* possible cancellation here: the error on r is at most 
     3*2^(EXP(old_r)-EXP(new_r)) */
  if (MPFR_SIGN(r) < 0)
    { /* initial approximation n was too large */
      n--;
      mpfr_mul_ui (r, s, (n < 0) ? -n : n, GMP_RNDZ);
      if (n < 0)
        mpfr_neg (r, r, GMP_RNDD);
      mpfr_sub (r, x, r, GMP_RNDU);
    }
  mpfr_prec_round (r, q, GMP_RNDU);
#ifdef DEBUG
  printf("x-r="); mpfr_print_binary(r); putchar('\n');
  if (MPFR_SIGN(r)<0) { fprintf(stderr,"Error in mpfr_exp: r<0\n"); exit(1); }
#endif
  mpfr_div_2ui (r, r, K, GMP_RNDU); /* r = (x-n*log(2))/2^K, exact */

  TMP_MARK(marker);
  MY_INIT_MPZ(ss, 3 + 2*((q-1)/BITS_PER_MP_LIMB));
  exps = mpfr_get_z_exp (ss, s);
  /* s <- 1 + r/1! + r^2/2! + ... + r^l/l! */
  l = (precy < SWITCH) ? mpfr_exp2_aux (ss, r, q, &exps) /* naive method */
    : mpfr_exp2_aux2 (ss, r, q, &exps); /* Brent/Kung method */

#ifdef DEBUG
  printf ("l=%d q=%d (K+l)*q^2=%1.3e\n", l, q, (K+l)*(double)q*q);
#endif

  for (k = 0; k < K; k++)
    {
      mpz_mul (ss, ss, ss);
      exps <<= 1;
      exps += mpz_normalize (ss, ss, q);
    }
  mpfr_set_z (s, ss, GMP_RNDN);
  MPFR_SET_EXP(s, MPFR_GET_EXP (s) + exps);
  TMP_FREE(marker); /* don't need ss anymore */

  if (n>0) mpfr_mul_2ui(s, s, n, GMP_RNDU);
  else mpfr_div_2ui(s, s, -n, GMP_RNDU);

  /* error is at most 2^K*(3l*(l+1)) ulp for mpfr_exp2_aux */
  if (precy<SWITCH) l = 3*l*(l+1);
  else l = l*(l+4);
  k=0; while (l) { k++; l >>= 1; }
  /* now k = ceil(log(error in ulps)/log(2)) */
  K += k;
#ifdef DEBUG
    printf("after mult. by 2^n:\n");
    printf("s="); mpfr_print_binary(s); putchar('\n');
    printf("err=%d bits\n", K);
#endif

  l = mpfr_can_round (s, q - K, GMP_RNDN, GMP_RNDZ,
                      precy + (rnd_mode == GMP_RNDN));
  if (l==0) {
#ifdef DEBUG
     printf("not enough precision, use %d\n", q+BITS_PER_MP_LIMB);
     printf("q=%d q-K=%d precy=%d\n",q,q-K,precy);
#endif
     q += BITS_PER_MP_LIMB;
     mpfr_set_prec(r, q); mpfr_set_prec(s, q); mpfr_set_prec(t, q);
  }
  } while (l==0);

  inexact = mpfr_set (y, s, rnd_mode);

  mpfr_clear(r); mpfr_clear(s); mpfr_clear(t);
  return inexact;
}

/* s <- 1 + r/1! + r^2/2! + ... + r^l/l! while MPFR_EXP(r^l/l!)+MPFR_EXPR(r)>-q
   using naive method with O(l) multiplications.
   Return the number of iterations l.
   The absolute error on s is less than 3*l*(l+1)*2^(-q).
   Version using fixed-point arithmetic with mpz instead 
   of mpfr for internal computations.
   s must have at least qn+1 limbs (qn should be enough, but currently fails
   since mpz_mul_2exp(s, s, q-1) reallocates qn+1 limbs)
*/
static int
mpfr_exp2_aux (mpz_t s, mpfr_srcptr r, int q, int *exps)
{
  int l, dif, qn;
  mpz_t t, rr; mp_exp_t expt, expr;
  TMP_DECL(marker);

  TMP_MARK(marker);
  qn = 1 + (q-1)/BITS_PER_MP_LIMB;
  MY_INIT_MPZ(t, 2*qn+1); /* 2*qn+1 is needed since mpz_div_2exp may 
			      need an extra limb */
  MY_INIT_MPZ(rr, qn+1);
  mpz_set_ui(t, 1); expt=0;
  mpz_set_ui(s, 1); mpz_mul_2exp(s, s, q-1); *exps = 1-q; /* s = 2^(q-1) */
  expr = mpfr_get_z_exp(rr, r); /* no error here */

  l = 0;
  do {
    l++;
    mpz_mul(t, t, rr); expt=expt+expr;
    dif = *exps + mpz_sizeinbase(s, 2) - expt - mpz_sizeinbase(t, 2);
    /* truncates the bits of t which are < ulp(s) = 2^(1-q) */
    expt += mpz_normalize(t, t, q-dif); /* error at most 2^(1-q) */
    mpz_div_ui(t, t, l); /* error at most 2^(1-q) */
    /* the error wrt t^l/l! is here at most 3*l*ulp(s) */
#ifdef DEBUG
    if (expt != *exps) {
      fprintf(stderr, "Error: expt != exps %d %d\n", expt, *exps); exit(1);
    }
#endif
    mpz_add(s, s, t); /* no error here: exact */
    /* ensures rr has the same size as t: after several shifts, the error
       on rr is still at most ulp(t)=ulp(s) */
    expr += mpz_normalize(rr, rr, mpz_sizeinbase(t, 2));
  } while (mpz_cmp_ui(t, 0));

  TMP_FREE(marker);
  return l;
}

/* s <- 1 + r/1! + r^2/2! + ... + r^l/l! while MPFR_EXP(r^l/l!)+MPFR_EXPR(r)>-q
   using Brent/Kung method with O(sqrt(l)) multiplications.
   Return l.
   Uses m multiplications of full size and 2l/m of decreasing size, 
   i.e. a total equivalent to about m+l/m full multiplications,
   i.e. 2*sqrt(l) for m=sqrt(l).
   Version using mpz. ss must have at least (sizer+1) limbs.
   The error is bounded by (l^2+4*l) ulps where l is the return value.
*/
static int
mpfr_exp2_aux2 (mpz_t s, mpfr_srcptr r, int q, int *exps)
{
  int expr, l, m, i, sizer, *expR, expt, ql;
  mpz_t t, *R, rr, tmp;
  TMP_DECL(marker);

  /* estimate value of l */
  l = q / (- MPFR_GET_EXP (r));
  m = (int) __gmpfr_isqrt (l);
  /* we access R[2], thus we need m >= 2 */
  if (m < 2)
    m = 2;
  TMP_MARK(marker);
  R = (mpz_t*) TMP_ALLOC((m+1)*sizeof(mpz_t)); /* R[i] stands for r^i */
  expR = (int*) TMP_ALLOC((m+1)*sizeof(int)); /* exponent for R[i] */
  sizer = 1 + (MPFR_PREC(r)-1)/BITS_PER_MP_LIMB;
  mpz_init(tmp);
  MY_INIT_MPZ(rr, sizer+2);
  MY_INIT_MPZ(t, 2*sizer); /* double size for products */
  mpz_set_ui(s, 0); *exps = 1-q; /* initialize s to zero, 1 ulp = 2^(1-q) */
  for (i=0;i<=m;i++)
    MY_INIT_MPZ(R[i], sizer+2);
  expR[1] = mpfr_get_z_exp(R[1], r); /* exact operation: no error */
  expR[1] = mpz_normalize2(R[1], R[1], expR[1], 1-q); /* error <= 1 ulp */
  mpz_mul(t, R[1], R[1]); /* err(t) <= 2 ulps */
  mpz_div_2exp(R[2], t, q-1); /* err(R[2]) <= 3 ulps */
  expR[2] = 1-q;
  for (i=3;i<=m;i++)
    {
      mpz_mul(t, R[i-1], R[1]); /* err(t) <= 2*i-2 */
      mpz_div_2exp(R[i], t, q-1); /* err(R[i]) <= 2*i-1 ulps */
      expR[i] = 1-q;
    }
  mpz_set_ui(R[0], 1);
  mpz_mul_2exp(R[0], R[0], q-1);
  expR[0]=1-q; /* R[0]=1 */
  mpz_set_ui(rr, 1);
  expr=0; /* rr contains r^l/l! */
  /* by induction: err(rr) <= 2*l ulps */

  l = 0;
  ql = q; /* precision used for current giant step */
  do
    {
      /* all R[i] must have exponent 1-ql */
      if (l)
        for (i=0;i<m;i++)
          {
            expR[i] = mpz_normalize2(R[i], R[i], expR[i], 1-ql);
          }
      /* the absolute error on R[i]*rr is still 2*i-1 ulps */
      expt = mpz_normalize2(t, R[m-1], expR[m-1], 1-ql); 
      /* err(t) <= 2*m-1 ulps */
      /* computes t = 1 + r/(l+1) + ... + r^(m-1)*l!/(l+m-1)! 
         using Horner's scheme */
      for (i=m-2;i>=0;i--)
        {
          mpz_div_ui(t, t, l+i+1); /* err(t) += 1 ulp */
          mpz_add(t, t, R[i]);
        }
      /* now err(t) <= (3m-2) ulps */

      /* now multiplies t by r^l/l! and adds to s */
      mpz_mul(t, t, rr);
      expt += expr;
      expt = mpz_normalize2(t, t, expt, *exps);
      /* err(t) <= (3m-1) + err_rr(l) <= (3m-2) + 2*l */
#ifdef DEBUG
      if (expt != *exps)
        {
          fprintf(stderr, "Error: expt != exps %d %d\n", expt, *exps);
          exit(1);
        }
#endif
      mpz_add(s, s, t); /* no error here */

      /* updates rr, the multiplication of the factors l+i could be done 
         using binary splitting too, but it is not sure it would save much */
      mpz_mul(t, rr, R[m]); /* err(t) <= err(rr) + 2m-1 */
      expr += expR[m];
      mpz_set_ui (tmp, 1);
      for (i=1; i<=m; i++)
          mpz_mul_ui (tmp, tmp, l + i);
      mpz_fdiv_q(t, t, tmp); /* err(t) <= err(rr) + 2m */
      expr += mpz_normalize(rr, t, ql); /* err_rr(l+1) <= err_rr(l) + 2m+1 */
      ql = q - *exps - mpz_sizeinbase(s, 2) + expr + mpz_sizeinbase(rr, 2);
      l+=m;
    }
  while (expr+mpz_sizeinbase(rr, 2) > -q);

  TMP_FREE(marker);
  mpz_clear(tmp);
  return l;
}
