/* mpfr_rint -- Round to an integer.

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
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_rint (mpfr_ptr r, mpfr_srcptr u, mp_rnd_t rnd_mode)
{
  int sign;
  mp_exp_t exp;

  if (MPFR_IS_NAN(u))
    {
      MPFR_SET_NAN(r);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(r);
  MPFR_SET_SAME_SIGN(r, u);

  if (MPFR_IS_INF(u))
    {
      MPFR_SET_INF(r);
      MPFR_RET(0);  /* infinity is exact */
    }

  MPFR_CLEAR_INF(r);

  if (MPFR_IS_ZERO(u))
    {
      MPFR_SET_ZERO(r);
      MPFR_RET(0);  /* zero is exact */
    }

  sign = MPFR_SIGN(u);
  exp = MPFR_EXP(u);

  /* Single out the case where |u| < 1. */
  if (exp <= 0)  /* 0 < |u| < 1 */
    {
      if ((rnd_mode == GMP_RNDD && sign < 0) ||
          (rnd_mode == GMP_RNDU && sign > 0) ||
          (rnd_mode == GMP_RNDN && exp == 0))
        {
          mp_limb_t *rp;
          mp_size_t rm;

          rp = MPFR_MANT(r);
          rm = (MPFR_PREC(r) - 1) / BITS_PER_MP_LIMB;
          rp[rm] = GMP_LIMB_HIGHBIT;
          MPN_ZERO(rp, rm);
          MPFR_EXP(r) = 1;  /* |r| = 1 */
          MPFR_RET(sign > 0 ? 2 : -2);
        }
      else
        {
          MPFR_SET_ZERO(r);  /* r = 0 */
          MPFR_RET(sign > 0 ? -2 : 2);
        }
    }
  else  /* exp > 0, |u| >= 1 */
    {
      mp_limb_t *up, *rp;
      mp_size_t un, rn, ui;
      int sh, idiff;
      int uflags;

      /*
       * uflags will contain:
       *   _ 0 if u is an integer representable in r,
       *   _ 1 if u is an integer not representable in r,
       *   _ 2 if u is not an integer.
       */

      up = MPFR_MANT(u);
      rp = MPFR_MANT(r);

      un = MPFR_ESIZE(u);
      rn = MPFR_ESIZE(r);
      sh = rn * BITS_PER_MP_LIMB - MPFR_PREC(r);

      MPFR_EXP(r) = exp;

      if ((exp - 1) / BITS_PER_MP_LIMB >= un)
        {
          ui = un;
          idiff = 0;
          uflags = 0;  /* u is an integer */
        }
      else
        {
          mp_size_t uj;

          ui = (exp - 1) / BITS_PER_MP_LIMB + 1;  /* #limbs of the int part */
          uj = un - ui;  /* lowest limb of the integer part */
          idiff = exp % BITS_PER_MP_LIMB;  /* #int-part bits in up[uj] or 0 */

          uflags = idiff == 0 || (up[uj] << idiff) == 0 ? 0 : 2;
          if (uflags == 0)
            while (uj > 0)
              if (up[--uj] != 0)
                {
                  uflags = 2;
                  break;
                }
        }

      if (ui > rn)
        {
          MPFR_ASSERTN(rp != up && un > rn);
          MPN_COPY(rp, up + (un - rn), rn);
          /* In the rounding to the nearest mode, if the rounding bit
             is 0, change the rounding mode to GMP_RNDZ. */
          if (rnd_mode == GMP_RNDN &&
              ((sh != 0 && (rp[0] & (MP_LIMB_T_ONE << (sh - 1))) == 0) ||
               (sh == 0 && (up[un - rn - 1] & GMP_LIMB_HIGHBIT) == 0)))
            rnd_mode = GMP_RNDZ;
          if (uflags == 0)
            { /* u is an integer; determine if it is representable */
              if (sh != 0 && rp[0] << (BITS_PER_MP_LIMB - sh) != 0)
                uflags = 1;  /* u is not representable in r */
              else
                {
                  mp_size_t i;
                  for (i = un - rn - 1; i >= 0; i--)
                    if (up[i] != 0)
                      {
                        uflags = 1;  /* u is not representable in r */
                        break;
                      }
                }
            }
          if (sh != 0)
            rp[0] &= MP_LIMB_T_MAX << sh;
        }
      else
        {
          mp_size_t uj, rj;
          int ush;

          uj = un - ui;  /* lowest limb of the integer part in u */
          rj = rn - ui;  /* lowest limb of the integer part in r */

          MPN_ZERO(rp, rj);

          if (rp != up)
            MPN_COPY(rp + rj, up + uj, ui);

          rp += rj;
          rn = ui;

          ush = idiff == 0 ? 0 : BITS_PER_MP_LIMB - idiff;
          if (rj == 0 && ush < sh)
            {
              /* In the rounding to the nearest mode, if the rounding bit
                 is 0, change the rounding mode to GMP_RNDZ. */
              if (rnd_mode == GMP_RNDN &&
                  (rp[0] & (MP_LIMB_T_ONE << (sh - 1))) == 0)
                rnd_mode = GMP_RNDZ;  /* rounding bit is 0 */
              if (uflags == 0)
                { /* u is an integer; determine if it is representable */
                  mp_limb_t mask;
                  mask = (MP_LIMB_T_ONE << sh) - (MP_LIMB_T_ONE << ush);
                  if ((rp[0] & mask) != 0)
                    uflags = 1;  /* u is not representable in r */
                }
            }
          else
            {
              sh = ush;
              if (rnd_mode == GMP_RNDN &&
                  ((ush != 0 &&
                    (up[uj] & (MP_LIMB_T_ONE << (ush - 1))) == 0) ||
                   (ush == 0 &&
                    (uj == 0 || (up[uj - 1] & GMP_LIMB_HIGHBIT) == 0))))
                rnd_mode = GMP_RNDZ;  /* rounding bit is 0 */
            }
          if (sh != 0)
            rp[0] &= MP_LIMB_T_MAX << sh;
        }

      if (uflags == 0)
        MPFR_RET(0);

      /* Note: if rnd_mode == GMP_RNDN, then round away from 0 (if
         the rounding bit was 0 and rnd_mode == GMP_RNDN, rnd_mode
         has been changed to GMP_RNDZ). */

      if ((rnd_mode == GMP_RNDN ||
           (rnd_mode == GMP_RNDD && sign < 0) ||
           (rnd_mode == GMP_RNDU && sign > 0))
          && mpn_add_1(rp, rp, rn, MP_LIMB_T_ONE << sh))
        {
          if (exp == __mpfr_emax)
            return mpfr_set_overflow(r, rnd_mode, MPFR_SIGN(r)) >= 0 ?
              uflags : -uflags;
          else
            {
              MPFR_EXP(r)++;
              rp[rn-1] = GMP_LIMB_HIGHBIT;
            }
        }

      MPFR_RET(rnd_mode == GMP_RNDU ||
               (rnd_mode == GMP_RNDZ && sign < 0) ||
               (rnd_mode == GMP_RNDN && sign > 0) ? uflags : -uflags);
    }  /* exp > 0, |u| >= 1 */
}

#undef mpfr_round

int
mpfr_round (mpfr_ptr r, mpfr_srcptr u)
{
  return mpfr_rint(r, u, GMP_RNDN);
}

#undef mpfr_trunc

int
mpfr_trunc (mpfr_ptr r, mpfr_srcptr u)
{
  return mpfr_rint(r, u, GMP_RNDZ);
}

#undef mpfr_ceil

int
mpfr_ceil (mpfr_ptr r, mpfr_srcptr u)
{
  return mpfr_rint(r, u, GMP_RNDU);
}

#undef mpfr_floor

int
mpfr_floor (mpfr_ptr r, mpfr_srcptr u)
{
  return mpfr_rint(r, u, GMP_RNDD);
}
