/* mpfr expression evaluation.

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
#include "gmp.h"
#include "expr-impl.h"


/* Change this to "#define TRACE(x) x" to get some traces. */
#define TRACE(x)


/* This set of functions makes it possible to add the ROUND parameter, and a
   few of the mpfr's are macros and need a wrapper anyway.

   A whole new set of functions like this just to add a rounding parameter
   is a bit horrible, but it's the easiest way.  There doesn't seem much use
   having rounding passed down through mpfr_expr_a, since a single rounding
   for all operations in an expression doesn't give close enough control to
   be useful.  */

static void
e_mpfr_abs (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_abs (dst, src, ROUND);
}

static void
e_mpfr_acos (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_acos (dst, src, ROUND);
}

static void
e_mpfr_add (mpfr_ptr dst, mpfr_srcptr src1, mpfr_srcptr src2)
{
  mpfr_add (dst, src1, src2, ROUND);
}

static void
e_mpfr_agm (mpfr_ptr dst, mpfr_srcptr src1, mpfr_srcptr src2)
{
  mpfr_agm (dst, src1, src2, ROUND);
}

static void
e_mpfr_asin (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_asin (dst, src, ROUND);
}

static void
e_mpfr_atan (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_atan (dst, src, ROUND);
}

/* because mpfr_cmp is a macro */
static int
e_mpfr_cmp (mpfr_srcptr x, mpfr_srcptr y)
{
  return mpfr_cmp (x, y);
}

static void
e_mpfr_dim (mpfr_ptr dst, mpfr_srcptr src1, mpfr_srcptr src2)
{
  mpfr_dim (dst, src1, src2, ROUND);
}

static void
e_mpfr_const_euler (mpfr_ptr dst)
{
  mpfr_const_euler (dst, ROUND);
}

static void
e_mpfr_fac_ui (mpfr_ptr dst, unsigned long n)
{
  mpfr_fac_ui (dst, n, ROUND);
}

static void
e_mpfr_const_log2 (mpfr_ptr dst)
{
  mpfr_const_log2 (dst, ROUND);
}

static void
e_mpfr_const_pi (mpfr_ptr dst)
{
  mpfr_const_pi (dst, ROUND);
}

static void
e_mpfr_cos (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_cos (dst, src, ROUND);
}

static void
e_mpfr_cosh (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_cosh (dst, src, ROUND);
}

static void
e_mpfr_div (mpfr_ptr dst, mpfr_srcptr src1, mpfr_srcptr src2)
{
  mpfr_div (dst, src1, src2, ROUND);
}

static void
e_mpfr_div_2exp (mpfr_ptr dst, mpfr_srcptr src, unsigned long n)
{
  mpfr_div_2exp (dst, src, n, ROUND);
}

static void
e_mpfr_exp (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_exp (dst, src, ROUND);
}

static void
e_mpfr_expm1 (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_expm1 (dst, src, ROUND);
}

static void
e_mpfr_fma (mpfr_ptr w, mpfr_srcptr x, mpfr_srcptr y, mpfr_srcptr z)
{
  mpfr_fma (w, x, y, z, ROUND);
}

static void
e_mpfr_hypot (mpfr_ptr dst, mpfr_srcptr src1, mpfr_srcptr src2)
{
  mpfr_hypot (dst, src1, src2, ROUND);
}

static void
e_mpfr_log (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_log (dst, src, ROUND);
}

static void
e_mpfr_log2 (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_log2 (dst, src, ROUND);
}

static void
e_mpfr_log10 (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_log10 (dst, src, ROUND);
}

static void
e_mpfr_log1p (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_log1p (dst, src, ROUND);
}

static void
e_mpfr_mul (mpfr_ptr dst, mpfr_srcptr src1, mpfr_srcptr src2)
{
  mpfr_mul (dst, src1, src2, ROUND);
}

static void
e_mpfr_mul_2exp (mpfr_ptr dst, mpfr_srcptr src, unsigned long n)
{
  mpfr_mul_2exp (dst, src, n, ROUND);
}

static void
e_mpfr_neg (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_neg (dst, src, ROUND);
}

static void
e_mpfr_pow_ui (mpfr_ptr p, mpfr_srcptr b, unsigned long e)
{
  mpfr_pow_ui (p, b, e, ROUND);
}

static void
e_mpfr_reldiff (mpfr_ptr dst, mpfr_srcptr src1, mpfr_srcptr src2)
{
  mpfr_reldiff (dst, src1, src2, ROUND);
}

static void
e_mpfr_set_posinf (mpfr_ptr dst)
{
  mpfr_set_inf (dst, 1);
}

static void
e_mpfr_sin (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_sin (dst, src, ROUND);
}

static void
e_mpfr_sinh (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_sinh (dst, src, ROUND);
}

static int
e_mpfr_sgn (mpfr_srcptr x)
{
  return mpfr_sgn (x);
}

static void
e_mpfr_sqrt (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_sqrt (dst, src, ROUND);
}

static void
e_mpfr_sub (mpfr_ptr dst, mpfr_srcptr src1, mpfr_srcptr src2)
{
  mpfr_sub (dst, src1, src2, ROUND);
}

static void
e_mpfr_tan (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_tan (dst, src, ROUND);
}

static void
e_mpfr_tanh (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_tanh (dst, src, ROUND);
}


static __gmp_const struct mpexpr_operator_t  _mpfr_expr_standard_table[] = {

  { "**",  (mpexpr_fun_t) e_mpfr_pow_ui,
    MPEXPR_TYPE_BINARY_UI | MPEXPR_TYPE_RIGHTASSOC,                   220 },
  
  { "!",   (mpexpr_fun_t) e_mpfr_sgn,
    MPEXPR_TYPE_LOGICAL_NOT | MPEXPR_TYPE_PREFIX,                     210 },
  { "-",   (mpexpr_fun_t) e_mpfr_neg,
    MPEXPR_TYPE_UNARY | MPEXPR_TYPE_PREFIX,                           210 },

  { "*",   (mpexpr_fun_t) e_mpfr_mul,        MPEXPR_TYPE_BINARY,      200 },
  { "/",   (mpexpr_fun_t) e_mpfr_div,        MPEXPR_TYPE_BINARY,      200 },

  { "+",   (mpexpr_fun_t) e_mpfr_add,        MPEXPR_TYPE_BINARY,      190 },
  { "-",   (mpexpr_fun_t) e_mpfr_sub,        MPEXPR_TYPE_BINARY,      190 },

  { "<<",  (mpexpr_fun_t) e_mpfr_mul_2exp,   MPEXPR_TYPE_BINARY_UI,   180 },
  { ">>",  (mpexpr_fun_t) e_mpfr_div_2exp,   MPEXPR_TYPE_BINARY_UI,   180 },

  { "<=",  (mpexpr_fun_t) e_mpfr_cmp,        MPEXPR_TYPE_CMP_LE,      170 },
  { "<",   (mpexpr_fun_t) e_mpfr_cmp,        MPEXPR_TYPE_CMP_LT,      170 },
  { ">=",  (mpexpr_fun_t) e_mpfr_cmp,        MPEXPR_TYPE_CMP_GE,      170 },
  { ">",   (mpexpr_fun_t) e_mpfr_cmp,        MPEXPR_TYPE_CMP_GT,      170 },

  { "==",  (mpexpr_fun_t) e_mpfr_cmp,        MPEXPR_TYPE_CMP_EQ,      160 },
  { "!=",  (mpexpr_fun_t) e_mpfr_cmp,        MPEXPR_TYPE_CMP_NE,      160 },

  { "&&",  (mpexpr_fun_t) e_mpfr_sgn,        MPEXPR_TYPE_LOGICAL_AND, 120 },
  { "||",  (mpexpr_fun_t) e_mpfr_sgn,        MPEXPR_TYPE_LOGICAL_OR,  110 },

  { ":",   NULL,                             MPEXPR_TYPE_COLON,       101 },
  { "?",   (mpexpr_fun_t) e_mpfr_sgn,        MPEXPR_TYPE_QUESTION,    100 },

  { ")",   NULL,                             MPEXPR_TYPE_CLOSEPAREN,    4 },
  { "(",   NULL,                             MPEXPR_TYPE_OPENPAREN,     3 },
  { ",",   NULL,                             MPEXPR_TYPE_ARGSEP,        2 },
  { "$",   NULL,                             MPEXPR_TYPE_VARIABLE,      1 },

  { "abs",     (mpexpr_fun_t) e_mpfr_abs,     MPEXPR_TYPE_UNARY        },
  { "acos",    (mpexpr_fun_t) e_mpfr_acos,    MPEXPR_TYPE_UNARY        },
  { "agm",     (mpexpr_fun_t) e_mpfr_agm,     MPEXPR_TYPE_BINARY       },
  { "asin",    (mpexpr_fun_t) e_mpfr_asin,    MPEXPR_TYPE_UNARY        },
  { "atan",    (mpexpr_fun_t) e_mpfr_atan,    MPEXPR_TYPE_UNARY        },
  { "ceil",    (mpexpr_fun_t) mpfr_ceil,      MPEXPR_TYPE_UNARY        },
  { "cmp",     (mpexpr_fun_t) e_mpfr_cmp,     MPEXPR_TYPE_I_BINARY     },
  { "cos",     (mpexpr_fun_t) e_mpfr_cos,     MPEXPR_TYPE_UNARY        },
  { "cosh",    (mpexpr_fun_t) e_mpfr_cosh,    MPEXPR_TYPE_UNARY        },
  { "dim",     (mpexpr_fun_t) e_mpfr_dim,     MPEXPR_TYPE_BINARY       },
  { "eq",      (mpexpr_fun_t) mpfr_eq,        MPEXPR_TYPE_I_TERNARY_UI },
  { "exp",     (mpexpr_fun_t) e_mpfr_exp,     MPEXPR_TYPE_UNARY        },
  { "expm1",   (mpexpr_fun_t) e_mpfr_expm1,   MPEXPR_TYPE_UNARY        },
  { "fac",     (mpexpr_fun_t) e_mpfr_fac_ui,  MPEXPR_TYPE_UNARY_UI     },
  { "floor",   (mpexpr_fun_t) mpfr_floor,     MPEXPR_TYPE_UNARY        },
  { "fma",     (mpexpr_fun_t) e_mpfr_fma,     MPEXPR_TYPE_TERNARY      },
  { "hypot",   (mpexpr_fun_t) e_mpfr_hypot,   MPEXPR_TYPE_BINARY       },
  { "inf_p",   (mpexpr_fun_t) mpfr_inf_p,     MPEXPR_TYPE_I_UNARY      },
  { "log",     (mpexpr_fun_t) e_mpfr_log,     MPEXPR_TYPE_UNARY        },
  { "log2",    (mpexpr_fun_t) e_mpfr_log2,    MPEXPR_TYPE_UNARY        },
  { "log10",   (mpexpr_fun_t) e_mpfr_log10,   MPEXPR_TYPE_UNARY        },
  { "log1p",   (mpexpr_fun_t) e_mpfr_log1p,   MPEXPR_TYPE_UNARY        },
  { "max",     (mpexpr_fun_t) e_mpfr_cmp,     MPEXPR_TYPE_MAX
                                              | MPEXPR_TYPE_PAIRWISE   },
  { "min",     (mpexpr_fun_t) e_mpfr_cmp,     MPEXPR_TYPE_MIN
                                              | MPEXPR_TYPE_PAIRWISE   },
  { "nan_p",   (mpexpr_fun_t) mpfr_nan_p,     MPEXPR_TYPE_I_UNARY      },
  { "number_p",(mpexpr_fun_t) mpfr_number_p,  MPEXPR_TYPE_I_UNARY      },
  { "reldiff", (mpexpr_fun_t) e_mpfr_reldiff, MPEXPR_TYPE_BINARY       },
  { "round",   (mpexpr_fun_t) mpfr_round,     MPEXPR_TYPE_UNARY        },
  { "sgn",     (mpexpr_fun_t) e_mpfr_sgn,     MPEXPR_TYPE_I_UNARY      },
  { "sin",     (mpexpr_fun_t) e_mpfr_sin,     MPEXPR_TYPE_UNARY        },
  { "sinh",    (mpexpr_fun_t) e_mpfr_sinh,    MPEXPR_TYPE_UNARY        },
  { "sqrt",    (mpexpr_fun_t) e_mpfr_sqrt,    MPEXPR_TYPE_UNARY        },
  { "tan",     (mpexpr_fun_t) e_mpfr_tan,     MPEXPR_TYPE_UNARY        },
  { "tanh",    (mpexpr_fun_t) e_mpfr_tanh,    MPEXPR_TYPE_UNARY        },
  { "trunc",   (mpexpr_fun_t) mpfr_trunc,     MPEXPR_TYPE_UNARY        },

  { "euler",   (mpexpr_fun_t) e_mpfr_const_euler, MPEXPR_TYPE_CONSTANT },
  { "loge2",   (mpexpr_fun_t) e_mpfr_const_log2,  MPEXPR_TYPE_CONSTANT },
  { "nan",     (mpexpr_fun_t) mpfr_set_nan,       MPEXPR_TYPE_CONSTANT },
  { "inf",     (mpexpr_fun_t) e_mpfr_set_posinf,  MPEXPR_TYPE_CONSTANT },
  { "pi",      (mpexpr_fun_t) e_mpfr_const_pi,    MPEXPR_TYPE_CONSTANT },

  { NULL }
};

__gmp_const struct mpexpr_operator_t * __gmp_const mpfr_expr_standard_table
= _mpfr_expr_standard_table;


int
#if HAVE_STDARG
mpfr_expr (mpfr_ptr res, int base, __gmp_const char *e, ...)
#else
mpfr_expr (va_alist)
     va_dcl
#endif
{
  mpfr_srcptr  var[MPEXPR_VARIABLES];
  va_list     ap;
  int         ret;
#if HAVE_STDARG
  va_start (ap, e);
#else
  mpfr_ptr          res;
  int               base;
  __gmp_const char  *e;
  va_start (ap);
  res  = va_arg (ap, mpfr_ptr);
  base = va_arg (ap, int);
  e    = va_arg (ap, __gmp_const char *);
#endif

  TRACE (printf ("mpfr_expr(): base %d, %s\n", base, e));
  ret = mpexpr_va_to_var ((void **) var, ap);
  va_end (ap);

  if (ret != MPEXPR_RESULT_OK)
    return ret;

  return mpfr_expr_a (mpfr_expr_standard_table, res, base,
                      mpfr_get_prec (res), e, strlen(e), var);
}
