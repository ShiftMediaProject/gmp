/* mpfr_round_raw_generic, mpfr_round_raw2, mpfr_round_raw, mpfr_round_prec,
   mpfr_can_round, mpfr_can_round_raw -- various rounding functions

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

#if (BITS_PER_MP_LIMB & (BITS_PER_MP_LIMB - 1))
#error "BITS_PER_MP_LIMB must be a power of 2"
#endif

/*
 * If flag = 0, puts in y the value of xp (with precision xprec and
 * sign 1 if negative=0, -1 otherwise) rounded to precision yprec and
 * direction rnd_mode. Supposes x is not zero nor NaN nor +/- Infinity
 * (i.e. *xp != 0). If inexp != NULL, computes the inexact flag of the
 * rounding.
 *
 * In case of even rounding when rnd = GMP_RNDN, returns 2 or -2.
 *
 * If flag = 1, just returns whether one should add 1 or not for rounding.
 *
 * Note: yprec may be < MPFR_PREC_MIN; in particular, it may be equal
 * to 1. In this case, the even rounding is done away from 0, which is
 * a natural generalization. Indeed, a number with 1-bit precision can
 * be seen as a denormalized number with more precision.
 */

int
mpfr_round_raw_generic(mp_limb_t *yp, mp_limb_t *xp, mp_prec_t xprec,
                       int neg, mp_prec_t yprec, mp_rnd_t rnd_mode,
                       int *inexp, int flag)
{
  mp_size_t xsize, nw;
  mp_limb_t himask, lomask;
  int rw, carry = 0;

  xsize = (xprec-1)/BITS_PER_MP_LIMB + 1;

  nw = yprec / BITS_PER_MP_LIMB;
  rw = yprec & (BITS_PER_MP_LIMB - 1);

  if (flag && !inexp && (rnd_mode==GMP_RNDZ || xprec <= yprec 
			 || (rnd_mode==GMP_RNDU && neg)
			 || (rnd_mode==GMP_RNDD && neg==0))) 
    return 0; 

  if (rw)
    {
      nw++;
      lomask = ((MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - rw)) - MP_LIMB_T_ONE);
      himask = ~lomask;
    }
  else
    {
      lomask = -1;
      himask = -1;
    }
  MPFR_ASSERTN(nw >= 1);

  if (xprec <= yprec)
    { /* No rounding is necessary. */
      /* if yp=xp, maybe an overlap: MPN_COPY_DECR is ok when src <= dst */
      MPFR_ASSERTN(nw >= xsize);
      if (inexp)
        *inexp = 0;
      if (flag)
        return 0; 
      MPN_COPY_DECR(yp + (nw - xsize), xp, xsize);
      MPN_ZERO(yp, nw - xsize);
    }
  else
    {
      mp_limb_t sb;

      if ((rnd_mode == GMP_RNDU && neg) ||
          (rnd_mode == GMP_RNDD && !neg))
        rnd_mode = GMP_RNDZ;

      if (inexp || rnd_mode != GMP_RNDZ)
        {
          mp_size_t k;

          k = xsize - nw;
          if (!rw)
            k--;
          MPFR_ASSERTN(k >= 0);
          sb = xp[k] & lomask;  /* First non-significant bits */
          if (rnd_mode == GMP_RNDN)
            {
              mp_limb_t rbmask = MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - rw - 1);
              if (sb & rbmask) /* rounding bit */
                sb &= ~rbmask; /* it is 1, clear it */
              else
                rnd_mode = GMP_RNDZ; /* it is 0, behave like rounding to 0 */
            }
          while (sb == 0 && k > 0)
            sb = xp[--k];
          if (rnd_mode == GMP_RNDN)
            { /* rounding to nearest, with rounding bit = 1 */
              if (sb == 0) /* Even rounding. */
                {
                  sb = xp[xsize - nw] & (himask ^ (himask << 1));
                  if (inexp)
                    *inexp = ((neg != 0) ^ (sb != 0))
                      ? MPFR_EVEN_INEX  : -MPFR_EVEN_INEX;
                }
              else /* sb != 0 */
                {
                  if (inexp)
                    *inexp = (neg == 0) ? 1 : -1;
                }
            }
          else if (inexp)
            *inexp = sb == 0 ? 0
              : (((neg != 0) ^ (rnd_mode != GMP_RNDZ)) ? 1 : -1);
        }
      else
        sb = 0;

    if (flag)
      return sb != 0 && rnd_mode != GMP_RNDZ;

    if (sb != 0 && rnd_mode != GMP_RNDZ)
      carry = mpn_add_1(yp, xp + xsize - nw, nw,
                        rw ? MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - rw) : 1);
    else
      MPN_COPY_INCR(yp, xp + xsize - nw, nw);

    yp[0] &= himask;
  }

  return carry;
}

int
mpfr_round_prec (mpfr_ptr x, mp_rnd_t rnd_mode, mp_prec_t prec)
{
  mp_limb_t *tmp, *xp;
  int carry, inexact;
  mp_prec_t nw;
  TMP_DECL(marker);

  MPFR_ASSERTN(prec >= MPFR_PREC_MIN && prec <= MPFR_PREC_MAX);

  nw = 1 + (prec - 1) / BITS_PER_MP_LIMB; /* needed allocated limbs */

  /* check if x has enough allocated space for the mantissa */
  if (nw > MPFR_ABSSIZE(x))
    {
      MPFR_MANT(x) = (mp_ptr) (*__gmp_reallocate_func)
        (MPFR_MANT(x), (size_t) MPFR_ABSSIZE(x) * BYTES_PER_MP_LIMB,
         (size_t) nw * BYTES_PER_MP_LIMB);
      MPFR_SET_ABSSIZE(x, nw); /* new number of allocated limbs */
    }

  if (MPFR_IS_NAN(x))
    MPFR_RET_NAN;

  if (MPFR_IS_INF(x))
    return 0; /* infinity is exact */

  /* x is a real number */

  TMP_MARK(marker); 
  tmp = TMP_ALLOC (nw * BYTES_PER_MP_LIMB);
  xp = MPFR_MANT(x);
  carry = mpfr_round_raw (tmp, xp, MPFR_PREC(x), MPFR_SIGN(x) < 0,
                          prec, rnd_mode, &inexact);
  MPFR_PREC(x) = prec;

  if (carry)
    {
      mp_exp_t exp = MPFR_EXP(x);

      if (exp == __mpfr_emax)
        (void) mpfr_set_overflow(x, rnd_mode, MPFR_SIGN(x));
      else
        {
          MPFR_EXP(x)++;
          xp[nw - 1] = GMP_LIMB_HIGHBIT;
          if (nw - 1 > 0)
            MPN_ZERO(xp, nw - 1);
        }
    }
  else
    MPN_COPY(xp, tmp, nw);

  TMP_FREE(marker);
  return inexact;
}

/* assumption: BITS_PER_MP_LIMB is a power of 2 */

/* assuming b is an approximation of x in direction rnd1 
   with error at most 2^(MPFR_EXP(b)-err), returns 1 if one is 
   able to round exactly x to precision prec with direction rnd2,
   and 0 otherwise.

   Side effects: none.
*/

int
mpfr_can_round (mpfr_ptr b, mp_exp_t err, mp_rnd_t rnd1,
		mp_rnd_t rnd2, mp_prec_t prec)
{
  return MPFR_IS_ZERO(b) ? 0 : /* we cannot round if b=0 */
    mpfr_can_round_raw (MPFR_MANT(b),
			(MPFR_PREC(b) - 1)/BITS_PER_MP_LIMB + 1,
			MPFR_SIGN(b), err, rnd1, rnd2, prec);
}

int
mpfr_can_round_raw (mp_limb_t *bp, mp_size_t bn, int neg, mp_exp_t err0,
                    mp_rnd_t rnd1, mp_rnd_t rnd2, mp_prec_t prec)
{
  mp_prec_t err;
  mp_size_t k, k1, tn;
  int s, s1;
  mp_limb_t cc, cc2;
  mp_limb_t *tmp;
  TMP_DECL(marker);

  if (err0 < 0 || (mp_exp_unsigned_t) err0 <= prec)
    return 0;  /* can't round */

  neg = neg <= 0;

  /* if the error is smaller than ulp(b), then anyway it will propagate
     up to ulp(b) */
  err = ((mp_exp_unsigned_t) err0 > (mp_prec_t) bn * BITS_PER_MP_LIMB) ?
    (mp_prec_t) bn * BITS_PER_MP_LIMB : err0;

  /* warning: if k = m*BITS_PER_MP_LIMB, consider limb m-1 and not m */
  k = (err - 1) / BITS_PER_MP_LIMB;
  s = err % BITS_PER_MP_LIMB;
  if (s)
    s = BITS_PER_MP_LIMB - s;
  /* the error corresponds to bit s in limb k, the most significant limb
     being limb 0 */
  k1 = (prec - 1) / BITS_PER_MP_LIMB;
  s1 = prec % BITS_PER_MP_LIMB;
  if (s1)
    s1 = BITS_PER_MP_LIMB - s1;

  /* the last significant bit is bit s1 in limb k1 */

  /* don't need to consider the k1 most significant limbs */
  k -= k1;
  bn -= k1;
  prec -= (mp_prec_t) k1 * BITS_PER_MP_LIMB;
  /* if when adding or subtracting (1 << s) in bp[bn-1-k], it does not
     change bp[bn-1] >> s1, then we can round */

  if (rnd1 == GMP_RNDU)
    if (neg)
      rnd1 = GMP_RNDZ;

  if (rnd1 == GMP_RNDD)
    rnd1 = neg ? GMP_RNDU : GMP_RNDZ;

  /* in the sequel, RNDU = towards infinity, RNDZ = towards zero */

  TMP_MARK(marker);
  tn = bn;
  k++; /* since we work with k+1 everywhere */
  tmp = TMP_ALLOC(tn * BYTES_PER_MP_LIMB);
  if (bn > k)
    MPN_COPY (tmp, bp, bn - k);

  if (rnd1 != GMP_RNDN)
    { /* GMP_RNDZ or GMP_RNDU */
      cc = (bp[bn - 1] >> s1) & 1;
      cc ^= mpfr_round_raw2(bp, bn, neg, rnd2, prec);

      /* now round b +/- 2^(MPFR_EXP(b)-err) */
      cc2 = rnd1 == GMP_RNDZ ?
        mpn_add_1 (tmp + bn - k, bp + bn - k, k, MP_LIMB_T_ONE << s) :
        mpn_sub_1 (tmp + bn - k, bp + bn - k, k, MP_LIMB_T_ONE << s);
    }
  else
    { /* GMP_RNDN */
      /* first round b+2^(MPFR_EXP(b)-err) */
      cc = mpn_add_1 (tmp + bn - k, bp + bn - k, k, MP_LIMB_T_ONE << s);
      cc = (tmp[bn - 1] >> s1) & 1; /* gives 0 when cc=1 */
      cc ^= mpfr_round_raw2 (tmp, bn, neg, rnd2, prec);

      /* now round b-2^(MPFR_EXP(b)-err) */
      cc2 = mpn_sub_1 (tmp + bn - k, bp + bn - k, k, MP_LIMB_T_ONE << s);
    }

  if (cc2 && cc)
    {
      TMP_FREE(marker);
      return 0;
    }

  cc2 = (tmp[bn - 1] >> s1) & 1;
  cc2 ^= mpfr_round_raw2 (tmp, bn, neg, rnd2, prec);

  TMP_FREE(marker); 
  return cc == cc2;
}
