/* mpfr_nextabove, mpfr_nextbelow, mpfr_nexttoward -- next representable
floating-point number

Copyright 1999, 2001, 2002 Free Software Foundation.
Contributed by the Spaces project, INRIA Lorraine.

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

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

static void
mpfr_nexttozero (mpfr_ptr x)
{
  if (MPFR_IS_INF(x))
    {
      MPFR_CLEAR_FLAGS(x);
      mpfr_setmax (x, __gmpfr_emax);
      return;
    }

  if (MPFR_IS_ZERO(x))
    {
      MPFR_CHANGE_SIGN(x);
      mpfr_setmin (x, __gmpfr_emin);
    }
  else
    {
      mp_size_t xn;
      int sh;
      mp_limb_t *xp;

      xn = 1 + (MPFR_PREC(x) - 1) / BITS_PER_MP_LIMB;
      sh = (mp_prec_t) xn * BITS_PER_MP_LIMB - MPFR_PREC(x);
      xp = MPFR_MANT(x);
      mpn_sub_1 (xp, xp, xn, MP_LIMB_T_ONE << sh);
      if (xp[xn-1] >> (BITS_PER_MP_LIMB - 1) == 0)
        { /* was an exact power of two: not normalized any more */
          mp_exp_t exp = MPFR_EXP(x);
          if (exp == __gmpfr_emin)
            MPFR_SET_ZERO(x);
          else
            {
              mp_size_t i;
              MPFR_EXP(x)--;
              xp[0] = MP_LIMB_T_MAX << sh;
              for (i = 1; i < xn; i++)
                xp[i] = MP_LIMB_T_MAX;
            }
        }
    }
}

static void
mpfr_nexttoinf (mpfr_ptr x)
{
  if (MPFR_IS_INF(x))
    return;

  if (MPFR_IS_ZERO(x))
    mpfr_setmin (x, __gmpfr_emin);
  else
    {
      mp_size_t xn;
      int sh;
      mp_limb_t *xp;

      xn = 1 + (MPFR_PREC(x) - 1) / BITS_PER_MP_LIMB;
      sh = (mp_prec_t) xn * BITS_PER_MP_LIMB - MPFR_PREC(x);
      xp = MPFR_MANT(x);
      if (mpn_add_1 (xp, xp, xn, MP_LIMB_T_ONE << sh)) /* got 1.0000... */
        {
          mp_exp_t exp = MPFR_EXP(x);
          if (exp == __gmpfr_emax)
            MPFR_SET_INF(x);
          else
            {
              MPFR_EXP(x)++;
              xp[xn-1] = MPFR_LIMB_HIGHBIT;
            }
        }
    }
}

void
mpfr_nextabove (mpfr_ptr x)
{
  if (MPFR_IS_NAN(x))
    {
      __gmpfr_flags |= MPFR_FLAGS_NAN;
      return;
    }

  if (MPFR_SIGN(x) < 0)
    mpfr_nexttozero (x);
  else
    mpfr_nexttoinf (x);
}

void
mpfr_nextbelow (mpfr_ptr x)
{
  if (MPFR_IS_NAN(x))
    {
      __gmpfr_flags |= MPFR_FLAGS_NAN;
      return;
    }

  if (MPFR_SIGN(x) < 0)
    mpfr_nexttoinf (x);
  else
    mpfr_nexttozero (x);
}

void
mpfr_nexttoward (mpfr_ptr x, mpfr_srcptr y)
{
  int s;

  if (MPFR_IS_NAN(x) || MPFR_IS_NAN(y))
    {
      __gmpfr_flags |= MPFR_FLAGS_NAN;
      return;
    }

  s = mpfr_cmp (x, y);
  if (s == 0)
    return;
  if (s < 0)
    mpfr_nextabove (x);
  else
    mpfr_nextbelow (x);
}
