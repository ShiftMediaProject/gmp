/* mpfr expression evaluation */

/*
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
MA 02111-1307, USA.
*/

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

/* Test whether fits and is an integer.  FIXME: Use mpfr_integer_p and
   mpfr_fits_ulong_p (or just mpfr_cmp_ui), if/when these exist. */
static int
e_mpfr_ulong_p (mpfr_srcptr f)
{
  mp_size_t  size = f->_mpfr_size;
  mp_srcptr  ptr = f->_mpfr_d;
  mp_exp_t   exp = f->_mpfr_exp;
  mp_prec_t  high_index = (f->_mpfr_prec-1) / mp_bits_per_limb;
  mp_prec_t  i;
  mp_limb_t  high = ptr[high_index];

  if ((size & 0x60000000) != 0)  /* nan or inf don't fit */
    return 0;

  if (high == 0)  /* zero fits */
    return 1;

  for (i = 0; i < high_index; i++)  /* low limbs must be zero */
    if (ptr[i] != 0)
      return 0;

  if (exp <= 0)  /* fractions don't fit */
    return 0;

  if (exp > mp_bits_per_limb)  /* bigger than a limb doesn't fit */
    return 0;

  /* any fraction bits in the high limb must be zero */
  if (exp < mp_bits_per_limb && (high << exp) != 0)
    return 0;

  /* value must fit a ulong */
  return (high >> (mp_bits_per_limb - exp)) <= ULONG_MAX;
}

/* FIXME: Use mpfr_get_ui if/when it exists. */
static unsigned long
e_mpfr_get_ui_fits (mpfr_srcptr f)
{
  mp_srcptr  ptr = f->_mpfr_d;
  mp_exp_t   exp = f->_mpfr_exp;
  mp_prec_t  high_index = (f->_mpfr_prec-1) / mp_bits_per_limb;
  mp_limb_t  high = ptr[high_index];

  return high >> (mp_bits_per_limb - exp);
}

static size_t
e_mpfr_number (mpfr_ptr res, __gmp_const char *e, size_t elen, int base)
{
  char    *edup;
  size_t  i, j, ret, extra=0;

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
  edup = (*__gmp_allocate_func) (i+1);
  for (j = 0; j < i; j++)
    edup[j] = tolower (e[j]);
  edup[i] = '\0';

  TRACE (printf ("   attempt base=%d, len=%d, \"%s\"\n", base, i, edup));

  if (mpfr_set_str (res, edup, base, ROUND) == 0)
    ret = i + extra;
  else
    ret = 0;

  (*__gmp_free_func) (edup, i+1);
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
