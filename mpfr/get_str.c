/* mpfr_get_str -- output a floating-point number to a string

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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

#include <stdlib.h>
#include <string.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#define ERR 5

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
  int neg;

  if (base < 2 || base > 36)
    return NULL;

  if (n == 0) /* determine n from precision of op */
    {
      n = __mp_bases[base].chars_per_bit_exactly * MPFR_PREC(op);
      if (n < 2)
        n = 2;
    }

  /* Do not use MPFR_PREC_MIN as this applies to base 2 only. Perhaps we
     should allow n == 1 for directed rounding modes and odd bases. */
  if (n < 2)
    return NULL;

  if (MPFR_IS_NAN(op))
    {
      if (str == NULL)
        str = (*__gmp_allocate_func)(4);
      str[0] = 'N';
      str[1] = 'a';
      str[2] = 'N';
      str[3] = '\0';
      return str;
    }

  neg = MPFR_SIGN(op) < 0; /* 0 if positive, 1 if negative */

  if (MPFR_IS_INF(op))
    {
      char *str0;

      if (str == NULL)
        str = (*__gmp_allocate_func)(neg + 4);
      str0 = str;
      if (neg)
        *str++ = '-';
      *str++ = 'I';
      *str++ = 'n';
      *str++ = 'f';
      *str = '\0';
      return str0; /* strlen(str0) = neg + 3 */
    }

  /* op is a floating-point number */

  if (MPFR_IS_ZERO(op))
    {
      char *str0;

      if (str == NULL)
        str = (*__gmp_allocate_func)(neg + n + 1);
      str0 = str;
      if (neg)
        *str++ = '-';
      memset(str, '0', n);
      str[n] = '\0';
      *expptr = 0; /* a bit like frexp() in ISO C99 */
      return str0; /* strlen(str0) = neg + n */
    }
  else
    {
      int str_is_null;
      int pow2;
      mp_exp_t e, f;
      mpfr_t a, b;
      mpz_t bz;
      char *str0;
      size_t len;

      str_is_null = str == NULL;

      if (IS_POW2(base)) /* Is base a power of 2? */
        {
          count_leading_zeros (pow2, (mp_limb_t) base);
          pow2 = BITS_PER_MP_LIMB - pow2 - 1; /* base = 2^pow2 */
        }
      else
        pow2 = 0;

      /* first determines the exponent */
      e = MPFR_EXP(op);

      /* the absolute value of op is between 1/2*2^e and 2^e */
      /* the output exponent f is such that base^(f-1) <= |op| < base^f
         i.e. f = 1 + floor(log(|op|)/log(base))
         = 1 + floor((log(|m|)+e*log(2))/log(base)) */
      /* f = 1 + (int)floor((log(d)/LOG2+(double)e)*LOG2/log((double)base)); */
      /* when base = 2^pow2, then |op| < 2^(pow2*f)
         i.e. e <= pow2*f and f = ceil(e/pow2) */
      if (pow2)
        f = e <= 0 ? e / pow2 : (e - 1) / pow2 + 1; /* int overflow avoided */
      else
        {
          double d;

          d = mpfr_get_d3(op, 0, GMP_RNDN);
          d = ((double) e + (double) _mpfr_floor_log2 (ABS(d)))
            * __mp_bases[base].chars_per_bit_exactly;
          MPFR_ASSERTN(d >= MP_EXP_T_MIN);
          MPFR_ASSERTN(d <= MP_EXP_T_MAX);
          /* warning: (mp_exp_t) d rounds towards 0 */
          f = (mp_exp_t) d; /* f = floor(d) if d >= 0 and ceil(d) if d < 0 */
          if (f <= d)
            {
              MPFR_ASSERTN(f < MP_EXP_T_MAX);
              f++;
            }
        }

      mpfr_init (a);
      mpfr_init (b);
      mpz_init (bz);

      str0 = str;

      do
        {
          mp_prec_t prec, q;

          /* now the first n digits of the mantissa are obtained from
             rnd(op*base^(n-f)) */
          if (pow2)
            {
              MPFR_ASSERTN(n <= MPFR_INTPREC_MAX / pow2);
              prec = (mp_prec_t) n * pow2;
            }
          else
            {
              double d;

              d = (double) n / __mp_bases[base].chars_per_bit_exactly;
              MPFR_ASSERTN(d <= MPFR_INTPREC_MAX - 1);
              prec = (mp_prec_t) d + 1;
            }

          MPFR_ASSERTN(prec <= MPFR_INTPREC_MAX - ERR);
          /* one has to use at least q bits */
          q = ((prec + (ERR-1)) / BITS_PER_MP_LIMB + 1) * BITS_PER_MP_LIMB;
          mpfr_set_prec (a, q);
          mpfr_set_prec (b, q);

          while (1)
            {
              mp_exp_unsigned_t p;
              int div;

              if (f < 0)
                {
                  p = (mp_exp_unsigned_t) n - f;
                  MPFR_ASSERTN(p > n);
                  div = 0;
                }
              else if (n >= f)
                {
                  p = n - f;
                  div = 0;
                }
              else
                {
                  p = f - n;
                  div = 1;
                }

              if (pow2)
                {
                  MPFR_ASSERTN(p <= ULONG_MAX / pow2);
                  MPFR_ASSERTN(p <= __mpfr_emax / pow2);
                  if (div)
                    mpfr_div_2ui (b, op, pow2*p, rnd_mode);
                  else
                    mpfr_mul_2ui (b, op, pow2*p, rnd_mode);
                }
              else
                {
                  /* compute base^p with q bits */
                  mpfr_set_prec (b, q);
                  if (p == 0)
                    {
                      mpfr_set (b, op, rnd_mode);
                      mpfr_set_ui (a, 1, rnd_mode);
                    }
                  else
                    {
                      mp_rnd_t rnd1;

                      mpfr_set_prec (a, q);
                      if (div)
                        {
                          /* if div, we divide by base^p, so we have to invert
                             the rounding mode to compute base^p */
                          switch (rnd_mode)
                            {
                            case GMP_RNDN: rnd1 = GMP_RNDN; break;
                            case GMP_RNDZ: rnd1 = GMP_RNDU; break;
                            case GMP_RNDU: rnd1 = GMP_RNDZ; break;
                            case GMP_RNDD: rnd1 = GMP_RNDU; break;
                            default: MPFR_ASSERTN(0);
                            }
                        }
                      else
                        {
                          rnd1 = rnd_mode;
                        }
                      mpfr_ui_pow_ui (a, base, p, rnd1);
                      if (div)
                        {
                          mpfr_set_ui (b, 1, rnd_mode);
                          mpfr_div (a, b, a, rnd_mode);
                        }
                      /* now a is an approximation to 1/base^(f-n) */
                      mpfr_mul (b, op, a, rnd_mode);
                    }
                }

              if (neg)
                MPFR_CHANGE_SIGN(b); /* put b positive */

              if (prec <= (MPFR_INTPREC_MAX - BITS_PER_MP_LIMB) / 2 &&
                  q > 2 * prec + BITS_PER_MP_LIMB)
                {
                  /* if the intermediate precision exceeds twice that of the
                     input, a worst-case for the division cannot occur */
                  rnd_mode = GMP_RNDN;
                  break;
                }
              else if (pow2 ||
                       mpfr_can_round (b, q-ERR, rnd_mode, rnd_mode, prec))
                break;

              MPFR_ASSERTN(q <= MPFR_INTPREC_MAX - BITS_PER_MP_LIMB);
              q += BITS_PER_MP_LIMB;
            }

          {
            mp_rnd_t rnd = rnd_mode;

            if (neg)
              switch (rnd_mode)
                {
                case GMP_RNDU: rnd = GMP_RNDZ; break;
                case GMP_RNDD: rnd = GMP_RNDU; break;
                }

            mpfr_round_prec (b, rnd, MPFR_EXP(b));
          }

          prec = MPFR_EXP(b); /* may have changed due to rounding */

          {
            mp_size_t n, e;
            int sh;

            /* now the mantissa is the integer part of b */
            n = 1 + (prec - 1) / BITS_PER_MP_LIMB;
            _mpz_realloc (bz, n);
            sh = prec % BITS_PER_MP_LIMB;
            e = 1 + (MPFR_PREC(b) - 1) / BITS_PER_MP_LIMB;
            MPFR_ASSERTN(e >= n);
            e -= n;
            if (sh != 0)
              mpn_rshift (PTR(bz), MPFR_MANT(b) + e, n, BITS_PER_MP_LIMB - sh);
            else
              MPN_COPY(PTR(bz), MPFR_MANT(b) + e, n);
            bz->_mp_size = n;
          }

          /* computes the number of characters needed */
          /* n+1 may not be enough for 100000... */
          if (str0 == NULL)
            str0 = (*__gmp_allocate_func) (neg + n + 2);

          str = str0; /* restore initial value in case we had to restart */

          if (neg)
            *str++ = '-';

          mpz_get_str (str, base, bz); /* n digits of mantissa */
          len = strlen(str);
        }
      while (len > n && (f++, 1));

      if (len == n - 1)
        {
          len++;
          f--;
          str[n-1]='0';
          str[n]='\0';
        }

      *expptr = f;
      mpfr_clear (a);
      mpfr_clear (b);
      mpz_clear (bz);

      /* if the given string was null, ensure we return a block
         which is exactly strlen(str0)+1 bytes long (useful for
         __gmp_free_func and the C++ wrapper) */

      /* NOTE: len != n + 1 is always satisfied; either this condition
         is useless or there is a bug somewhere */
      if (str_is_null && len != n + 1)
        str0 = (*__gmp_reallocate_func) (str0, neg + n + 2, neg + len + 1);

      return str0;
    }
}
