/* mpfr_frac -- Fractional part of a floating-point number.

Copyright 2002, 2003 Free Software Foundation, Inc.

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
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_frac (mpfr_ptr r, mpfr_srcptr u, mp_rnd_t rnd_mode)
{
  mp_exp_t re, ue;
  mp_prec_t uq, fq;
  mp_size_t un, tn, t0;
  mp_limb_t *up, *tp, k;
  int sh;
  mpfr_t tmp;
  mpfr_ptr t;

  if (MPFR_IS_NAN(u))
    {
      MPFR_SET_NAN(r);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_INF(u) || mpfr_integer_p (u))
    {
      MPFR_CLEAR_FLAGS(r);
      MPFR_SET_SAME_SIGN(r, u);
      MPFR_SET_ZERO(r);
      MPFR_RET(0);  /* zero is exact */
    }

  ue = MPFR_GET_EXP (u);
  if (ue <= 0)  /* |u| < 1 */
    return mpfr_set(r, u, rnd_mode);

  uq = MPFR_PREC(u);
  un = (uq - 1) / BITS_PER_MP_LIMB;  /* index of most significant limb */
  un -= (mp_size_t) (ue / BITS_PER_MP_LIMB);
  /* now the index of the MSL containing bits of the fractional part */

  up = MPFR_MANT(u);
  sh = ue % BITS_PER_MP_LIMB;
  k = up[un] << sh;

  if (k != 0)
    {
      int cnt;

      count_leading_zeros(cnt, k);
      re = -cnt;
      sh += cnt;
      k <<= cnt;
    }
  else
    {
      re = sh - BITS_PER_MP_LIMB;
      while ((k = up[--un]) == 0)
        re -= BITS_PER_MP_LIMB;
      MPFR_ASSERTN(un >= 0);
      count_leading_zeros(sh, k);
      re -= sh;
      k <<= sh;
    }
  /* The exponent of r will be re */

  ue -= re;  /* number of bits of u to discard */
  fq = uq - ue;  /* number of bits of the fractional part of u */

  t = fq > MPFR_PREC(r) ?
    (mpfr_init2(tmp, (un + 1) * BITS_PER_MP_LIMB), tmp) : r;
  /* t has enough precision to contain the fractional part of u */
  /* If we use a temporary variable, we take the non-significant bits
     of u into account, because of the mpn_lshift below. */
  MPFR_CLEAR_FLAGS(t);
  MPFR_SET_SAME_SIGN(t, u);
  MPFR_SET_EXP (t, re);

  /* Put the fractional part of u into t */
  tn = (MPFR_PREC(t) - 1) / BITS_PER_MP_LIMB;
  MPFR_ASSERTN(tn >= un);
  t0 = tn - un;
  tp = MPFR_MANT(t);
  if (sh == 0)
    MPN_COPY_DECR(tp + t0, up, un + 1);
  else
    tp[tn] = k | mpn_lshift(tp + t0, up, un, sh);
  if (t0 > 0)
    MPN_ZERO(tp, t0);

  if (t != r)
    { /* t is tmp */
      int inex;

      inex = mpfr_set(r, t, rnd_mode);
      mpfr_clear(t);
      return inex;
    }
  else
    MPFR_RET(0);
}
