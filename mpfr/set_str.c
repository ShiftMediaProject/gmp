/* mpfr_set_str -- set a floating-point number from a string

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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
along with the MPFR Library; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_set_str (mpfr_ptr x, __gmp_const char *str, int base, mp_rnd_t rnd_mode)
{
  mpz_t mantissa;
  int negative, inex;
  long k = 0;
  unsigned char c;
  long e;
  mp_prec_t q;
  mpfr_t y, z;

  if (base < 2 || base > 36)
    return 1;

  mpz_init(mantissa);
  mpz_set_ui(mantissa, 0);

  negative = *str == '-';
  if (negative || *str == '+')
    str++;

  while (*str == '0')
    str++; /* skip initial zeros */

  /* allowed characters are '0' to '0'+base-1 if base <= 10,
     and '0' to '9' plus 'a' to 'a'+base-11 if 10 < base <= 36 */
  while (c = *str,
         (isdigit(c) && c < '0' + base) ||
         (islower(c) && c < 'a'-10 + base))
    {
      str++;
      mpz_mul_ui(mantissa, mantissa, base);
      mpz_add_ui(mantissa, mantissa, isdigit(c) ? c - '0' : c - ('a'-10));
    }

  /* k is the number of non-zero digits before the decimal point */

  if (*str == '.')
    {
      str++;
      while (c = *str,
             (isdigit(c) && c < '0' + base) ||
             (islower(c) && c < 'a'-10 + base))
	{
          if (k == LONG_MAX)
            {
              mpz_clear(mantissa);
              return -1;
            }
	  k++;
	  str++;
          mpz_mul_ui(mantissa, mantissa, base);
          mpz_add_ui(mantissa, mantissa, isdigit(c) ? c - '0' : c - ('a'-10));
	}
    }

  if (*str == '\0') /* no exponent */
    {
      e = -k;
    }
  else if ((base <= 10 && (*str == 'e' || *str == 'E')) || *str == '@')
    {
      char *endptr;

      if (*++str == '\0') /* exponent character but no exponent */
        {
          mpz_clear(mantissa);
          return 1;
        }

      errno = 0;
      e = strtol(str, &endptr, 10); /* signed exponent after 'e', 'E' or '@' */
      if (*endptr != '\0')
        {
          mpz_clear(mantissa);
          return 1;
        }
      if (errno)
        {
          mpz_clear(mantissa);
          return -1;
        }

      if (e < 0 && (unsigned long) e - k < (unsigned long) LONG_MIN)
        {
          mpz_clear(mantissa);
          return -1;
        }
      e -= k;
    }
  else /* unexpected character */
    {
      mpz_clear(mantissa);
      return 1;
    }

  /* the number is mantissa*base^expn */

  q = MPFR_PREC(x) & ~(mp_prec_t) (BITS_PER_MP_LIMB - 1);
  mpfr_init(y);
  mpfr_init(z);

  do
    {
      q += BITS_PER_MP_LIMB;
      mpfr_set_prec(y, q);
      mpfr_set_z(y, mantissa, GMP_RNDN); /* error <= 1/2*ulp(y) */

      mpfr_set_prec(z, q);
      if (e > 0)
        {
          inex = mpfr_ui_pow_ui(z, base, e, GMP_RNDN);
          mpfr_mul(y, y, z, GMP_RNDN);
        }
      else if (e < 0)
        {
          inex = mpfr_ui_pow_ui(z, base, -e, GMP_RNDN);
          mpfr_div(y, y, z, GMP_RNDN);
        }
      else
        inex = 1;
      if (negative)
        mpfr_neg(y, y, GMP_RNDN);
    }
  while (mpfr_can_round(y, q-inex, GMP_RNDN, rnd_mode, MPFR_PREC(x))==0
         && q<=2*MPFR_PREC(x));

  mpfr_set(x, y, rnd_mode);

  mpz_clear(mantissa);
  mpfr_clear(y);
  mpfr_clear(z);
  return 0;
}

int
mpfr_init_set_str (mpfr_ptr x, char *str, int base, mp_rnd_t rnd_mode)
{
  mpfr_init (x);
  return mpfr_set_str (x, str, base, rnd_mode);
}
