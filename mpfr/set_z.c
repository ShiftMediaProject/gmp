/* mpfr_set_z -- set a floating-point number from a multiple-precision integer

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

/* set f to the integer z */
int 
mpfr_set_z (mpfr_ptr f, mpz_srcptr z, mp_rnd_t rnd_mode)
{
  mp_size_t fn, zn, dif;
  int k, sign_z, inex;
  mp_limb_t *fp, *zp;
  mp_exp_t exp;

  MPFR_CLEAR_FLAGS (f); /* z cannot be NaN nor Inf */

  sign_z = mpz_cmp_ui (z, 0);

  if (sign_z == 0)
    {
      MPFR_SET_ZERO(f);
      MPFR_SET_POS(f);
      MPFR_RET(0);
    }

  fp = MPFR_MANT(f);
  fn = 1 + (MPFR_PREC(f) - 1) / BITS_PER_MP_LIMB;
  zn = ABS(SIZ(z));
  dif = zn - fn;
  zp = PTR(z);
  count_leading_zeros(k, zp[zn-1]);

  exp = (mp_prec_t) zn * BITS_PER_MP_LIMB - k;
  /* The exponent will be exp or exp + 1 (due to rounding) */
  if (exp > __mpfr_emax)
    return mpfr_set_overflow(f, rnd_mode, sign_z);
  if (exp + 1 < __mpfr_emin)
    return mpfr_set_underflow(f, rnd_mode, sign_z);

  if (MPFR_SIGN(f) * sign_z < 0)
    MPFR_CHANGE_SIGN(f);

  if (dif >= 0)
    {
      mp_limb_t cc;
      int sh;

      /* number has to be truncated */
      if (k != 0)
        {
          mpn_lshift(fp, zp + dif, fn, k);
          if (dif != 0)
            fp[0] += zp[dif - 1] >> (BITS_PER_MP_LIMB - k);
        }
      else
        MPN_COPY(fp, zp + dif, fn);

      sh = (mp_prec_t) fn * BITS_PER_MP_LIMB - MPFR_PREC(f);
      cc = fp[0] & ((MP_LIMB_T_ONE << sh) - 1);
      fp[0] &= ~cc;

      if ((rnd_mode == GMP_RNDU && sign_z < 0) ||
          (rnd_mode == GMP_RNDD && sign_z > 0))
        rnd_mode = GMP_RNDZ;

      /* remaining bits... */
      if (rnd_mode == GMP_RNDN)
        {
          /* 1) If rounding bit is 0, behave like rounding to 0.
             2) Determine the sticky bit (cc != 0). */
          if (sh != 0)
            {
              mp_limb_t rb;

              rb = MP_LIMB_T_ONE << (sh - 1);
              if ((cc & rb) == 0)
                rnd_mode = GMP_RNDZ; /* rounding bit is 0 */
              else
                cc &= ~rb;
              if (cc == 0 && dif > 0)
                cc = zp[--dif] << k;
            }
          else /* sh == 0 */
            {
              MPFR_ASSERTN(cc == 0);
              if (dif > 0)
                cc = zp[--dif] << k;
              if ((cc & GMP_LIMB_HIGHBIT) == 0)
                rnd_mode = GMP_RNDZ; /* rounding bit is 0 */
              else
                cc <<= 1;
            }

          while (cc == 0 && dif > 0)
            cc = zp[--dif];

          if (rnd_mode == GMP_RNDN && cc == 0) /* even rounding */
            {
              cc = 1;
              if ((fp[0] & (MP_LIMB_T_ONE << sh)) == 0)
                rnd_mode = GMP_RNDZ;
            }
        } /* rnd_mode == GMP_RNDN */
      else if (cc == 0 && dif > 0)
        {
          cc = zp[--dif] << k;
          while (cc == 0 && dif > 0)
            cc = zp[--dif];
        }

      if (cc == 0)
        inex = 0;
      else if (rnd_mode == GMP_RNDZ)
        inex = -sign_z;
      else
        {
          if (mpn_add_1(fp, fp, fn, MP_LIMB_T_ONE << sh))
            {
              if (exp == __mpfr_emax)
                return mpfr_set_overflow(f, rnd_mode, sign_z);
              else
                {
                  exp++;
                  fp[fn-1] = GMP_LIMB_HIGHBIT;
                }
            }
          inex = sign_z;
        }
    } /* dif >= 0 */
  else /* dif < 0 */
    {
      if (k != 0)
        mpn_lshift(fp - dif, zp, zn, k);
      else
        MPN_COPY(fp - dif, zp, zn);
      /* fill with zeroes */
      MPN_ZERO(fp, -dif);
      inex = 0; /* result is exact */
    }

  if (exp < __mpfr_emin)
    return mpfr_set_underflow(f, rnd_mode, sign_z);
  MPFR_EXP(f) = exp;
  MPFR_RET(inex);
}
