/* mpfr_set_str -- set a floating-point number from a string

Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include <ctype.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "log_b2.h"

static double __gmpfr_ceil _PROTO((double));
static int digit_value_in_base _PROTO ((int, int));

static double
__gmpfr_ceil (double x)
{
  double y = (double) (long int) x;
  return ((y < x) ? y + 1.0 : y);
}

/* Compatible with any locale, but one still assumes that 'a', 'b', 'c',
   ..., 'z', and 'A', 'B', 'C', ..., 'Z' are consecutive values (like
   in any ASCII-based character set). */
static int
digit_value_in_base (int c, int base)
{
  int digit;

  if (isdigit (c))
    digit = c - '0';
  else if (c >= 'a' && c <= 'z')
    digit = c - 'a' + 10;
  else if (c >= 'A' && c <= 'Z')
    digit = c - 'A' + 10;
  else
    return -1;

  if (digit < base)
    return digit;
  return -1;
}

/* Set x to the value of str, in base base, rounded in direction rnd.
   return value is: 0 if the entire string up to the final required '\0'
                     is a valid number, otherwise -1 (compatibility with
                     mpf_set_str)
 */
int
mpfr_set_str (mpfr_t x, const char *str, int base, mp_rnd_t rnd)
{
  mp_limb_t *y;                /* current value of x */
  mp_exp_t exp_y;              /* such as x = y*base^exp_y */
  mp_limb_t *z, *result;       /* temporary variables */
  mp_exp_t exp_z;              /* exponent of z */
  size_t n;                    /* working precision in limbs */
  size_t prec_s;               /* precision of str */
  mp_exp_t pr;                 /* needed precision requise for str */
  mp_exp_t exp_s = 0;          /* exponent in base 'base', normalized for a
                                  mantissa 0.xxx...xxx */
  mp_exp_t prec_x;             /* working precision for x */
  char *str1;                  /* copy of str, should not be modified */
  size_t size_str1;            /* number of characters in str1 */
  char *mant_s;                /* pointer in str1 */
  int negative = 0;            /* 1 if str<=0, 0 otherwise */
  size_t i, j;
  long err = 0;
  int exact;                   /* is the computation exact */
  int point = 0;               /* position in str */
  int cboucle = 1;
  int c, res = 0;
  TMP_DECL(marker);

  if (base < 2 || base > 36)
    return -1;

  /* be careful that 'nan' is a valid number in base >= 24,
     since n=23, a=10, n=23 */
  if (((base < 24) ? strncasecmp (str, "NaN", 3) : strncmp (str, "NaN", 3))
      == 0)
    {
      MPFR_SET_NAN(x);
      /* MPFR_RET_NAN not used as the return value isn't a ternary value */
      __gmpfr_flags |= MPFR_FLAGS_NAN;
      return 0;
    }

  /* deals with the sign */
  negative = (str[0] == '-');
  if ((str[0] == '-') || (str[0] == '+'))
    str ++;

  /* be careful that 'inf' is a valid number in base >= 24,
     since i=18, n=23, f=15 */
  if (((base < 24) ? strncasecmp (str, "Inf", 3) : strncmp (str, "Inf", 3))
      == 0)
    {
      MPFR_CLEAR_NAN (x);
      MPFR_SET_INF (x);
      (negative) ? MPFR_SET_NEG (x) : MPFR_SET_POS (x);
      return 0;
    }

  /* allocate str1 to store value of characters in str.
     FIXME: we could use str directly here if mpn_set_str would deal
     with real characters. */
  size_str1 = strlen (str);
  str1 = (*__gmp_allocate_func) (size_str1 * sizeof (char));
  /* the str1 pointer should not be modified */

  /* checks if the input string is valid, and converts characters into
     their value */
  for (mant_s = str1; *str != 0; str++)
    {
      if (((base < 10) && (*str == 'e')) || (*str == 'E') ||
	  (*str == '@'))
	{
          char *endptr[1];
	  /* the exponent digits are kept in ASCII */
          exp_s += (mp_exp_t) strtol (str + 1, endptr, 10);
          if (**endptr != '\0')
            {
              res = -1; /* invalid input: garbage after exponent */
              goto end; 
            }
	  break;
	}
      else if (*str == '.')
	{	
	  point = 1;
	  continue;    /* go to next character */
	}
      else if ((c = digit_value_in_base (*str, base)) != -1)
        *mant_s = c; /* valid character */
      else 
	{
          res = -1; /* invalid input */
          goto end;
	}
      if (!point) 
	exp_s ++;
      mant_s ++;
    }
  prec_s = mant_s - str1; /* number of characters in the mantissa */

  mant_s = str1;
  /* remove 0's at the beginning and end of mant_s[0..prec_s-1] */
  for (; (prec_s > 0) && (*mant_s == 0); mant_s++, prec_s--, exp_s--);
  for (; (prec_s > 0) && (mant_s[prec_s - 1] == 0); prec_s--);

  /* test if x = 0 */
  if (prec_s == 0)
    {
      MPFR_CLEAR_FLAGS (x);
      MPFR_SET_ZERO (x);
      res = 0;
      goto end;
    }

  /* now we have str = 0.mant_s[0]...mant_s[prec_s-1]*base^exp_s */

  /* determine the minimal precision for the computation */
  prec_x = MPFR_PREC(x) + (mp_exp_t) __gmpfr_ceil_log2 ((double) MPFR_PREC(x));

  /* compute y as long as rounding is not possible */
  for (;;cboucle ++)
    {
      TMP_MARK(marker);

      /* number of limbs needed to compute x */
      n = (prec_x - 1) / BITS_PER_MP_LIMB + 1;

      /* initialize y to the value of 0.mant_s[0]...mant_s[pr-1] */
      y = TMP_ALLOC ((2 * n + 1) * sizeof (mp_limb_t));
      y += n;
      /* remember that y - n is allocated for n limbs */

      /* required precision for str */
      pr = (mp_exp_t) __gmpfr_ceil 
	(((double) (n * BITS_PER_MP_LIMB) - 1.0) * log_b2[base-2]) + 1;

      /* check is there are enough digits in str */
      if (pr >= prec_s)
	pr = prec_s;

      /* convert str into binary */
      i = mpn_set_str (y, mant_s, pr, base);

      /* normalize y */
      count_leading_zeros (j, *(y + i - 1));
      if ((exact = (i <= n)))         /* shift y to the left
				       in that case y shoud be exact */
	{
	  /* shift {y, i} for j bits to the left */
	  if (j != 0)
	    mpn_lshift (y, y, i, j);
	  /* shift {y, i} for (n-i) limbs to the left */
	  if (i != n)
	    {
	      MPN_COPY_DECR (y + n - i, y, i);
	      MPN_ZERO (y, n - i);
	    }
	  /* for each bit shift decrease exponent of y */
	  exp_y = - ((n - i) * BITS_PER_MP_LIMB + j);
	}
      else                          /* shift y for the right */
	{
	  /* shift {y, i} for (BITS_PER_MP_LIMB - j) bits to the right */
	  mpn_rshift (y, y, i, BITS_PER_MP_LIMB - j);
	  /* for each bit shift increase exponent of y */
	  exp_y = BITS_PER_MP_LIMB - j;
	}

      /* compute base^(exp_s-pr) on n limbs */

      if (IS_POW2 (base))
	{
	  int pow2;

	  count_leading_zeros (pow2, (mp_limb_t) base);
	  pow2 = BITS_PER_MP_LIMB - pow2 - 1; /* base = 2^pow2 */
	  
	  exp_y = exp_y + pow2 * (exp_s - (mp_exp_t) pr);

	  result = y - n;
	}
      /* case exp_s > pr */
      else if (exp_s > (mp_exp_t) pr)
	{
	  result = TMP_ALLOC ((2 * n + 1) * sizeof (mp_limb_t));

	  /* (z, exp_z) = base^(exp_s-pr), then result = y*z */
	  /* z is allocated at y - n */
	  z = y - n;
	  err = mpn_exp (z, &exp_z, base, exp_s - (mp_exp_t) pr, n);
	  exact = (exact && (err == -1));

	  /* multiply (y = 0.mant_s[0]...mant_s[pr-1])_base by base^(exp_s-g) */
	  mpn_mul_n (result, y, z, n);

	  /* compute the error on the product */
          if (err == -1)
	    err = 0;
          err ++;

	  /* compute the exponent of y */
	  exp_y += exp_z + n * BITS_PER_MP_LIMB;
	  
	  /* normalize result */
	  if ((result[2 * n - 1] & MPFR_LIMB_HIGHBIT) == 0)
	    {
	      mpn_lshift (result + n - 1, result + n - 1, n + 1, 1);
	      exp_y --;
	    }

	  exact = (exact && (mpn_scan1 (result, 0) >= n * BITS_PER_MP_LIMB));
	}

      /* cass exp_s < pr */
      else if (exp_s < (mp_exp_t) pr)
	{
	  result = TMP_ALLOC ((2 * n + 1) * sizeof (mp_limb_t));

	  /* set y to y * K^n */
	  y = y - n;  /* we have allocated n limbs at y - n */
	  MPN_ZERO (y, n);

	  /* (z, exp_z) = base^(exp_s-pr) */
	  z = TMP_ALLOC (n * sizeof (mp_limb_t));
	  err = mpn_exp (z, &exp_z, base, pr - exp_s, n);
	  exact = (exact && (err == -1));

	  if (err == -1)
	    err = 0;

	  /* compute y / z */
	  /* result will be put into result + n, and remainder into result */
	  mpn_tdiv_qr (result + n, result, (mp_size_t) 0, y, 2*n, z, n);

	  exp_y = exp_y - exp_z - n * BITS_PER_MP_LIMB;

	  err += 2;

	  exact = (exact && (mpn_popcount (result, n) == 0));

	  /* normalize result */
	  if (result[2 * n] == 1)
	    {
	      exact = (exact && ((result[n] & 1) == 0));
	      mpn_rshift (result + n, result + n, n + 1, 1);
	      exp_y ++;
	    }
	}
      else
	{
	  /* cass exp_s = pr : base^(exp_s-pr) = 1
	     nothing to compute, but we must put y into result and free it */
	  result = y - n;
	}

      /* test if rounding is possible, and if so exit the loop */
      if (exact || mpfr_can_round_raw (result + n, n, (negative) ? -1 : 1,
			       n * BITS_PER_MP_LIMB - err - 1, GMP_RNDN, rnd, MPFR_PREC(x)))
        break;

      /* update for next loop */
      if (cboucle == 2)
	prec_x = (mp_exp_t)
	  __gmpfr_ceil ((double) prec_s / log_b2[base - 2]) + 3;
      else
	{
	  mp_exp_t ajout = __gmpfr_ceil_log2 ((double) prec_x);
	  prec_x += (ajout < BITS_PER_MP_LIMB) ? BITS_PER_MP_LIMB : ajout;
	}

      TMP_FREE(marker);
    }

  /* round y */

  if (mpfr_round_raw_generic (MPFR_MANT(x), result + n, n * BITS_PER_MP_LIMB,
			      negative, MPFR_PREC(x), rnd, NULL, (int) 0))
    {
      /* overflaw when rounding y */
      MPFR_MANT(x)[MPFR_ESIZE(x) - 1] 
	= MPFR_LIMB_HIGHBIT;
      exp_y ++;
    }

  TMP_FREE(marker);

  MPFR_CLEAR_FLAGS(x);
  if (negative)
    MPFR_SET_NEG(x);
  else
    MPFR_SET_POS(x);
  MPFR_EXP(x) = exp_y + n * BITS_PER_MP_LIMB;

 end:
  (*__gmp_free_func) (str1, size_str1 * sizeof (char));

  return res;
}



