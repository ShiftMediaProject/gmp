/* mpfr_exp2 -- exponential of a floating-point number 
                using Brent's algorithms in O(n^(1/2)*M(n)) and O(n^(1/3)*M(n))

Copyright (C) 1999-2000, 2001 Free Software Foundation.

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
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int mpfr_exp2_aux      _PROTO ((mpz_t, mpfr_srcptr, int, int*));
int mpfr_exp2_aux2     _PROTO ((mpz_t, mpfr_srcptr, int, int*));
mp_exp_t mpz_normalize _PROTO ((mpz_t, mpz_t, int));
int mpz_normalize2     _PROTO ((mpz_t, mpz_t, int, int));
int mpfr_exp2          _PROTO ((mpfr_ptr, mpfr_srcptr, mp_rnd_t));

/* returns floor(sqrt(n)) */
unsigned long
_mpfr_isqrt (unsigned long n)
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
_mpfr_cuberoot (unsigned long n)
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
mp_exp_t
#if __STDC__
mpz_normalize (mpz_t rop, mpz_t z, int q)
#else
mpz_normalize (rop, z, q)
     mpz_t rop;
     mpz_t z;
     int q;
#endif
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
int
#if __STDC__
mpz_normalize2 (mpz_t rop, mpz_t z, int expz, int target)
#else
mpz_normalize2 (rop, z, expz, target)
     mpz_t rop;
     mpz_t z;
     int expz;
     int target;
#endif
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
#if __STDC__
mpfr_exp2 (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode) 
#else
mpfr_exp2 (y, x, rnd_mode)
     mpfr_ptr y;
     mpfr_srcptr x;
     mp_rnd_t rnd_mode;
#endif
{
  int n, expx, K, precy, q, k, l, err, exps; 
  mpfr_t r, s, t; mpz_t ss;
  TMP_DECL(marker);

  if (MPFR_IS_NAN(x)) { MPFR_SET_NAN(y); return 1; }
  if (MPFR_IS_INF(x)) 
    { 
      if (MPFR_SIGN(x) > 0) 
	{ MPFR_SET_INF(y); if (MPFR_SIGN(y) == -1) { MPFR_CHANGE_SIGN(y); } }
      else 
	{ MPFR_SET_ZERO(y);  if (MPFR_SIGN(y) == -1) { MPFR_CHANGE_SIGN(y); } }
      /*    TODO: conflits entre infinis et zeros ? */
	    }
  if (!MPFR_NOTZERO(x)) { mpfr_set_ui(y, 1, GMP_RNDN); return 0; }

  expx = MPFR_EXP(x);
  precy = MPFR_PREC(y);
#ifdef DEBUG
  printf("MPFR_EXP(x)=%d\n",expx);
#endif

  /* if x > (2^31-1)*ln(2), then exp(x) > 2^(2^31-1) i.e. gives +infinity */
  if (expx > 30) {
    if (MPFR_SIGN(x) > 0) {
      MPFR_SET_INF(y);
      if (MPFR_SIGN(y) < 0) MPFR_CHANGE_SIGN(y);
      return 1;
    }
    else { MPFR_SET_ZERO(y); return 1; }
  }

  /* if x < 2^(-precy), then exp(x) i.e. gives 1 +/- 1 ulp(1) */
  if (expx < -precy) { int signx = MPFR_SIGN(x);
    mpfr_set_ui(y, 1, rnd_mode);
    if (signx>0 && rnd_mode==GMP_RNDU) mpfr_add_one_ulp(y);
    else if (signx<0 && (rnd_mode==GMP_RNDD || rnd_mode==GMP_RNDZ)) 
      mpfr_sub_one_ulp(y);
    return 1; }

  n = (int) (mpfr_get_d(x) / LOG2);

  /* for the O(n^(1/2)*M(n)) method, the Taylor series computation of
     n/K terms costs about n/(2K) multiplications when computed in fixed
     point */
  K = (precy<SWITCH) ? _mpfr_isqrt((precy + 1) / 2) : _mpfr_cuberoot (4*precy);
  l = (precy-1)/K + 1;
  err = K + (int) _mpfr_ceil_log2 (2.0 * (double) l + 18.0);
  /* add K extra bits, i.e. failure probability <= 1/2^K = O(1/precy) */
  q = precy + err + K + 3;
  mpfr_init2(r, q); mpfr_init2(s, q); mpfr_init2(t, q);
  /* the algorithm consists in computing an upper bound of exp(x) using
     a precision of q bits, and see if we can round to MPFR_PREC(y) taking
     into account the maximal error. Otherwise we increase q. */
  do {
#ifdef DEBUG
  printf("n=%d K=%d l=%d q=%d\n",n,K,l,q);
#endif

  /* if n<0, we have to get an upper bound of log(2)
     in order to get an upper bound of r = x-n*log(2) */
  mpfr_const_log2 (s, (n>=0) ? GMP_RNDZ : GMP_RNDU);
#ifdef DEBUG
  printf("n=%d log(2)=",n); mpfr_print_raw(s); putchar('\n');
#endif
  mpfr_mul_ui(r, s, (n<0) ? -n : n, (n>=0) ? GMP_RNDZ : GMP_RNDU); 
  if (n<0) mpfr_neg(r, r, GMP_RNDD);
  /* r = floor(n*log(2)) */

#ifdef DEBUG
  printf("x=%1.20e\n",mpfr_get_d(x));
  printf(" ="); mpfr_print_raw(x); putchar('\n');
  printf("r=%1.20e\n",mpfr_get_d(r));
  printf(" ="); mpfr_print_raw(r); putchar('\n');
#endif
  mpfr_sub(r, x, r, GMP_RNDU);
  if (MPFR_SIGN(r)<0) { /* initial approximation n was too large */
    n--;
    mpfr_mul_ui(r, s, (n<0) ? -n : n, GMP_RNDZ);
    if (n<0) mpfr_neg(r, r, GMP_RNDD);
    mpfr_sub(r, x, r, GMP_RNDU);
  }
#ifdef DEBUG
  printf("x-r=%1.20e\n",mpfr_get_d(r));
  printf(" ="); mpfr_print_raw(r); putchar('\n');
  if (MPFR_SIGN(r)<0) { fprintf(stderr,"Error in mpfr_exp: r<0\n"); exit(1); }
#endif
  mpfr_div_2exp(r, r, K, GMP_RNDU); /* r = (x-n*log(2))/2^K */

  TMP_MARK(marker);
  MY_INIT_MPZ(ss, 3 + 2*((q-1)/BITS_PER_MP_LIMB));
  exps = mpz_set_fr(ss, s);
  /* s <- 1 + r/1! + r^2/2! + ... + r^l/l! */
  l = (precy<SWITCH) ? mpfr_exp2_aux(ss, r, q, &exps) /* naive method */
    : mpfr_exp2_aux2(ss, r, q, &exps); /* Brent/Kung method */

#ifdef DEBUG
  printf("l=%d q=%d (K+l)*q^2=%1.3e\n", l, q, (K+l)*(double)q*q);
#endif

  for (k=0;k<K;k++) {
    mpz_mul(ss, ss, ss); exps <<= 1;
    exps += mpz_normalize(ss, ss, q);
  }
  mpfr_set_z(s, ss, GMP_RNDN); MPFR_EXP(s) += exps;
  TMP_FREE(marker); /* don't need ss anymore */

  if (n>0) mpfr_mul_2exp(s, s, n, GMP_RNDU);
  else mpfr_div_2exp(s, s, -n, GMP_RNDU);

  /* error is at most 2^K*(3l*(l+1)) ulp for mpfr_exp2_aux */
  if (precy<SWITCH) l = 3*l*(l+1);
  else l = l*(l+4);
  k=0; while (l) { k++; l >>= 1; }
  /* now k = ceil(log(error in ulps)/log(2)) */
  K += k;
#ifdef DEBUG
    printf("after mult. by 2^n:\n");
    if (MPFR_EXP(s)>-1024) printf("s=%1.20e\n",mpfr_get_d(s)); 
    printf(" ="); mpfr_print_raw(s); putchar('\n');
    printf("err=%d bits\n", K);
#endif

  l = mpfr_can_round(s, q-K, GMP_RNDN, rnd_mode, precy);
  if (l==0) {
#ifdef DEBUG
     printf("not enough precision, use %d\n", q+BITS_PER_MP_LIMB);
     printf("q=%d q-K=%d precy=%d\n",q,q-K,precy);
#endif
     q += BITS_PER_MP_LIMB;
     mpfr_set_prec(r, q); mpfr_set_prec(s, q); mpfr_set_prec(t, q);
  }
  } while (l==0);

  mpfr_set(y, s, rnd_mode);

  mpfr_clear(r); mpfr_clear(s); mpfr_clear(t);
  return 1;
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
int
#if __STDC__
mpfr_exp2_aux(mpz_t s, mpfr_srcptr r, int q, int *exps)
#else
mpfr_exp2_aux(s, r, q, exps)
     mpz_t s;
     mpfr_srcptr r;
     int q;
     int *exps;
#endif
{
  int l, dif, qn;
  mpz_t t, rr; mp_exp_t expt, expr;
  TMP_DECL(marker);

  TMP_MARK(marker);
  qn = 1 + (q-1)/BITS_PER_MP_LIMB;
  MY_INIT_MPZ(t, 2*qn+1); /* 2*qn+1 is neeeded since mpz_div_2exp may 
			      need an extra limb */
  MY_INIT_MPZ(rr, qn+1);
  mpz_set_ui(t, 1); expt=0;
  mpz_set_ui(s, 1); mpz_mul_2exp(s, s, q-1); *exps = 1-q; /* s = 2^(q-1) */
  expr = mpz_set_fr(rr, r); /* no error here */

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
int
#if __STDC__
mpfr_exp2_aux2 (mpz_t s, mpfr_srcptr r, int q, int *exps)
#else
mpfr_exp2_aux2 (s, r, q, exps)
     mpz_t s;
     mpfr_srcptr r;
     int q;
     int *exps;
#endif
{
  int expr, l, m, i, sizer, *expR, expt, ql;
  unsigned long int c;
  mpz_t t, *R, rr, tmp;
  TMP_DECL(marker);

  /* estimate value of l */
  l = q / (-MPFR_EXP(r));
  m = (int) _mpfr_isqrt (l);
  /* we access R[2], thus we need m >= 2 */
  if (m < 2) m = 2;
  TMP_MARK(marker);
  R = (mpz_t*) TMP_ALLOC((m+1)*sizeof(mpz_t)); /* R[i] stands for r^i */
  expR = (int*) TMP_ALLOC((m+1)*sizeof(int)); /* exponent for R[i] */
  sizer = 1 + (MPFR_PREC(r)-1)/BITS_PER_MP_LIMB;
  mpz_init(tmp);
  MY_INIT_MPZ(rr, sizer+2);
  MY_INIT_MPZ(t, 2*sizer); /* double size for products */
  mpz_set_ui(s, 0); *exps = 1-q; /* initialize s to zero, 1 ulp = 2^(1-q) */
  for (i=0;i<=m;i++) MY_INIT_MPZ(R[i], sizer+2);
  expR[1] = mpz_set_fr(R[1], r); /* exact operation: no error */
  expR[1] = mpz_normalize2(R[1], R[1], expR[1], 1-q); /* error <= 1 ulp */
  mpz_mul(t, R[1], R[1]); /* err(t) <= 2 ulps */
  mpz_div_2exp(R[2], t, q-1); /* err(R[2]) <= 3 ulps */
  expR[2] = 1-q;
  for (i=3;i<=m;i++) {
    mpz_mul(t, R[i-1], R[1]); /* err(t) <= 2*i-2 */
    mpz_div_2exp(R[i], t, q-1); /* err(R[i]) <= 2*i-1 ulps */
    expR[i] = 1-q;
  }
  mpz_set_ui(R[0], 1); mpz_mul_2exp(R[0], R[0], q-1); expR[0]=1-q; /* R[0]=1 */
  mpz_set_ui(rr, 1); expr=0; /* rr contains r^l/l! */
  /* by induction: err(rr) <= 2*l ulps */

  l = 0;
  ql = q; /* precision used for current giant step */
  do {
    /* all R[i] must have exponent 1-ql */
    if (l) for (i=0;i<m;i++) {
      expR[i] = mpz_normalize2(R[i], R[i], expR[i], 1-ql);
    }
    /* the absolute error on R[i]*rr is still 2*i-1 ulps */
    expt = mpz_normalize2(t, R[m-1], expR[m-1], 1-ql); 
    /* err(t) <= 2*m-1 ulps */
    /* computes t = 1 + r/(l+1) + ... + r^(m-1)*l!/(l+m-1)! 
       using Horner's scheme */
    for (i=m-2;i>=0;i--) {
      mpz_div_ui(t, t, l+i+1); /* err(t) += 1 ulp */
      mpz_add(t, t, R[i]);
    }
    /* now err(t) <= (3m-2) ulps */
    
    /* now multiplies t by r^l/l! and adds to s */
    mpz_mul(t, t, rr); expt += expr;
    expt = mpz_normalize2(t, t, expt, *exps);
    /* err(t) <= (3m-1) + err_rr(l) <= (3m-2) + 2*l */
#ifdef DEBUG
    if (expt != *exps) {
      fprintf(stderr, "Error: expt != exps %d %d\n", expt, *exps); exit(1);
    }
#endif
    mpz_add(s, s, t); /* no error here */

    /* updates rr, the multiplication of the factors l+i could be done 
       using binary splitting too, but it is not sure it would save much */
    mpz_mul(t, rr, R[m]); /* err(t) <= err(rr) + 2m-1 */
    expr += expR[m];
    mpz_set_ui (tmp, 1);
    for (i=1, c=1; i<=m; i++) {
      if (l+i > ~((unsigned long int) 0)/c) {
	mpz_mul_ui(tmp, tmp, c);
	c = l+i;
      }
      else c *= (unsigned long int) l+i;
    }
    if (c != 1) mpz_mul_ui (tmp, tmp, c); /* tmp is exact */
    mpz_fdiv_q(t, t, tmp); /* err(t) <= err(rr) + 2m */
    expr += mpz_normalize(rr, t, ql); /* err_rr(l+1) <= err_rr(l) + 2m+1 */
    ql = q - *exps - mpz_sizeinbase(s, 2) + expr + mpz_sizeinbase(rr, 2);
    l+=m;
  } while (expr+mpz_sizeinbase(rr, 2) > -q);

  TMP_FREE(marker);
  mpz_clear(tmp);
  return l;
}
