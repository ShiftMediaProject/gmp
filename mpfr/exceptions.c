/* Exception flags and utilities.

Copyright 2001 Free Software Foundation.

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

unsigned int __mpfr_flags = 0;

mp_exp_t __mpfr_emin = MPFR_EMIN_DEFAULT;
mp_exp_t __mpfr_emax = MPFR_EMAX_DEFAULT;

#undef mpfr_get_emin

mp_exp_t
mpfr_get_emin (void)
{
  return __mpfr_emin;
}

#undef mpfr_set_emin

int
mpfr_set_emin (mp_exp_t exponent)
{
  if (exponent >= MPFR_EMIN_MIN && exponent <= MPFR_EMIN_MAX)
    {
      __mpfr_emin = exponent;
      return 0;
    }
  else
    {
      return 1;
    }
}

#undef mpfr_get_emax

mp_exp_t
mpfr_get_emax (void)
{
  return __mpfr_emax;
}

#undef mpfr_set_emax

int
mpfr_set_emax (mp_exp_t exponent)
{
  if (exponent >= MPFR_EMAX_MIN && exponent <= MPFR_EMAX_MAX)
    {
      __mpfr_emax = exponent;
      return 0;
    }
  else
    {
      return 1;
    }
}

#undef mpfr_clear_flags

void
mpfr_clear_flags (void)
{
  __mpfr_flags = 0;
}

#undef mpfr_clear_underflow

void
mpfr_clear_underflow (void)
{
  __mpfr_flags &= MPFR_FLAGS_ALL ^ MPFR_FLAGS_UNDERFLOW;
}

#undef mpfr_clear_overflow

void
mpfr_clear_overflow (void)
{
  __mpfr_flags &= MPFR_FLAGS_ALL ^ MPFR_FLAGS_OVERFLOW;
}

#undef mpfr_clear_nanflag

void
mpfr_clear_nanflag (void)
{
  __mpfr_flags &= MPFR_FLAGS_ALL ^ MPFR_FLAGS_NAN;
}

#undef mpfr_clear_inexflag

void
mpfr_clear_inexflag (void)
{
  __mpfr_flags &= MPFR_FLAGS_ALL ^ MPFR_FLAGS_INEXACT;
}

#undef mpfr_check_range

int
mpfr_check_range (mpfr_ptr x, mp_rnd_t rnd_mode)
{
  if (MPFR_IS_FP(x) && MPFR_NOTZERO(x))
    { /* x is a non-zero FP */
      mp_exp_t exp = MPFR_EXP(x);
      if (exp < __mpfr_emin)
        return mpfr_set_underflow(x, rnd_mode, MPFR_SIGN(x));
      if (exp > __mpfr_emax)
        return mpfr_set_overflow(x, rnd_mode, MPFR_SIGN(x));
    }
  return 0;
}

#undef mpfr_underflow_p

int
mpfr_underflow_p (void)
{
  return __mpfr_flags & MPFR_FLAGS_UNDERFLOW;
}

#undef mpfr_overflow_p

int
mpfr_overflow_p (void)
{
  return __mpfr_flags & MPFR_FLAGS_OVERFLOW;
}

#undef mpfr_nanflag_p

int
mpfr_nanflag_p (void)
{
  return __mpfr_flags & MPFR_FLAGS_NAN;
}

#undef mpfr_inexflag_p

int
mpfr_inexflag_p (void)
{
  return __mpfr_flags & MPFR_FLAGS_INEXACT;
}

#undef mpfr_set_underflow

int
mpfr_set_underflow (mpfr_ptr x, mp_rnd_t rnd_mode, int sign)
{
  int inex;

  MPFR_CLEAR_FLAGS(x);
  if ((rnd_mode == GMP_RNDU && sign > 0)
   || (rnd_mode == GMP_RNDD && sign < 0))
    {
      mp_size_t xn;
      mp_limb_t *xp;

      MPFR_EXP(x) = __mpfr_emin;
      xn = (MPFR_PREC(x)-1)/BITS_PER_MP_LIMB;
      xp = MPFR_MANT(x);
      xp[xn] = GMP_LIMB_HIGHBIT;
      MPN_ZERO(xp, xn);
      inex = 1;
    }
  else
    {
      MPFR_SET_ZERO(x);
      inex = -1;
    }
  if (MPFR_SIGN(x) != sign)
    MPFR_CHANGE_SIGN(x);
  __mpfr_flags |= MPFR_FLAGS_INEXACT | MPFR_FLAGS_UNDERFLOW;
  return sign > 0 ? inex : -inex;
}

#undef mpfr_set_overflow

int
mpfr_set_overflow (mpfr_ptr x, mp_rnd_t rnd_mode, int sign)
{
  int inex;

  MPFR_CLEAR_FLAGS(x);
  if ((rnd_mode == GMP_RNDU && sign < 0)
   || (rnd_mode == GMP_RNDD && sign > 0))
    {
      mp_size_t xn, i;
      int sh;
      mp_limb_t *xp;

      MPFR_EXP(x) = __mpfr_emax;
      xn = 1 + (MPFR_PREC(x) - 1) / BITS_PER_MP_LIMB;
      sh = xn * BITS_PER_MP_LIMB - MPFR_PREC(x);
      xp = MPFR_MANT(x);
      xp[0] = MP_LIMB_T_MAX << sh;
      for (i = 1; i < xn; i++)
        xp[i] = MP_LIMB_T_MAX;
      inex = -1;
    }
  else
    {
      MPFR_SET_INF(x);
      inex = 1;
    }
  if (MPFR_SIGN(x) != sign)
    MPFR_CHANGE_SIGN(x);
  __mpfr_flags |= MPFR_FLAGS_INEXACT | MPFR_FLAGS_OVERFLOW;
  return sign > 0 ? inex : -inex;
}
