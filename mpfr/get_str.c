/* mpfr_get_str -- output a floating-point number to a string

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"

/*
  Convert op to a string in base 'base' with 'n' digits and writes the 
  mantissa in 'str', the exponent in 'expptr'.
  The result is rounded wrt 'rnd_mode'.

  For op = 3.1416 we get str = "31416" and expptr=1.
 */
#if __STDC__
char *mpfr_get_str(char *str, mp_exp_t *expptr, int base, size_t n,
		  mpfr_srcptr op, unsigned char rnd_mode)
#else
char *mpfr_get_str(str, expptr, base, n, op, rnd_mode)
     char *str;  
     mp_exp_t *expptr;
     int base;
     size_t n;
     mpfr_srcptr op; 
     unsigned char rnd_mode; 
#endif
{
  double d; long e, q, div, p, err, prec, sh; mpfr_t a, b; mpz_t bz;
  char *str0; unsigned char rnd1; int f, pow2, ok=0, neg;

  if (base<2 || 36<base) {
    fprintf(stderr, "Error: too small or too large base in mpfr_get_str: %d\n",
	    base);
    exit(1);
  }

  neg = (SIGN(op)<0) ? 1 : 0;

  if (!NOTZERO(op)) {
    if (str==NULL) str0=str=(*_mp_allocate_func)(neg + n + 2);
    if (SIGN(op)<0) *str++ = '-';
    for (f=0;f<n;f++) *str++ = '0';
    *expptr = 1;
    return str0;
  }

  count_leading_zeros(pow2, (mp_limb_t)base); 
  pow2 = BITS_PER_MP_LIMB - pow2 - 1;
  if (base != (1<<pow2)) pow2=0; 
  /* if pow2 <> 0, then base = 2^pow2 */

  /* first determines the exponent */
  e = EXP(op); 
  d = fabs(mpfr_get_d2(op, 0));
  /* the absolute value of op is between 1/2*2^e and 2^e */
  /* the output exponent f is such that base^(f-1) <= |op| < base^f
     i.e. f = 1 + floor(log(|op|)/log(base))
     = 1 + floor((log(|m|)+e*log(2))/log(base)) */
  f = 1 + (int) floor((log(d)+(double)e*log(2.0))/log((double)base));
  if (n==0) {
    /* performs exact rounding, i.e. returns y such that for rnd_mode=RNDN
       for example, we have:
       y*base^(f-n) <= x*2^(e-p) < (x+1)*2^(e-p) <= (y+1)*base^(f-n)
       which implies 2^(EXP(op)-PREC(op)) <= base^(f-n)
     */
    n = f + (int) ceil(((double)PREC(op)-e)*log(2.0)/log((double)base));
  }
  /* now the first n digits of the mantissa are obtained from
     rnd(op*base^(n-f)) */
  prec = (long) ceil((double)n*log((double)base)/log(2.0));
  err = 5;
  q = prec+err;
  /* one has to use at least q bits */
  q = (((q-1)/BITS_PER_MP_LIMB)+1)*BITS_PER_MP_LIMB;
  mpfr_init2(a,q); mpfr_init2(b,q);

  do {
    p = n-f; if ((div=(p<0))) p=-p;
    rnd1 = rnd_mode;
    if (div) {
      /* if div we divide by base^p so we have to invert the rounding mode */
      switch (rnd1) {
      case GMP_RNDN: rnd1=GMP_RNDN; break;
      case GMP_RNDZ: rnd1=GMP_RNDU; break;
      case GMP_RNDU: rnd1=GMP_RNDZ; break;
      case GMP_RNDD: rnd1=GMP_RNDZ; break;
      }
    }

    if (pow2) {
      if (div) mpfr_div_2exp(b, op, pow2*p, rnd_mode);
      else mpfr_mul_2exp(b, op, pow2*p, rnd_mode);
    } 
    else {
       /* compute base^p with q bits and rounding towards zero */
       mpfr_set_prec(b, q);
       if (p==0) { mpfr_set(b, op, rnd_mode); mpfr_set_ui(a, 1, rnd_mode); }
       else {
	 mpfr_set_prec(a, q);
	 mpfr_ui_pow_ui(a, base, p, rnd1);
	 if (div) {
	   mpfr_set_ui(b, 1, rnd_mode);
	   mpfr_div(a, b, a, rnd_mode);
	 }
	 /* now a is an approximation by default of 1/base^(f-n) */
	 mpfr_mul(b, op, a, rnd_mode);
       }
    }
    if (neg) CHANGE_SIGN(b); /* put b positive */

    if (q>2*prec+BITS_PER_MP_LIMB) {
      /* happens when just in the middle between two digits */
      n--; q-=BITS_PER_MP_LIMB;
      if (n==0) {
          fprintf(stderr, "cannot determine leading digit\n"); exit(1);
        }
    }
    ok = pow2 || mpfr_can_round(b, q-err, rnd_mode, rnd_mode, prec);

    if (ok) { 
      if (pow2) {
	sh = e-PREC(op) + pow2*(n-f); /* error at most 2^e */
	ok = mpfr_can_round(b, EXP(b)-sh-1, rnd_mode, rnd_mode, n*pow2);
      }
      else {
	 /* check that value is the same at distance 2^(e-PREC(op))/base^(f-n)
	  in opposite from rounding direction */
	 if (e>=PREC(op)) mpfr_mul_2exp(a, a, e-PREC(op), rnd_mode);
	 else mpfr_div_2exp(a, a, PREC(op)-e, rnd_mode);
	 if (rnd_mode==GMP_RNDN) {
	   mpfr_div_2exp(a, a, 2, rnd_mode);
	   mpfr_sub(b, b, a, rnd_mode); /* b - a/2 */
	   mpfr_mul_2exp(a, a, 2, rnd_mode);
	   mpfr_add(a, b, a, rnd_mode); /* b + a/2 */
	 }
	 else if ((rnd_mode==GMP_RNDU && neg==0) || (rnd_mode==GMP_RNDD && neg))
	   mpfr_sub(a, b, a, rnd_mode);
	 else mpfr_add(a, b, a, rnd_mode);
	 /* check that a and b are rounded similarly */
	 prec=EXP(b);
	 if (EXP(a) != prec) ok=0;
	 else {
	   mpfr_round(b, rnd_mode, prec);
	   mpfr_round(a, rnd_mode, prec);
	   if (mpfr_cmp(a, b)) ok=0;
	 }
       }
      if (ok==0) { /* n is too large */
	n--;
	if (n==0) {
	  fprintf(stderr, "cannot determine leading digit\n"); exit(1);
	}
	q -= BITS_PER_MP_LIMB;
      }
    }
  } while (ok==0 && (q+=BITS_PER_MP_LIMB) );
  if (neg)
    switch (rnd_mode) {
    case GMP_RNDU: rnd_mode=GMP_RNDZ; break;
    case GMP_RNDD: rnd_mode=GMP_RNDU; break;
  }

  prec=EXP(b); /* may have changed due to rounding */

  /* now the mantissa is the integer part of b */
  mpz_init(bz); q=1+(prec-1)/BITS_PER_MP_LIMB;
  _mpz_realloc(bz, q);
  sh = prec%BITS_PER_MP_LIMB;
  e = 1 + (PREC(b)-1)/BITS_PER_MP_LIMB-q;
  if (sh) mpn_rshift(PTR(bz), MANT(b)+e, q, BITS_PER_MP_LIMB-sh);
  else MPN_COPY(PTR(bz), MANT(b)+e, q);
  bz->_mp_size=q;

  /* computes the number of characters needed */
  q = neg + n + 2; /* n+1 may not be enough for 100000... */
  if (str==NULL) str0=str=(*_mp_allocate_func)(q);
  if (neg) *str++='-';
  mpz_get_str(str, base, bz); /* n digits of mantissa */
  if (strlen(str)==n+1) f++; /* possible due to rounding */
  *expptr = f;
  mpfr_clear(a); mpfr_clear(b); mpz_clear(bz);
  return str0;
}

