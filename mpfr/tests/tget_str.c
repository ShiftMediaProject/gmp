/* Test file for mpfr_get_str.

Copyright 1999, 2001, 2002, 2003 Free Software Foundation, Inc.

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
#include <time.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"
#include "mpfr-test.h"

static void
check3 (double d, mp_rnd_t rnd, char *res)
{
  mpfr_t x;
  char *str;
  mp_exp_t e;

  mpfr_init2 (x, 53);
  mpfr_set_d (x, d, rnd);
  str = mpfr_get_str (NULL, &e, 10, 5, x, rnd);
  if (strcmp (str, res))
    {
      printf ("Error in mpfr_get_str for x=%1.20e\n", d);
      printf ("got %s instead of %s\n", str, res);
      exit (1);
    }
  mpfr_clear (x);
  (*__gmp_free_func) (str, strlen (str) + 1);
}

static void
check_small (void)
{
  mpfr_t x;
  char *s;
  mp_exp_t e;
  mp_prec_t p;

  mpfr_init (x);

  mpfr_set_prec (x, 20);
  mpfr_set_ui (x, 2, GMP_RNDN);
  mpfr_sub_one_ulp (x, GMP_RNDD);
  s = mpfr_get_str (NULL, &e, 4, 2, x, GMP_RNDU);
  if (strcmp (s, "20") || (e != 1))
    {
      printf ("Error in mpfr_get_str: 2- rounded up with 2 digits"
              " in base 4\n");
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  /* check n_digits=0 */
  mpfr_set_prec (x, 5);
  mpfr_set_ui (x, 17, GMP_RNDN);
  s = mpfr_get_str (NULL, &e, 3, 0, x, GMP_RNDN);
  (*__gmp_free_func) (s, strlen (s) + 1);
  s = mpfr_get_str (NULL, &e, 36, 0, x, GMP_RNDN);
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_set_prec (x, 64);
  mpfr_set_si (x, -1, GMP_RNDN);
  mpfr_div_2exp (x, x, 63, GMP_RNDN); /* x = -2^(-63) */
  mpfr_add_ui (x, x, 1, GMP_RNDN); /* x = 1 - 2^(-63) */
  mpfr_mul_2exp (x, x, 32, GMP_RNDN); /* x = 2^32 - 2^(-31) */
  s = mpfr_get_str (NULL, &e, 3, 21, x, GMP_RNDU);
  if (strcmp (s, "102002022201221111211") || (e != 21))
    {
      printf ("Error in mpfr_get_str: 2^32-2^(-31) rounded up with"
              " 21 digits in base 3\n");
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);
  s = mpfr_get_str (NULL, &e, 3, 20, x, GMP_RNDU);
  if (strcmp (s, "10200202220122111122") || (e != 21))
    {
      printf ("Error in mpfr_get_str: 2^32-2^(-31) rounded up with"
              " 20 digits in base 3\n");
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  for (p=4; p<=200; p++)
    {
      mpfr_set_prec (x, p);
      mpfr_set_d (x, 6.5, GMP_RNDN);

      s = mpfr_get_str (NULL, &e, 6, 2, x, GMP_RNDN);
      if (strcmp (s, "10") || (e != 2))
        {
          printf ("Error in mpfr_get_str: 6.5 rounded to nearest with"
                  " 2 digits in base 6\n");
          exit (1);
        }
      (*__gmp_free_func) (s, strlen (s) + 1);

      mpfr_add_one_ulp (x, GMP_RNDU);
      s = mpfr_get_str (NULL, &e, 6, 2, x, GMP_RNDN);
      if (strcmp (s, "11") || (e != 2))
        {
          printf ("Error in mpfr_get_str: 6.5+ rounded to nearest with"
                  " 2 digits in base 6\ngot %se%d instead of 11e2\n",
                  s, (int) e);
          exit (1);
        }
      (*__gmp_free_func) (s, strlen (s) + 1);

      mpfr_set_d (x, 6.5, GMP_RNDN);
      mpfr_sub_one_ulp (x, GMP_RNDU);
      s = mpfr_get_str (NULL, &e, 6, 2, x, GMP_RNDN);
      if (strcmp (s, "10") || (e != 2))
        {
          printf ("Error in mpfr_get_str: 6.5- rounded to nearest with"
                  " 2 digits in base 6\n");
          exit (1);
        }
      (*__gmp_free_func) (s, strlen (s) + 1);
    }

  mpfr_set_prec (x, 3);
  mpfr_set_ui (x, 7, GMP_RNDN);
  s = mpfr_get_str (NULL, &e, 2, 2, x, GMP_RNDU);
  if (strcmp (s, "10") || (e != 4))
    {
      printf ("Error in mpfr_get_str: 7 rounded up with 2 bits should"
              " give 0.10e3 instead of 0.%s*2^%d\n", s, (int) e);
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  /* problem found by Fabrice Rouillier */
  mpfr_set_prec (x, 63);
  mpfr_set_d (x, 5e14, GMP_RNDN);
  s = mpfr_get_str (NULL, &e, 10, 18, x, GMP_RNDU);
  (*__gmp_free_func) (s, strlen (s) + 1);

  /* bug found by Johan Vervloet */
  mpfr_set_prec (x, 6);
  mpfr_set_d (x, 688.0, GMP_RNDN);
  s = mpfr_get_str (NULL, &e, 2, 4, x, GMP_RNDU);
  if (strcmp (s, "1011") || (e != 10))
    {
      printf ("Error in mpfr_get_str: 688 printed up to 4 bits should"
              " give 1.011e9\ninstead of ");
      mpfr_out_str (stdout, 2, 4, x, GMP_RNDU);
      puts ("");
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_set_prec (x, 38);
  mpfr_set_str_binary (x, "1.0001110111110100011010100010010100110e-6");
  s = mpfr_get_str (NULL, &e, 8, 10, x, GMP_RNDU);
  if (strcmp (s, "1073721522") || (e != -1))
    {
      printf ("Error in mpfr_get_str (3): s=%s e=%d\n", s, (int) e);
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_set_prec (x, 53);
  mpfr_set_str_binary (x, "0.11010111011101100010000100010101110001000000010111001E454");
  s = mpfr_get_str (NULL, &e, 19, 12, x, GMP_RNDU);
  if (strcmp (s, "b1cgfa4gha0h") || (e != 107))
    {
      printf ("Error in mpfr_get_str (4): s=%s e=%d\n", s, (int) e);
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_clear (x);
}

/* bugs found by Alain Delplanque */
static void
check_large (void)
{
  mpfr_t x;
  char *s, s1[7];
  const char xm[] = { '1', '1', '9', '1', '3', '2', '9', '3', '7', '3',
                      '5', '8', '4', '4', '5', '4', '9', '0', '2', '9',
                      '6', '3', '4', '4', '6', '9', '9', '1', '9', '5',
                      '5', '7', '2', '0', '1', '7', '5', '2', '8', '6',
                      '1', '2', '5', '2', '5', '2', '7', '4', '0', '2',
                      '7', '9', '1', '1', '7', '4', '5', '6', '7', '5',
                      '9', '3', '1', '4', '2', '5', '5', '6', '6', '6',
                      '1', '6', '4', '3', '8', '1', '2', '8', '7', '6',
                      '2', '9', '2', '0', '8', '8', '9', '4', '3', '9',
                      '6', '2', '8', '4', '1', '1', '8', '1', '0', '6',
                      '2', '3', '7', '6', '3', '8', '1', '5', '1', '7',
                      '3', '4', '6', '1', '2', '4', '0', '1', '3', '0',
                      '8', '4', '1', '3', '9', '3', '2', '0', '1', '6',
                      '3', '6', '7', '1', '5', '1', '7', '5', '0', '1',
                      '9', '8', '4', '0', '8', '2', '7', '9', '1', '3',
                      '2', '2', '8', '3', '4', '1', '6', '2', '3', '9',
                      '6', '2', '0', '7', '3', '5', '5', '5', '3', '4',
                      '2', '1', '7', '0', '9', '7', '6', '2', '1', '0',
                      '3', '3', '5', '4', '7', '6', '0', '9', '7', '6',
                      '9', '3', '5', '1', '7', '8', '6', '8', '8', '2',
                      '8', '1', '4', '3', '7', '4', '3', '3', '2', '4',
                      '1', '5', '4', '7', '8', '1', '1', '4', '2', '1',
                      '2', '4', '2', '7', '6', '5', '9', '5', '4', '5',
                      '2', '6', '7', '3', '0', '3', '4', '0', '6', '9',
                      '1', '8', '9', '9', '9', '8', '0', '5', '7', '0',
                      '9', '3', '8', '7', '6', '2', '4', '6', '1', '6',
                      '7', '2', '0', '3', '5', '9', '3', '5', '8', '8',
                      '9', '7', '7', '9', '2', '7', '0', '8', '1', '6',
                      '8', '7', '4', '8', '5', '3', '0', '8', '4', '3',
                      '5', '6', '5', '1', '6', '6', '0', '9', '7', '9',
                      '8', '9', '2', '7', '2', '6', '8', '5', '9', '4',
                      '5', '8', '1', '3', '7', '2', '9', '3', '8', '3',
                      '7', '9', '1', '7', '9', '9', '7', '7', '2', '8',
                      '4', '6', '5', '5', '7', '3', '3', '8', '3', '6',
                      '6', '9', '7', '1', '4', '3', '3', '7', '1', '4',
                      '9', '4', '1', '2', '4', '9', '5', '1', '4', '7',
                      '2', '6', '4', '4', '8', '0', '6', '2', '6', '0',
                      '6', '9', '8', '1', '1', '7', '9', '9', '3', '9',
                      '3', '8', '4', '7', '3', '1', '9', '0', '2', '3',
                      '5', '3', '5', '4', '2', '1', '1', '7', '6', '7',
                      '4', '3', '2', '2', '0', '6', '5', '9', '9', '3',
                      '2', '6', '7', '1', '2', '0', '0', '3', '7', '3',
                      '8', '7', '4', '3', '3', '3', '3', '3', '2', '3',
                      '8', '2', '8', '6', '3', '1', '5', '5', '2', '2',
                      '5', '9', '3', '3', '7', '0', '6', '2', '8', '1',
                      '0', '3', '6', '7', '6', '9', '6', '5', '9', '0',
                      '6', '6', '6', '3', '6', '9', '9', '3', '8', '7',
                      '6', '5', '4', '5', '3', '5', '9', '4', '0', '0',
                      '7', '5', '8', '5', '4', '1', '4', '3', '1', '5',
                      '7', '6', '6', '3', '4', '4', '5', '0', '8', '7',
                      '5', '7', '5', '0', '1', '0', '1', '8', '4', '7',
                      '3', '1', '9', '9', '2', '7', '1', '1', '1', '2',
                      '3', '9', '9', '6', '5', '9', '2', '3', '2', '8',
                      '1', '5', '5', '1', '2', '6', '4', '9', '6', '6',
                      '4', '5', '1', '1', '6', '0', '0', '3', '2', '8',
                      '4', '8', '7', '1', '4', '9', '6', '8', '1', '6',
                      '5', '9', '8', '3', '4', '2', '9', '7', '0', '1',
                      '9', '2', '6', '6', '9', '1', '3', '5', '9', '3',
                      '2', '9', '6', '2', '3', '0', '6', '0', '1', '1',
                      '6', '5', '1', '7', '9', '0', '7', '5', '8', '6',
                      '8', '4', '2', '1', '0', '3', '8', '6', '6', '4',
                      '4', '9', '9', '7', '5', '8', '1', '7', '5', '7',
                      '9', '6', '6', '8', '8', '5', '8', '6', '7', '4',
                      '0', '7', '2', '0', '2', '9', '9', '4', '4', '1',
                      '9', '5', '8', '6', '5', '0', '6', '7', '4', '2',
                      '7', '3', '2', '3', '2', '7', '0', '2', '1', '3',
                      '0', '5', '9', '0', '3', '9', '1', '4', '5', '3',
                      '7', '2', '7', '0', '8', '5', '5', '4', '6', '1',
                      '1', '0', '0', '9', '2', '0', '4', '1', '6', '6',
                      '4', '6', '9', '1', '3', '2', '8', '5', '0', '3',
                      '3', '8', '9', '8', '7', '8', '5', '9', '5', '5',
                      '9', '1', '9', '3', '6', '5', '4', '1', '7', '4',
                      '0', '2', '4', '7', '2', '9', '7', '1', '2', '4',
                      '5', '8', '1', '4', '4', '6', '1', '8', '5', '8',
                      '7', '6', '9', '7', '2', '1', '2', '0', '8', '9',
                      '5', '9', '5', '5', '3', '8', '1', '2', '5', '4',
                      '3', '0', '7', '6', '5', '1', '7', '8', '2', '0',
                      '0', '7', '6', '7', '4', '8', '1', '0', '6', '3',
                      '2', '3', '0', '5', '2', '5', '0', '1', '1', '4',
                      '3', '8', '4', '5', '2', '3', '9', '5', '0', '9',
                      '8', '2', '6', '4', '7', '4', '8', '0', '1', '1',
                      '7', '1', '5', '4', '9', '0', '9', '2', '2', '3',
                      '8', '1', '6', '9', '0', '4', '6', '4', '5', '4',
                      '6', '3', '8', '7', '3', '6', '1', '7', '2', '3',
                      '4', '5', '5', '2', '0', '2', '5', '8', '1', '4',
                      '9', '3', '0', '7', '4', '1', '6', '8', '7', '8',
                      '2', '6', '2', '5', '1', '0', '7', '4', '7', '3',
                      '6', '6', '4', '5', '6', '6', '6', '6', '8', '5',
                      '1', '3', '5', '7', '1', '6', '2', '0', '9', '2',
                      '3', '2', '6', '0', '7', '9', '8', '1', '6', '2',
                      '0', '3', '8', '8', '0', '2', '8', '7', '7', '5',
                      '9', '3', '1', '0', '6', '7', '5', '7', '3', '1',
                      '2', '7', '7', '2', '0', '0', '4', '1', '2', '8',
                      '2', '0', '8', '4', '0', '5', '0', '5', '0', '1',
                      '9', '3', '3', '6', '3', '6', '9', '6', '2', '8',
                      '2', '9', '7', '5', '3', '8', '8', '9', '1', '1',
                      '4', '5', '7', '7', '5', '6', '0', '2', '7', '9',
                      '7', '2', '1', '7', '4', '3', '0', '3', '6', '7',
                      '3', '7', '2', '2', '7', '5', '6', '2', '3', '1',
                      '2', '1', '3', '1', '4', '2', '6', '9', '2', '3',
                      '\0' };
  mp_exp_t e;

  mpfr_init2 (x, 3322);
  mpfr_set_str (x, xm, 10, GMP_RNDN);
  mpfr_div_2exp (x, x, 4343, GMP_RNDN);
  s = mpfr_get_str (NULL, &e, 10, 1000, x, GMP_RNDN);
  if (s[999] != '1') /* s must be 5.04383...689071e-309 */
    {
      printf ("Error in check_large: expected '689071', got '%s'\n",
              s + 994);
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_mul_2exp (x, x, 4343, GMP_RNDN);
  s = mpfr_get_str (NULL, &e, 10, 2, x, GMP_RNDN);
  if (strcmp (s, "12") || (e != 1000))
    {
      printf ("Error in check_large: expected 0.12e1000\n");
      printf ("got %se%d\n", s, (int) e);
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_set_nan (x);
  s = mpfr_get_str (NULL, &e, 10, 1000, x, GMP_RNDN);
  if (strcmp (s, "@NaN@"))
    {
      printf ("Error for NaN\n");
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_get_str (s1, &e, 10, 1000, x, GMP_RNDN);

  mpfr_set_inf (x, 1);
  s = mpfr_get_str (NULL, &e, 10, 1000, x, GMP_RNDN);
  if (strcmp (s, "@Inf@"))
    {
      printf ("Error for Inf\n");
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_get_str (s1, &e, 10, 1000, x, GMP_RNDN);

  mpfr_set_inf (x, -1);
  s = mpfr_get_str (NULL, &e, 10, 1000, x, GMP_RNDN);
  if (strcmp (s, "-@Inf@"))
    {
      printf ("Error for -Inf\n");
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);

  mpfr_get_str (s1, &e, 10, 1000, x, GMP_RNDN);

  mpfr_set_ui (x, 0, GMP_RNDN);
  s = mpfr_get_str (NULL, &e, 10, 2, x, GMP_RNDN);
  if (e != 0 || strcmp (s, "00"))
    {
      printf ("Error for 0.0\n");
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);
  mpfr_get_str (s1, &e, 10, 2, x, GMP_RNDN);

  mpfr_neg (x, x, GMP_RNDN); /* -0.0 */
  s = mpfr_get_str (NULL, &e, 10, 2, x, GMP_RNDN);
  if (e != 0 || strcmp (s, "-00"))
    {
      printf ("Error for -0.0\ngot %se%d\n", s, (int) e);
      exit (1);
    }
  (*__gmp_free_func) (s, strlen (s) + 1);
  mpfr_get_str (s1, &e, 10, 2, x, GMP_RNDN);

  mpfr_clear (x);
}

#define MAX_DIGITS 100

static void
check_special (int b, mp_prec_t p)
{
  mpfr_t x;
  int i, j;
  char s[MAX_DIGITS + 2], s2[MAX_DIGITS + 2], c;
  mp_exp_t e;
  mp_rnd_t r;
  size_t m;

  s2[0] = '1';
  for (i=1; i<MAX_DIGITS+2; i++)
    s2[i] = '0';

  mpfr_init2 (x, p);
  mpfr_set_ui (x, 1, GMP_RNDN);
  for (i=1; i<100 && mpfr_mul_ui (x, x, b, GMP_RNDN) == 0; i++)
    {
      /* x = b^i (exact) */
      for (r=0; r<4; r++)
        for (m=(i < 3) ? 2 : i-1; m<=i+1; m++)
          {
            mpfr_get_str (s, &e, b, m, x, r);
            /* s should be 1 followed by (m-1) zeros, and e should be i+1 */
            if ((e != i+1) || strncmp (s, s2, m) != 0)
              {
                printf ("Error in mpfr_get_str for %u^%u\n", b, i);
                exit (1);
              }
          }
      if (mpfr_sub_ui (x, x, 1, GMP_RNDN) != 0)
        break;
      /* now x = b^i-1 (exact) */
      for (r=0; r<4; r++)
        if (i >= 2)
          {
            mpfr_get_str (s, &e, b, i, x, r);
            /* should be i times (b-1) */
            c = (b <= 10) ? '0' + b - 1 : 'a' + (b - 11);
            for (j=0; (j < i) && (s[j] == c); j++);
            if ((j < i) || (e != i))
              {
                printf ("Error in mpfr_get_str for %u^%u-1\n", b, i);
                printf ("got 0.%s*2^%d\n", s, (int) e);
                exit (1);
              }
          }
      if (i >= 3)
        {
          mpfr_get_str (s, &e, b, i - 1, x, GMP_RNDU);
          /* should be b^i */
          if ((e != i+1) || strncmp (s, s2, i - 1) != 0)
            {
              printf ("Error in mpfr_get_str for %u^%u-1\n", b, i);
              printf ("got 0.%s*2^%d\n", s, (int) e);
              exit (1);
            }
        }

      mpfr_add_ui (x, x, 1, GMP_RNDN);
    }
  mpfr_clear (x);
}

int
main (int argc, char *argv[])
{
  int b;
  mpfr_t x;
  mp_rnd_t r;
  char s[MAX_DIGITS + 2];
  mp_exp_t e, f;
  size_t m;
  mp_prec_t p;

  tests_start_mpfr ();

  check_small ();

  for (p=2; p<=MAX_DIGITS; p++)
    for (b=2; b<=36; b++)
      check_special (b, p);

  mpfr_init2 (x, MAX_DIGITS);
  for (m=2; m<=MAX_DIGITS; m++)
    {
      mpfr_random (x);
      for (e=-10; e<=10; e++)
        {
          mpfr_set_exp (x, (e == -10) ? mpfr_get_emin () :
                        ((e == 10) ? mpfr_get_emax () : e));
          for (b=2; b<=36; b++)
            for (r=0; r<4; r++)
              mpfr_get_str (s, &f, b, m, x, r);
        }
    }
  mpfr_clear (x);

  check_large ();
  check3 (4.059650008e-83, GMP_RNDN, "40597");
  check3 (-6.606499965302424244461355e233, GMP_RNDN, "-66065");
  check3 (-7.4, GMP_RNDN, "-74000");
  check3 (0.997, GMP_RNDN, "99700");
  check3 (-4.53063926135729747564e-308, GMP_RNDN, "-45306");
  check3 (2.14478198760196000000e+16, GMP_RNDN, "21448");
  check3 (7.02293374921793516813e-84, GMP_RNDN, "70229");

  check3 (-6.7274500420134077e-87, GMP_RNDN, "-67275");
  check3 (-6.7274500420134077e-87, GMP_RNDZ, "-67274");
  check3 (-6.7274500420134077e-87, GMP_RNDU, "-67274");
  check3 (-6.7274500420134077e-87, GMP_RNDD, "-67275");

  check3 (6.7274500420134077e-87, GMP_RNDN, "67275");
  check3 (6.7274500420134077e-87, GMP_RNDZ, "67274");
  check3 (6.7274500420134077e-87, GMP_RNDU, "67275");
  check3 (6.7274500420134077e-87, GMP_RNDD, "67274");

  tests_end_mpfr ();
  return 0;
}
