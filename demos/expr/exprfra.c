/* mpfr expression evaluation

Copyright 2000, 2001 Free Software Foundation, Inc.

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

#include <ctype.h>
#include <stdio.h>
#include "gmp.h"
#include "expr-impl.h"

#ifndef ULONG_MAX
#define ULONG_MAX  (~ (unsigned long) 0)
#endif


/* Change this to "#define TRACE(x) x" to get some traces. */
#define TRACE(x)


static void
e_mpfr_init2 (mpfr_ptr f, unsigned long prec)
{
  mpfr_init2 (f, prec);
}

static void
e_mpfr_set (mpfr_ptr dst, mpfr_srcptr src)
{
  mpfr_set (dst, src, ROUND);
}

static int
e_mpfr_ulong_p (mpfr_srcptr f)
{
  return mpfr_integer_p (f) && mpfr_fits_ulong_p (f, GMP_RNDZ);
}

static unsigned long
e_mpfr_get_ui_fits (mpfr_srcptr f)
{
  return mpfr_get_ui (f, GMP_RNDZ);
}

static size_t
e_mpfr_number (mpfr_ptr res, __gmp_const char *e, size_t elen, int base)
{
  char    *edup;
  size_t  i, j, ret, extra=0;
  void    *(*allocate_func) (size_t);
  void    (*free_func) (void *, size_t);

  TRACE (printf ("mpfr_number prec=%lu, base=%d, \"%.*s\"\n",
                 mpfr_get_prec (res), base, (int) elen, e));

  /* mpfr_set_str doesn't currently accept 0x for hex in base==0, so do it
     here instead.  FIXME: Would prefer to let mpfr_set_str handle this.  */
  if (base == 0)
    {
      if (elen >= 2 && e[0] == '0' && (e[1] == 'x' || e[1] == 'X'))
        {
          base = 16;
          extra = 2;
          e += extra;
          elen -= extra;
        }
      else
        base = 10;
    }

  i = 0;
  for (;;)
    {
      if (i >= elen)
        goto parsed;
      if (e[i] == '.')
        break;
      if (e[i] == '@' || (base <= 10 && (e[i] == 'e' || e[i] == 'E')))
        goto exponent;
      if (! isasciidigit_in_base (e[i], base == 0 ? 10 : base))
        goto parsed;
      i++;
    }

  /* fraction */
  i++;
  for (;;)
    {
      if (i >= elen)
        goto parsed;
      if (e[i] == '@' || (base <= 10 && (e[i] == 'e' || e[i] == 'E')))
        break;
      if (! isasciidigit_in_base (e[i], base == 0 ? 10 : base))
        goto parsed;
      i++;
    }

 exponent:
  i++;
  if (i >= elen)
    goto parsed;
  if (e[i] == '-')
    i++;
  for (;;)
    {
      if (i >= elen)
        goto parsed;
      if (! isasciidigit_in_base (e[i], base == 0 ? 10 : base))
        break;
      i++;
    }

 parsed:
  TRACE (printf ("  parsed i=%d \"%.*s\"\n", i, i, e));

  /* mpfr_set_str doesn't currently accept upper case for hex, so convert to
     lower here instead.  FIXME: Would prefer to let mpfr_set_str handle
     this.  */
  mp_get_memory_functions (&allocate_func, NULL, &free_func);
  edup = (*allocate_func) (i+1);
  for (j = 0; j < i; j++)
    edup[j] = tolower (e[j]);
  edup[i] = '\0';

  TRACE (printf ("   attempt base=%d, len=%d, \"%s\"\n", base, i, edup));

  if (mpfr_set_str (res, edup, base, ROUND) != -1)
    ret = i + extra;
  else
    ret = 0;

  (*free_func) (edup, i+1);
  return ret;
}

/* Don't want to change the precision of w, can only do an actual swap when
   w and x have the same precision.  */
static void
e_mpfr_set_or_swap (mpfr_ptr w, mpfr_ptr x)
{
  if (mpfr_get_prec (w) == mpfr_get_prec (x))
    mpfr_swap (w, x);
  else
    mpfr_set (w, x, ROUND);
}

int
mpfr_expr_a (__gmp_const struct mpexpr_operator_t *table,
             mpfr_ptr res, int base, unsigned long prec,
             __gmp_const char *e, size_t elen,
             mpfr_srcptr var[26])
{
  struct mpexpr_parse_t  p;

  p.table = table;
  p.res = (mpX_ptr) res;
  p.base = base;
  p.prec = prec;
  p.e = e;
  p.elen = elen;
  p.var = (mpX_srcptr *) var;

  p.mpX_clear       = (mpexpr_fun_one_t)      mpfr_clear;
  p.mpX_ulong_p     = (mpexpr_fun_i_unary_t)  e_mpfr_ulong_p;
  p.mpX_get_ui      = (mpexpr_fun_get_ui_t)   e_mpfr_get_ui_fits;
  p.mpX_init        = (mpexpr_fun_unary_ui_t) e_mpfr_init2;
  p.mpX_number      = (mpexpr_fun_number_t)   e_mpfr_number;
  p.mpX_set         = (mpexpr_fun_unary_t)    e_mpfr_set;
  p.mpX_set_or_swap = (mpexpr_fun_unary_t)    e_mpfr_set_or_swap;
  p.mpX_set_si      = (mpexpr_fun_set_si_t)   mpfr_set_si;
  p.mpX_swap        = (mpexpr_fun_swap_t)     mpfr_swap;

  return mpexpr_evaluate (&p);
}
