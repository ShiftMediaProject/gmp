/* Test expression evaluation (print nothing and exit 0 if successful).

Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <stdlib.h>

#include "gmp.h"
#include "gmp-impl.h"
#include "tests.h"
#include "expr-impl.h"

#if HAVE_MPFR
extern mpfr_t __mpfr_const_pi;
extern int __mpfr_const_pi_prec;
extern mpfr_t __mpfr_const_log2;
extern int __mpfr_const_log2_prec;
#endif


int  option_trace = 0;


struct data_t {
  int         base;
  const char  *expr;
  const char  *want;
};


/* These data_xxx[] arrays are tables to be tested with one or more of the
   mp?_t types.  z=mpz_t, q=mpz_t, f=mpf_t, r=mpfr_t.  */

struct data_t  data_zqfr[] = {

  /* various deliberately wrong expressions */
  { 0, "", NULL },
  { 0, "1+", NULL },
  { 0, "+2", NULL },
  { 0, "1,2", NULL },
  { 0, "foo(1,2)", NULL },
  { 0, "1+foo", NULL },
  { 10, "0fff", NULL },
  { 0, "!", NULL },
  { 0, "10!", NULL },
  { 0, "-10!", NULL },
  { 0, "gcd((4,6))", NULL },
  { 0, "()", NULL },
  { 0, "fac(2**1000)", NULL },
  { 0, "$", NULL },
  { 0, "$-", NULL },

  /* some basics */
  { 10, "123", "123" },
  { 10, "-123", "-123" },
  { 10, "1+2", "3" },
  { 10, "1+2+3", "6" },
  { 10, "1+2*3", "7" },
  { 10, "3*2+1", "7" },
  { 10, "$a", "55" },
  { 10, "b", "99" },
  { 16, "b", "11" },
  { 10, "4**3 * 2 + 1", "129" },
  { 10, "1<2", "1" },
  { 10, "1>2", "0" },

  { 10, "(123)", "123" },

  { 10, "sgn(-123)", "-1" },
  { 10, "5-7", "-2" },

  { 0, "cmp(0,0)", "0" },
  { 0, "cmp(1,0)", "1" },
  { 0, "cmp(0,1)", "-1" },
  { 0, "cmp(-1,0)", "-1" },
  { 0, "cmp(0,-1)", "1" },

  { 10, "0 ? 123 : 456", "456" },
  { 10, "1 ? 4+5 : 6+7", "9" },

  { 10, "(123)", "123" },
  { 10, "(2+3)", "5" },
  { 10, "(4+5)*(5+6)", "99" },

  { 0, "1 << 16", "65536" },
  { 0, "256 >> 4", "16" },
  { 0, "-256 >> 4", "-16" },

  { 0, "!1", "0" },
  { 0, "!9", "0" },
  { 0, "!0", "1" },

  { 0, "2**2**2", "16" },
  { 0, "-2**2**2", "-16" },

  { 0, "0x100", "256" },
  { 10, "0x100", NULL },
  { 10, "0x 100", NULL },

  { 0, " max ( 1, 2, 3, 4, 5, 6, 7, 8)", "8" },
  { 0, " max ( 1, 9, 2, 3, 4, 5, 6, 7, 8)", "9" },
  { 0, " min ( 1, 9, 2, 3, 4, 5, 6, 7, 8)", "1" },

  { 10, "abs(123)",  "123" },
  { 10, "abs(-123)", "123" },
  { 10, "abs(0)",    "0" },

  /* filling data stack */
  { 0, "1+(1+(1+(1+(1+(1+(1+(1+(1+(1+(1+(1+(1+(1+(1+1))))))))))))))", "16" },

  /* filling control stack */
  { 0, "----------------------------------------------------1", "1" },
};


const struct data_t  data_z[] = {
  { 0, "cmpabs(0,0)", "0" },
  { 0, "cmpabs(1,0)", "1" },
  { 0, "cmpabs(0,1)", "-1" },
  { 0, "cmpabs(-1,0)", "1" },
  { 0, "cmpabs(0,-1)", "-1" },

  { 0, "divisible_p(333,3)", "1" },
  { 0, "congruent_p(7,1,3)", "1" },

  { 0, "odd_p(1)", "1" },
  { 0, "odd_p(0)", "0" },
  { 0, "odd_p(-1)", "1" },

  { 0, "even_p(1)", "0" },
  { 0, "even_p(0)", "1" },
  { 0, "even_p(-1)", "0" },

  { 10, "root(81,4)", "3" },

  { 10, "gcd(4,6)", "2" },
  { 10, "gcd(4,6,9)", "1" },

  { 10, "powm(3,2,9)", "0" },
  { 10, "powm(3,2,8)", "1" },

  /* filling data stack */
  { 0, "1 ? 1 : 1 || 1 && 1 | 1 ^ 1 & 1 == 1 >= 1 << 1 - 1 * 1 ** 1", "1" },

  /* filling control stack */
  { 0, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~1", "1" },

  { 0, "fib(10)", "55" },

  { 0, "setbit(0,5)", "32" },
  { 0, "clrbit(32,5)", "0" },
  { 0, "tstbit(32,5)", "1" },
  { 0, "tstbit(32,4)", "0" },
  { 0, "scan0(7,0)", "3" },
  { 0, "scan1(7,0)", "0" },
};

const struct data_t  data_zq[] = {
  /* expecting failure */
  { 0, "1.2", NULL },
};

const struct data_t  data_zr[] = {
  { 0, "fac(0)",  "1" },
  { 0, "fac(1)",  "1" },
  { 0, "fac(2)",  "2" },
  { 0, "fac(3)",  "6" },
  { 0, "fac(10)", "3628800" },
};

const struct data_t  data_q[] = {
  { 10,  "(1/2 + 1/3 + 1/4 + 1/5 + 1/6)*20", "29" },
  { 0, "num(5/9)", "5" },
  { 0, "den(5/9)", "9" },
};

const struct data_t  data_zfr[] = {
  { 10, "sqrt ( 49 )", "7" },
  { 10, "sqrt ( 49 ) + 1", "8" },
  { 10, "sqrt((49))", "7" },
  { 10, "sqrt((((((((49))))))))", "7" },
};

const struct data_t  data_fr[] = {
  { 0, "1@10",    "10000000000" },
  { 0, "1.5@10",  "15000000000" },
  { 0, "1000@-1", "100" },
  { 0, "10.00@-1", "1" },

  { 0, "1e10",     "10000000000" },
  { 0, "1.5e10",   "15000000000" },
  { 0, "1000e-1",  "100" },
  { 0, "10.00e-1", "1" },

  { 16, "1@9",  "68719476736" },

  { 0, "ceil(0)",           "0" },
  { 0, "ceil(0.25)",        "1" },
  { 0, "ceil(0.5)",         "1" },
  { 0, "ceil(1.5)",         "2" },
  { 0, "ceil(-0.5)",        "0" },
  { 0, "ceil(-1.5)",        "-1" },

  /* only simple cases because mpf_eq currently only works on whole limbs */
  { 0, "eq(0xFFFFFFFFFFFFFFFF1111111111111111,0xFFFFFFFFFFFFFFFF2222222222222222,64)", "1" },
  { 0, "eq(0xFFFFFFFFFFFFFFFF1111111111111111,0xFFFFFFFFFFFFFFFF2222222222222222,128)", "0" },

  { 0, "floor(0)",           "0" },
  { 0, "floor(0.25)",        "0" },
  { 0, "floor(0.5)",         "0" },
  { 0, "floor(1.5)",         "1" },
  { 0, "floor(-0.5)",        "-1" },
  { 0, "floor(-1.5)",        "-2" },

  { 0, "trunc(0)",           "0" },
  { 0, "trunc(0.25)",        "0" },
  { 0, "trunc(0.5)",         "0" },
  { 0, "trunc(1.5)",         "1" },
  { 0, "trunc(-0.5)",        "0" },
  { 0, "trunc(-1.5)",        "-1" },

};

const struct data_t  data_f[] = {
  { 16,  "1@10", "18446744073709551616" },
  { -16, "1@10", "1099511627776" },

  { 0, "integer_p(1)",   "1" },
  { 0, "integer_p(0.5)", "0" },
};

const struct data_t  data_r[] = {
  { 16, "1@10", "1099511627776" },

  { 0, "euler",         "" },
  { 0, "euler+1",       "" },

  { 0, "loge2",         "" },
  { 0, "loge2+1",       "" },

  { 0, "pi",            "" },
  { 0, "2*pi",          "" },
  { 0, "pi()",          NULL },
  { 0, "2 + pi() * 4",  NULL },

  { 0, "acos(0)-pi/2",  "~0" },
  { 0, "acos(1)",       "0" },

  { 0, "agm(0,0)",      "0" },
  { 0, "agm(1,1)",      "1" },

  { 0, "asin(0)",       "0" },
  { 0, "asin(1)-pi/2",  "~0" },

  { 0, "atan(0)",       "0" },
  { 0, "atan(1)-pi/4",  "~0" },
  { 0, "atan(-1)+pi/4", "~0" },

  { 0, "cos(0)",        "1" },
  { 0, "cos(pi/2)",     "~0" },
  { 0, "cos(pi)",       "~-1" },
  { 0, "cos(3*pi/2)",   "~0" },
  { 0, "cos(2*pi)",     "~1" },

  { 0, "cosh(0)",       "1" },

  { 0, "dim(0,0)",      "0" },
  { 0, "dim(2,0)",      "2" },
  { 0, "dim(3,1)",      "2" },
  { 0, "dim(1,3)",      "0" },
  { 0, "dim(-2,0)",     "0" },
  { 0, "dim(-3,-1)",    "0" },
  { 0, "dim(-1,-3)",    "2" },

  { 0, "eq(0xFF,0xF0,4)", "1" },
  { 0, "eq(0xFF,0xF0,5)", "0" },

  { 0, "exp(0)",          "1" },
  { 0, "expm1(0)",        "0" },

  { 0, "fma(2,3,4)",      "10" },
  { 0, "fma(-2,3,4)",     "-2" },
  { 0, "fma(2,-3,4)",     "-2" },
  { 0, "fma(2,-3,-4)",    "-10" },

  { 0, "hypot(3,4)",      "5" },

  { 0, "inf_p(1)",        "0" },
  { 0, "inf_p(-1)",       "0" },
  { 0, "inf_p(1/0)",      "1" },
  { 0, "inf_p(sqrt(-1))", "0" },
  { 0, "inf_p(1)",        "0" },
  { 0, "inf_p(nan)",      "0" },
  { 0, "inf_p(inf)",      "1" },
  { 0, "inf_p(-inf)",     "1" },

  { 0, "inf > 0",         "1" },
  { 0, "inf == 0",        "0" },
  { 0, "inf < 0",         "0" },
  { 0, "-inf > 0",        "0" },
  { 0, "-inf == 0",       "0" },
  { 0, "-inf < 0",        "1" },
  { 0, "inf == inf",      "1" },
  { 0, "inf != inf",      "0" },
  { 0, "inf == -inf",     "0" },
  { 0, "inf != -inf",     "1" },

  { 0, "nan_p(log(-1))",  "1" },
  { 0, "inf_p(log(0))",   "1" },
  { 0, "log(0) == -inf",  "1" },
  { 0, "log(1)",          "~0" },

  { 0, "inf_p(log2(0))",  "1" },
  { 0, "log2(1)",         "0" },
  { 0, "log2(2)",         "1" },
  { 0, "log2(4)",         "2" },
  { 0, "log2(8)",         "3" },

  { 0, "inf_p(log10(0))",  "1" },
  { 0, "log10(1)",         "0" },
  { 0, "log10(10)",        "1" },
  { 0, "log10(100)",       "2" },
  { 0, "log10(1000)",      "3" },

  { 0, "log1p(0)",         "0" },

  { 0, "nan_p(-1)",        "0" },
  { 0, "nan_p(1/0)",       "0" },
  { 0, "nan_p(sqrt(-1))",  "1" },
  { 0, "nan_p(1)",         "0" },
  { 0, "nan_p(nan)",       "1" },
  { 0, "nan_p(inf)",       "0" },
  { 0, "nan_p(-inf)",      "0" },

  { 0, "number_p(-1)",       "1" },
  { 0, "number_p(1/0)",      "0" },
  { 0, "number_p(sqrt(-1))", "0" },
  { 0, "number_p(1)",        "1" },
  { 0, "number_p(nan)",      "0" },
  { 0, "number_p(inf)",      "0" },
  { 0, "number_p(-inf)",     "0" },

  { 0, "round(0)",           "0" },
  { 0, "round(0.25)",        "0" },
  { 0, "round(0.5)",         "1" },
  { 0, "round(1.5)",         "2" },
  { 0, "round(-0.5)",        "-1" },
  { 0, "round(-1.5)",        "-2" },

  { 0, "sin(0)",        "0" },
  { 0, "sin(pi/2)",     "~1" },
  { 0, "sin(pi)",       "~0" },
  { 0, "sin(3*pi/2)",   "~-1" },
  { 0, "sin(2*pi)",     "~0" },

  { 0, "sinh(0)",       "0" },

  { 0, "tan(0)",        "0" },
  { 0, "tan(pi/4)",     "~1" },
  { 0, "tan(3*pi/4)",   "~-1" },
  { 0, "tan(pi)",       "~0" },
  { 0, "tan(5*pi/4)",   "~1" },
  { 0, "tan(7*pi/4)",   "~-1" },

  { 0, "tan(0)",        "0" },
};

struct datalist_t {
  const struct data_t  *data;
  int                  num;
};

#define DATALIST(data)  { data, numberof (data) }

struct datalist_t  list_z[] = {
  DATALIST (data_z),
  DATALIST (data_zq),
  DATALIST (data_zfr),
  DATALIST (data_zqfr),
  DATALIST (data_zr),
};

struct datalist_t  list_q[] = {
  DATALIST (data_q),
  DATALIST (data_zq),
  DATALIST (data_zqfr),
};

struct datalist_t  list_f[] = {
  DATALIST (data_zfr),
  DATALIST (data_zqfr),
  DATALIST (data_fr),
  DATALIST (data_f),
};

struct datalist_t  list_r[] = {
  DATALIST (data_zfr),
  DATALIST (data_zqfr),
  DATALIST (data_zr),
  DATALIST (data_fr),
  DATALIST (data_r),
};


void
check_z (void)
{
  const struct data_t  *data;
  mpz_t  a, b, got, want;
  int    l, i, ret;

  mpz_init (got);
  mpz_init (want);
  mpz_init_set_ui (a, 55);
  mpz_init_set_ui (b, 99);

  for (l = 0; l < numberof (list_z); l++)
    {
      data = list_z[l].data;

      for (i = 0; i < list_z[l].num; i++)
        {
          if (option_trace)
            printf ("mpz_expr \"%s\"\n", data[i].expr);

          ret = mpz_expr (got, data[i].base, data[i].expr, a, b, NULL);

          if (data[i].want == NULL)
            {
              /* expect to fail */
              if (ret == MPEXPR_RESULT_OK)
                {
                  printf ("mpz_expr wrong return value, got %d, expected failure\n", ret);
                  goto error;
                }
            }
          else
            {
              if (mpz_set_str (want, data[i].want, 0) != 0)
                {
                  printf ("Cannot parse wanted value string\n");
                  goto error;
                }
              if (ret != MPEXPR_RESULT_OK)
                {
                  printf ("mpz_expr failed unexpectedly\n");
                  printf ("   return value %d\n", ret);
                  goto error;
                }
              if (mpz_cmp (got, want) != 0)
                {
                  printf ("mpz_expr wrong result\n");
                  printf ("   got  "); mpz_out_str (stdout, 10, got);
                  printf ("\n");
                  printf ("   want "); mpz_out_str (stdout, 10, want);
                  printf ("\n");
                  goto error;
                }
            }
        }
    }
  mpz_clear (a);
  mpz_clear (b);
  mpz_clear (got);
  mpz_clear (want);
  return;

 error:
  printf ("   base %d\n", data[i].base);
  printf ("   expr \"%s\"\n", data[i].expr);
  if (data[i].want != NULL)
    printf ("   want \"%s\"\n", data[i].want);
  abort ();
}

void
check_q (void)
{
  const struct data_t  *data;
  mpq_t  a, b, got, want;
  int    l, i, ret;

  mpq_init (got);
  mpq_init (want);
  mpq_init (a);
  mpq_init (b);

  mpq_set_ui (a, 55, 1);
  mpq_set_ui (b, 99, 1);

  for (l = 0; l < numberof (list_q); l++)
    {
      data = list_q[l].data;

      for (i = 0; i < list_q[l].num; i++)
        {
          if (option_trace)
            printf ("mpq_expr \"%s\"\n", data[i].expr);

          ret = mpq_expr (got, data[i].base, data[i].expr, a, b, NULL);

          if (data[i].want == NULL)
            {
              /* expect to fail */
              if (ret == MPEXPR_RESULT_OK)
                {
                  printf ("mpq_expr wrong return value, got %d, expected failure\n", ret);
                  goto error;
                }
            }
          else
            {
              if (mpz_set_str (mpq_numref(want), data[i].want, 0) != 0)
                {
                  printf ("Cannot parse wanted value string\n");
                  goto error;
                }
              mpz_set_ui (mpq_denref(want), 1);

              if (ret != MPEXPR_RESULT_OK)
                {
                  printf ("mpq_expr failed unexpectedly\n");
                  printf ("   return value %d\n", ret);
                  goto error;
                }
              if (mpq_cmp (got, want) != 0)
                {
                  printf ("mpq_expr wrong result\n");
                  printf ("   got  "); mpq_out_str (stdout, 10, got);
                  printf ("\n");
                  printf ("   want "); mpq_out_str (stdout, 10, want);
                  printf ("\n");
                  goto error;
                }
            }
        }
    }
  mpq_clear (a);
  mpq_clear (b);
  mpq_clear (got);
  mpq_clear (want);
  return;

 error:
  printf ("   base %d\n", data[i].base);
  printf ("   expr \"%s\"\n", data[i].expr);
  if (data[i].want != NULL)
    printf ("   want \"%s\"\n", data[i].want);
  abort ();
}

void
check_f (void)
{
  const struct data_t  *data;
  mpf_t  a, b, got, want;
  int    l, i, ret;

  mpf_set_default_prec (200L);

  mpf_init (got);
  mpf_init (want);
  mpf_init_set_ui (a, 55);
  mpf_init_set_ui (b, 99);

  for (l = 0; l < numberof (list_f); l++)
    {
      data = list_f[l].data;

      for (i = 0; i < list_f[l].num; i++)
        {
          if (option_trace)
            printf ("mpf_expr \"%s\"\n", data[i].expr);

          ret = mpf_expr (got, data[i].base, data[i].expr, a, b, NULL);

          if (data[i].want == NULL)
            {
              /* expect to fail */
              if (ret == MPEXPR_RESULT_OK)
                {
                  printf ("mpf_expr wrong return value, got %d, expected failure\n", ret);
                  goto error;
                }
            }
          else
            {
              if (mpf_set_str (want, data[i].want, 0) != 0)
                {
                  printf ("Cannot parse wanted value string\n");
                  goto error;
                }

              if (ret != MPEXPR_RESULT_OK)
                {
                  printf ("mpf_expr failed unexpectedly\n");
                  printf ("   return value %d\n", ret);
                  goto error;
                }
              if (mpf_cmp (got, want) != 0)
                {
                  printf ("mpf_expr wrong result\n");
                  printf ("   got  "); mpf_out_str (stdout, 10, 20, got);
                  printf ("\n");
                  printf ("   want "); mpf_out_str (stdout, 10, 20, want);
                  printf ("\n");
                  goto error;
                }
            }
        }
    }
  mpf_clear (a);
  mpf_clear (b);
  mpf_clear (got);
  mpf_clear (want);
  return;

 error:
  printf ("   base %d\n", data[i].base);
  printf ("   expr \"%s\"\n", data[i].expr);
  if (data[i].want != NULL)
    printf ("   want \"%s\"\n", data[i].want);
  abort ();
}

#if HAVE_MPFR
void
check_r (void)
{
  const struct data_t  *data;
  mpfr_t  a, b, got, want, diff, tolerance;
  int     l, i, ret;

  mpfr_set_default_prec (300L);

  mpfr_init (got);
  mpfr_init (want);
  mpfr_init (diff);
  mpfr_init (tolerance);
  mpfr_init (a);
  mpfr_init (b);
  mpfr_set_ui (a, 55L, GMP_RNDZ);
  mpfr_set_ui (b, 99L, GMP_RNDZ);

  mpfr_set_ui (tolerance, 1L, GMP_RNDZ);
  mpfr_div_2exp (tolerance, tolerance, 190L, GMP_RNDZ);

  for (l = 0; l < numberof (list_r); l++)
    {
      data = list_r[l].data;

      for (i = 0; i < list_r[l].num; i++)
        {
          if (option_trace)
            printf ("mpfr_expr \"%s\"\n", data[i].expr);

          ret = mpfr_expr (got, data[i].base, data[i].expr, a, b, NULL);

          if (data[i].want == NULL)
            {
              /* expect to fail */
              if (ret == MPEXPR_RESULT_OK)
                {
                  printf ("mpfr_expr wrong return value, got %d, expected failure\n", ret);
                  goto error;
                }
            }
          else
            {
              if (ret != MPEXPR_RESULT_OK)
                {
                  printf ("mpfr_expr failed unexpectedly\n");
                  printf ("   return value %d\n", ret);
                  goto error;
                }

              /* empty string means don't check the result value as such */
              if (data[i].want[0] != '\0')
                {
                  const char  *want_str = data[i].want;
                  int         approx = (*want_str == '~');
                  want_str += approx;
                  if (mpfr_set_str (want, want_str, 10, GMP_RNDZ) != 0)
                    {
                      printf ("Cannot parse wanted value string\n");
                      printf ("    \"%s\"\n", want_str);
                      goto error;
                    }
                  if (approx)
                    {
                      mpfr_sub (diff, got, want, GMP_RNDZ);
                      mpfr_abs (diff, diff, GMP_RNDZ);
                      if (mpfr_cmp (diff, tolerance) >= 0)
                        {
                          printf ("mpfr_expr result outside tolerance\n");
                          printf ("   \"%s\"\n", data[i].expr);
                          printf ("   got  ");
                          mpfr_out_str (stdout, 10, 0, got, GMP_RNDZ);
                          printf ("\n");
                          printf ("   want ");
                          mpfr_out_str (stdout, 10, 0, want, GMP_RNDZ);
                          printf ("\n");
                          printf ("   diff ");
                          mpfr_out_str (stdout, 10, 0, diff, GMP_RNDZ);
                          printf ("\n");
                          goto error;
                        }
                    }
                  else
                    {
                      if (mpfr_cmp (got, want) != 0)
                        {
                          printf ("mpfr_expr wrong result\n");
                          printf ("   \"%s\"\n", data[i].expr);
                          printf ("   got  ");
                          mpfr_out_str (stdout, 10, 20, got, GMP_RNDZ);
                          printf ("\n");
                          printf ("   want ");
                          mpfr_out_str (stdout, 10, 20, want, GMP_RNDZ);
                          printf ("\n");
                          goto error;
                        }
                    }
                }
            }
        }
    }
  mpfr_clear (a);
  mpfr_clear (b);
  mpfr_clear (got);
  mpfr_clear (want);
  mpfr_clear (diff);
  mpfr_clear (tolerance);

  if (__mpfr_const_pi_prec != 0)
    {
      mpfr_clear (__mpfr_const_pi);
      __mpfr_const_pi_prec = 0;
    }
  if (__mpfr_const_log2_prec != 0)
    {
      mpfr_clear (__mpfr_const_log2);
      __mpfr_const_log2_prec = 0;
    }

  return;

 error:
  printf ("   base %d\n", data[i].base);
  printf ("   expr \"%s\"\n", data[i].expr);
  if (data[i].want != NULL)
    printf ("   want \"%s\"\n", data[i].want);
  abort ();
}
#endif


int
main (int argc, char *argv[])
{
  tests_start ();

  if (argc >= 2)
    option_trace = 1;

  check_z ();
  check_q ();
  check_f ();
#if HAVE_MPFR
  check_r ();
#endif

  tests_end ();
  exit (0);
}
