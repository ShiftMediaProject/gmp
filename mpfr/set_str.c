/* mpfr_set_str -- set a floating-point number from a string

Copyright (C) 2000, 2001 Free Software Foundation, Inc.

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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_set_str (mpfr_ptr x, __gmp_const char *str, int base, mp_rnd_t rnd_mode)
{
  char negative = 0, *endptr;
  unsigned long k = 0, l, q;
  long expn = 0, e;
  mpz_t mantissa;
  mpfr_t y, z;

  mpz_init(mantissa); mpz_set_ui(mantissa, 0);
  l = strlen(str);
  if (*str == '-') { negative = 1; str++; l--; }
  else if (*str == '+') { str++; l--; }

  while (*str == '0') { str++; l--; } /* skip initial zeros */

  /* allowed characters are '0' to '0'+base-1 if base <= 10,
     and '0' to '9' plus 'a' to 'a'+base-11 if 10 < base <= 36 */
  while ((isdigit((unsigned char) *str) && (unsigned char) *str < '0'+base)
      || (islower((unsigned char) *str) && (unsigned char) *str < 'a'+base-10))
    { 
      mpz_mul_ui(mantissa, mantissa, base);
      mpz_add_ui(mantissa, mantissa, isdigit((unsigned char) *str) ?
		 (*str)-'0' : (*str)-'a'+10);
      str++; l--;
    }

  /* k is the number of non-zero digits before the decimal point */

  if (*str == '.') 
    {
      str++; l--;
      while ((isdigit((unsigned char) *str) && (unsigned char) *str < '0'+base)
      || (islower((unsigned char) *str) && (unsigned char) *str < 'a'+base-10))
	{ 
	  mpz_mul_ui(mantissa, mantissa, base);
	  mpz_add_ui(mantissa, mantissa, isdigit((unsigned char) *str) ?
		     (*str)-'0' : (*str)-'a'+10);
	  str++; l--;
	  k++;
	}
    }
    
  if ((base <= 10 && (*str == 'e' || *str == 'E')) || *str == '@')
    {
      str++; l--;
      e = strtol(str, &endptr, 10); /* signed exponent after 'e', 'E' or '@' */
      expn = e - k; 
      if (expn > e)
	  fprintf(stderr, "Warning: exponent underflow in mpfr_set_str\n"); 
    }
  else if (l) {
    /* unexpected end of string */
    return -1;
  }
  else {
    expn = -k;
    endptr = (char*) str;
  }

  /* the number is mantissa*base^expn */

  q = (MPFR_PREC(x)/BITS_PER_MP_LIMB)*BITS_PER_MP_LIMB;
  mpfr_init(y);
  mpfr_init(z);

  do {
    q += BITS_PER_MP_LIMB;
    mpfr_set_prec(y, q);
    mpfr_set_z(y, mantissa, GMP_RNDN); /* error <= 1/2*ulp(y) */

    mpfr_set_prec(z, q);
    if (expn>0) {
      e = mpfr_ui_pow_ui(z, base, expn, GMP_RNDN);
      mpfr_mul(y, y, z, GMP_RNDN);
    }
    else if (expn<0) {
      e = mpfr_ui_pow_ui(z, base, -expn, GMP_RNDN);
      mpfr_div(y, y, z, GMP_RNDN);
    }
    else e=1;
    if (negative) mpfr_neg(y, y, GMP_RNDN);

    /* now y is an approximation of mantissa*base^expn with error at most
       2^e*ulp(y) */

  } while (mpfr_can_round(y, q-e, GMP_RNDN, rnd_mode, MPFR_PREC(x))==0
	   && q<=2*MPFR_PREC(x));

  mpfr_set(x, y, rnd_mode);

  mpz_clear(mantissa);
  mpfr_clear(y);
  mpfr_clear(z);
  return ((*endptr=='\0') ? 0 : -1);
}

int
mpfr_init_set_str (mpfr_ptr x, char *str, int base, mp_rnd_t rnd_mode)
{ 
  mpfr_init (x);
  return mpfr_set_str (x, str, base, rnd_mode);
}

