/* mpfr_get_str -- output a floating-point number to a string

Copyright (C) 1999, 2001 Free Software Foundation, Inc.

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
#include <string.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/*
  Convert op to a string in base 'base' with 'n' digits and writes the 
  mantissa in 'str', the exponent in 'expptr'.
  The result is rounded wrt 'rnd_mode'.

  For op = 3.1416 we get str = "31416" and expptr=1.
 */
char*
mpfr_get_str (char *str, mp_exp_t *expptr, int base, size_t n,
		  mpfr_srcptr op, mp_rnd_t rnd_mode)
{
  double d;
  long e, q, div, p, err, prec, sh;
  mpfr_t a, b;
  mpz_t bz;
  char *str0=NULL;
  mp_rnd_t rnd1;
  int f, pow2, ok=0, neg, str_is_null=(str==NULL);

  if (base<2 || 36<base) {
    fprintf(stderr, "Error: too small or too large base in mpfr_get_str: %d\n",
	    base);
    exit(1);
  }
  
  neg = MPFR_SIGN(op) < 0;

  if (MPFR_IS_INF(op)) { 
    if (str == NULL)
      str = (*__gmp_allocate_func)(neg + 4);
    str0 = str;
    if (neg) { *str++ = '-'; }
    *str++ = 'I'; *str++ = 'n'; *str++ = 'f'; *str='\0';
    return str0; /* strlen(str0) = neg + 3 */
  }

  if (!MPFR_NOTZERO(op)) {
    if (str == NULL)
      str = (*__gmp_allocate_func)(neg + n + 1);
    str0 = str;
    if (neg) *str++ = '-';
    for (f=0;f<n;f++) *str++ = '0';
    *str++ = '\0';
    *expptr = 1;
    return str0; /* strlen(str0) = neg + n */
  }

  count_leading_zeros(pow2, (mp_limb_t) base);
  pow2 = BITS_PER_MP_LIMB - pow2 - 1;
  if (base != (1<<pow2)) pow2=0; 
  /* if pow2 <> 0, then base = 2^pow2 */

  /* first determines the exponent */
  e = MPFR_EXP(op); 
  d = ABS(mpfr_get_d2(op, 0));
  /* the absolute value of op is between 1/2*2^e and 2^e */
  /* the output exponent f is such that base^(f-1) <= |op| < base^f
     i.e. f = 1 + floor(log(|op|)/log(base))
     = 1 + floor((log(|m|)+e*log(2))/log(base)) */
  /* f = 1 + (int) floor((log(d)/LOG2+(double)e)*LOG2/log((double)base)); */
  /* when base = 2^pow2, then |op| < 2^(pow2*f)
     i.e. e <= pow2*f and f = ceil(e/pow2) */
  if (pow2)
      f = ((e < 0) ? e : (e + pow2 - 1)) / pow2;
  else
    {
      d = ((double) e + (double) _mpfr_floor_log2(d))
                      * __mp_bases[base].chars_per_bit_exactly;
      /* warning: (int) d rounds towards 0 */
      f = (int) d; /* f equals floor(d) if d >= 0 and ceil(d) if d < 0 */
      if (f <= d) f++;
    }
  if (n==0) {
    /* performs exact rounding, i.e. returns y such that for GMP_RNDU
       for example, we have:       x*2^(e-p) <= y*base^(f-n)
     */
    n = (int) (__mp_bases[base].chars_per_bit_exactly * MPFR_PREC(op));
    if (n==0) n=1;
  }
  /* now the first n digits of the mantissa are obtained from
     rnd(op*base^(n-f)) */
  if (pow2) prec = n*pow2;
  else
    prec = 1 + (long) ((double) n / __mp_bases[base].chars_per_bit_exactly);
  err = 5;
  q = prec + err;
  /* one has to use at least q bits */
  q = (((q-1)/BITS_PER_MP_LIMB)+1)*BITS_PER_MP_LIMB;
  mpfr_init2(a, q); mpfr_init2(b, q);

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

    if (pow2)
      {
	if (div)
	  mpfr_div_2exp (b, op, pow2*p, rnd_mode);
	else
	  mpfr_mul_2exp (b, op, pow2*p, rnd_mode);
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
    if (neg) MPFR_CHANGE_SIGN(b); /* put b positive */
    if (q>2*prec+BITS_PER_MP_LIMB) {
      /* if the intermediate precision exceeds twice that of the input,
	 a worst-case for the division cannot occur */
      ok=1;
      rnd_mode=GMP_RNDN;
    }
    else ok = pow2 || mpfr_can_round(b, q-err, rnd_mode, rnd_mode, prec);

  } while (ok==0 && (q+=BITS_PER_MP_LIMB) );

  if (neg)
    switch (rnd_mode) {
    case GMP_RNDU: rnd_mode=GMP_RNDZ; break;
    case GMP_RNDD: rnd_mode=GMP_RNDU; break;
  }

  if (ok)
    mpfr_round (b, rnd_mode, MPFR_EXP(b));

  prec=MPFR_EXP(b); /* may have changed due to rounding */

  /* now the mantissa is the integer part of b */
  mpz_init(bz); q=1+(prec-1)/BITS_PER_MP_LIMB;
  _mpz_realloc(bz, q);
  sh = prec%BITS_PER_MP_LIMB;
  e = 1 + (MPFR_PREC(b)-1)/BITS_PER_MP_LIMB-q;
  if (sh) mpn_rshift(PTR(bz), MPFR_MANT(b)+e, q, BITS_PER_MP_LIMB-sh);
  else MPN_COPY(PTR(bz), MPFR_MANT(b)+e, q);
  bz->_mp_size=q;

  /* computes the number of characters needed */
  q = neg + n + 2; /* n+1 may not be enough for 100000... */
  if (str == NULL) {
    str0 = str = (*__gmp_allocate_func) (q);
  }
  if (neg) *str++='-';
  mpz_get_str(str, base, bz); /* n digits of mantissa */
  if (strlen(str)==n+1) {
    f++; /* possible due to rounding */
    str[n]='\0'; /* ensures we get only n digits of output */
  }
  else if (strlen(str)==n-1) {
    f--;
    str[n-1]='0';
    str[n]='\0';
  }
  *expptr = f;
  mpfr_clear(a); mpfr_clear(b); mpz_clear(bz);

  /* if the given string was null, ensure we return a block which is exactly
     strlen(str)+1 bytes long (useful for __gmp_free_func and the C++ wrapper)
  */
  if (str_is_null && ((strlen(str0) + 1) != q))
    str0 = (char*) (*__gmp_reallocate_func) (str0, q, strlen(str0) + 1);

  return str0;
}
