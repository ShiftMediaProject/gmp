/* Test mpn_get_d.

Copyright 2002, 2003 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"


/* BITS_PER_DOUBLE_MANT is the number of bits in the mantissa of a "double".
   Exact when we know the format of a double, or an underestimate if we
   don't.  */
#if _GMP_IEEE_FLOATS
#define BITS_PER_DOUBLE_MANT  53
#endif
#if HAVE_DOUBLE_CRAY_CFP
#define BITS_PER_DOUBLE_MANT  48
#endif
#if HAVE_DOUBLE_VAX_D
#define BITS_PER_DOUBLE_MANT  55
#endif
#if HAVE_DOUBLE_VAX_G
#define BITS_PER_DOUBLE_MANT  53
#endif
#ifndef BITS_PER_DOUBLE_MANT
#define BITS_PER_DOUBLE_MANT  ((8 * sizeof (double)) / 2)
#endif


/* Exercise various 2^n values, with various exponents and positive and
   negative.  */
void
check_onebit (void)
{
  static const int bit_table[] = {
    0, 1, 2, 3,
    GMP_NUMB_BITS - 2, GMP_NUMB_BITS - 1,
    GMP_NUMB_BITS,
    GMP_NUMB_BITS + 1, GMP_NUMB_BITS + 2,
    2 * GMP_NUMB_BITS - 2, 2 * GMP_NUMB_BITS - 1,
    2 * GMP_NUMB_BITS,
    2 * GMP_NUMB_BITS + 1, 2 * GMP_NUMB_BITS + 2,
    3 * GMP_NUMB_BITS - 2, 3 * GMP_NUMB_BITS - 1,
    3 * GMP_NUMB_BITS,
    3 * GMP_NUMB_BITS + 1, 3 * GMP_NUMB_BITS + 2,
    4 * GMP_NUMB_BITS - 2, 4 * GMP_NUMB_BITS - 1,
    4 * GMP_NUMB_BITS,
    4 * GMP_NUMB_BITS + 1, 4 * GMP_NUMB_BITS + 2,
    5 * GMP_NUMB_BITS - 2, 5 * GMP_NUMB_BITS - 1,
    5 * GMP_NUMB_BITS,
    5 * GMP_NUMB_BITS + 1, 5 * GMP_NUMB_BITS + 2,
    6 * GMP_NUMB_BITS - 2, 6 * GMP_NUMB_BITS - 1,
    6 * GMP_NUMB_BITS,
    6 * GMP_NUMB_BITS + 1, 6 * GMP_NUMB_BITS + 2,
  };
  static const int exp_table[] = {
    0, -100, -10, -1, 1, 10, 100,
  };

  /* FIXME: It'd be better to base this on the float format. */
#ifdef __vax
  int     limit = 127;  /* vax fp numbers have limited range */
#else
  int     limit = 511;
#endif

  int        bit_i, exp_i, i;
  double     got, want;
  mp_size_t  nsize, sign;
  long       bit, exp, want_bit;
  mp_limb_t  np[20];

  for (bit_i = 0; bit_i < numberof (bit_table); bit_i++)
    {
      bit = bit_table[bit_i];

      nsize = BITS_TO_LIMBS (bit+1);
      refmpn_zero (np, nsize);
      np[bit/GMP_NUMB_BITS] = CNST_LIMB(1) << (bit % GMP_NUMB_BITS);

      for (exp_i = 0; exp_i < numberof (exp_table); exp_i++)
        {
          exp = exp_table[exp_i];

          want_bit = bit + exp;
          if (want_bit > limit || want_bit < -limit)
            continue;

          want = 1.0;
          for (i = 0; i < want_bit; i++)
            want *= 2.0;
          for (i = 0; i > want_bit; i--)
            want *= 0.5;

          for (sign = 0; sign >= -1; sign--, want = -want)
            {
              got = mpn_get_d (np, nsize, sign, exp);
              if (got != want)
                {
                  printf    ("mpn_get_d wrong on 2^n\n");
                  printf    ("   bit      %ld\n", bit);
                  printf    ("   exp      %ld\n", exp);
                  printf    ("   want_bit %ld\n", want_bit);
                  printf    ("   sign     %ld\n", (long) sign);
                  mpn_trace ("   n        ", np, nsize);
                  printf    ("   nsize    %ld\n", (long) nsize);
                  d_trace   ("   want     ", want);
                  d_trace   ("   got      ", got);
                  abort();
                }
            }
        }
    }
}

/* Exercise values 2^n+1, while such a value fits the mantissa of a
   double.  */
void
check_twobit (void)
{
  int        i;
  double     got, want;
  mp_size_t  nsize, sign;
  mp_limb_t  np[BITS_TO_LIMBS (BITS_PER_DOUBLE_MANT)];

  want = 3.0;
  for (i = 1; i < BITS_PER_DOUBLE_MANT; i++)
    {
      nsize = BITS_TO_LIMBS (i+1);
      refmpn_zero (np, nsize);
      np[i/GMP_NUMB_BITS] = CNST_LIMB(1) << (i % GMP_NUMB_BITS);
      np[0] |= 1;

      for (sign = 0; sign >= -1; sign--)
        {
          got = mpn_get_d (np, nsize, sign, 0);
          if (got != want)
            {
              printf    ("mpn_get_d wrong on 2^%d + 1\n", i);
              printf    ("   sign     %ld\n", (long) sign);
              mpn_trace ("   n        ", np, nsize);
              printf    ("   nsize    %ld\n", (long) nsize);
              d_trace   ("   want     ", want);
              d_trace   ("   got      ", got);
              abort();
            }
          want = -want;
        }

      want = 2.0 * want - 1.0;
    }
}

/* Expect large values to result in +/-inf, on systems with such doubles. */
void
check_inf (void)
{
#if _GMP_IEEE_FLOATS
  static const long exp_table[] = {
    999999L, LONG_MAX,
  };
  static const mp_limb_t  np[4] = { 1, 1, 1, 1 };
  long       exp;
  mp_size_t  nsize, sign, got_sign;
  double     got;
  int        exp_i;

  for (nsize = 1; nsize <= numberof (np); nsize++)
    {
      for (exp_i = 0; exp_i < numberof (exp_table); exp_i++)
        {
          exp = exp_table[exp_i];

          for (sign = 0; sign >= -1; sign--)
            {
              got = mpn_get_d (np, nsize, sign, exp);
              got_sign = (got >= 0 ? 0 : -1);
              if (! tests_isinf (got))
                {
                  printf  ("mpn_get_d wrong, didn't get infinity\n");
                bad:
                  printf  ("  nsize  %ld\n", (long) nsize);
                  printf  ("  exp    %ld\n", exp);
                  d_trace ("  got    ", got);
                  abort ();
                }
              if (got_sign != sign)
                {
                  printf  ("mpn_get_d wrong sign on infinity\n");
                  goto bad;
                }
            }
        }
    }
#endif
}

/* ARM gcc 2.95.4 was seen generating bad code for ulong->double
   conversions, resulting in for instance 0x81c25113 incorrectly converted.
   This test exercises that value, to see mpn_get_d has avoided the
   problem.  */
void
check_0x81c25113 (void)
{
#if GMP_NUMB_BITS >= 32
  double     want = 2176995603.0;
  double     got;
  mp_limb_t  np[4];
  mp_size_t  nsize;
  long       exp;

  for (nsize = 1; nsize <= numberof (np); nsize++)
    {
      refmpn_zero (np, nsize-1);
      np[nsize-1] = CNST_LIMB(0x81c25113);
      exp = - (nsize-1) * GMP_NUMB_BITS;
      got = mpn_get_d (np, nsize, (mp_size_t) 0, exp);
      if (got != want)
        {
          printf  ("mpn_get_d wrong on 2176995603 (0x81c25113)\n");
          printf  ("  nsize  %ld\n", (long) nsize);
          printf  ("  exp    %ld\n", exp);
          d_trace ("  got    ", got);
          d_trace ("  want   ", want);
          abort ();
        }
    }
#endif
}

int
main (void)
{
  tests_start ();

  check_onebit ();
  check_twobit ();
  check_inf ();
  check_0x81c25113 ();

  tests_end ();
  exit (0);
}
