/* mpfr_mul_ui -- multiply a floating-point number by a machine integer

Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

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
mpfr_mul_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int u, mp_rnd_t rnd_mode)
{
  mp_limb_t *yp, *old_yp;
  mp_size_t xn, yn;
  int cnt, c, inexact;
  TMP_DECL(marker);

  if (MPFR_IS_NAN(x))
    {
      MPFR_SET_NAN(y);
      MPFR_RET_NAN;
    }

  if (MPFR_IS_INF(x)) 
    {
      if (u != 0)
	{
          MPFR_CLEAR_FLAGS(y);
          MPFR_SET_INF(y);
          MPFR_SET_SAME_SIGN(y, x);
          MPFR_RET(0); /* infinity is exact */
	}
      else /* 0 * infinity */
	{
	  MPFR_SET_NAN(y);
          MPFR_RET_NAN;
	}
    }

  MPFR_CLEAR_FLAGS(y);

  if (u == 0 || MPFR_IS_ZERO(x))
    {
      MPFR_SET_ZERO(y);
      MPFR_SET_SAME_SIGN(y, x);
      MPFR_RET(0); /* zero is exact */
    }

  if (u == 1)
    return mpfr_set (y, x, rnd_mode);

  TMP_MARK(marker);
  yp = MPFR_MANT(y);
  yn = (MPFR_PREC(y) - 1) / BITS_PER_MP_LIMB + 1;
  xn = (MPFR_PREC(x) - 1) / BITS_PER_MP_LIMB + 1;

  old_yp = yp;

  MPFR_ASSERTN(xn < MP_SIZE_T_MAX);
  if (yn < xn + 1)
    yp = (mp_ptr) TMP_ALLOC ((size_t) (xn + 1) * BYTES_PER_MP_LIMB);

  yp[xn] = mpn_mul_1 (yp, MPFR_MANT(x), xn, u);

  /* x * u is stored in yp[xn], ..., yp[0] */

  /* since the case u=1 was treated above, we have u >= 2, thus
     yp[xn] >= 1 since x was msb-normalized */
  MPFR_ASSERTN(yp[xn] != 0);
  if ((yp[xn] & GMP_LIMB_HIGHBIT) == 0)
    {
      count_leading_zeros(cnt, yp[xn]);
      mpn_lshift (yp, yp, xn + 1, cnt);
    }
  else
    {
      cnt = 0;
    }

  /* now yp[xn], ..., yp[0] is msb-normalized too, and has at most
     PREC(x) + (BITS_PER_MP_LIMB - cnt) non-zero bits */

  c = mpfr_round_raw (old_yp, yp, (mp_prec_t) (xn + 1) * BITS_PER_MP_LIMB,
		      (MPFR_SIGN(x) < 0), MPFR_PREC(y), rnd_mode, &inexact);

  cnt = BITS_PER_MP_LIMB - cnt;

  if (c) /* rounded result is 1.0000000000000000... */
    {
      old_yp[yn-1] = GMP_LIMB_HIGHBIT;
      cnt++;
    }

  TMP_FREE(marker);

  if (__mpfr_emax < MPFR_EMAX_MIN + cnt || MPFR_EXP(x) > __mpfr_emax - cnt)
    return mpfr_set_overflow(y, rnd_mode, MPFR_SIGN(x));

  MPFR_EXP(y) = MPFR_EXP(x) + cnt;
  MPFR_SET_SAME_SIGN(y, x);

  return inexact;
}
