/* mpfr_round_raw_generic, mpfr_round_raw2, mpfr_round_raw, mpfr_round,
   mpfr_can_round, mpfr_can_round_raw -- various rounding functions

Copyright (C) 1999, 2001 Free Software Foundation, Inc.

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

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

#if (BITS_PER_MP_LIMB & (BITS_PER_MP_LIMB - 1))
#error "BITS_PER_MP_LIMB must be a power of 2"
#endif

/* 
   If flag = 0, puts in y the value of xp (with precision xprec and
   sign 1 if negative=0, -1 otherwise) rounded to precision yprec and
   direction rnd_mode. Supposes x is not zero nor NaN nor +/- Infinity
   (i.e. *xp != 0). If inexp != NULL, computes the inexact flag of the
   rounding. 

   In case of even rounding when rnd = GMP_RNDN, returns 2 or -2. 

   If flag = 1, just returns whether one should add 1 or not for rounding.   
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
mpfr_round (mpfr_ptr x, mp_rnd_t rnd_mode, mp_prec_t prec)
{
  mp_limb_t *tmp;
  int carry, neg, inexact;
  mp_prec_t nw;
  TMP_DECL(marker);

  if (MPFR_IS_NAN(x))
    MPFR_RET_NAN;

  if (MPFR_IS_INF(x))
    return 0; /* infinity is exact */

  nw = 1 + (prec - 1) / BITS_PER_MP_LIMB; /* needed allocated limbs */
  neg = MPFR_SIGN(x) < 0;

  /* check if x has enough allocated space for the mantissa */
  if (nw > MPFR_ABSSIZE(x)) {
    MPFR_MANT(x) = (mp_ptr) (*__gmp_reallocate_func) 
      (MPFR_MANT(x), MPFR_ABSSIZE(x)*BYTES_PER_MP_LIMB, nw * BYTES_PER_MP_LIMB);
    MPFR_SIZE(x) = nw; /* new number of allocated limbs */
    if (neg)
      MPFR_CHANGE_SIGN(x);
  }

  TMP_MARK(marker); 
  tmp = TMP_ALLOC (nw * BYTES_PER_MP_LIMB);
  carry = mpfr_round_raw (tmp, MPFR_MANT(x), MPFR_PREC(x), neg, prec, rnd_mode,
                          &inexact);

  if (carry)
    {
      /* Is a shift necessary here? Isn't the result 1.0000...? */
      mpn_rshift (tmp, tmp, nw, 1);
      tmp [nw-1] |= MP_LIMB_T_HIGHBIT;
      MPFR_EXP(x)++;
    }

  MPFR_PREC(x) = prec;
  MPN_COPY(MPFR_MANT(x), tmp, nw);
  TMP_FREE(marker);

  return inexact;
}

/* assumptions:
   (i) BITS_PER_MP_LIMB is a power of 2
   (ii) in a test, the left part of the && is evaluated first */

/* assuming b is an approximation of x in direction rnd1 
   with error at most 2^(MPFR_EXP(b)-err), returns 1 if one is 
   able to round exactly x to precision prec with direction rnd2,
   and 0 otherwise.

   Side effects: none.
*/

int
mpfr_can_round (mpfr_ptr b, mp_prec_t err, mp_rnd_t rnd1,
		mp_rnd_t rnd2, mp_prec_t prec)
{
  return (MPFR_IS_ZERO(b)) ? 0 : /* we cannot round if b=0 */
    mpfr_can_round_raw (MPFR_MANT(b),
			(MPFR_PREC(b) - 1)/BITS_PER_MP_LIMB + 1,
			MPFR_SIGN(b), err, rnd1, rnd2, prec);
}

int
mpfr_can_round_raw (mp_limb_t *bp, mp_prec_t bn, int neg, mp_prec_t err,
		    mp_rnd_t rnd1, mp_rnd_t rnd2, mp_prec_t prec)
{
  int k, k1, l, l1, tn;
  mp_limb_t cc, cc2, *tmp;
  TMP_DECL(marker);

  if (err <= prec)
    return 0;
  neg = (neg > 0 ? 0 : 1);

  /* if the error is smaller than ulp(b), then anyway it will propagate
     up to ulp(b) */
  if (err > bn * BITS_PER_MP_LIMB)
    err = bn * BITS_PER_MP_LIMB;

  /* warning: if k = m*BITS_PER_MP_LIMB, consider limb m-1 and not m */
  k = (err - 1) / BITS_PER_MP_LIMB;
  l = err % BITS_PER_MP_LIMB;
  if (l)
    l = BITS_PER_MP_LIMB - l;
  /* the error corresponds to bit l in limb k, the most significant limb
   being limb 0 */
  k1 = (prec - 1) / BITS_PER_MP_LIMB;
  l1 = prec % BITS_PER_MP_LIMB;
  if (l1)
    l1 = BITS_PER_MP_LIMB - l1;

  /* the last significant bit is bit l1 in limb k1 */

  /* don't need to consider the k1 most significant limbs */
  k -= k1;
  bn -= k1;
  prec -= k1 * BITS_PER_MP_LIMB;
  k1=0;
  /* if when adding or subtracting (1 << l) in bp[bn-1-k], it does not
     change bp[bn-1] >> l1, then we can round */

  if (rnd1 == GMP_RNDU)
    if (neg)
      rnd1 = GMP_RNDZ;
  if (rnd1 == GMP_RNDD)
    rnd1 = (neg) ? GMP_RNDU : GMP_RNDZ;

  /* in the sequel, RNDU = towards infinity, RNDZ = towards zero */

  TMP_MARK(marker);
  tn = bn;
  k++; /* since we work with k+1 everywhere */

  switch (rnd1)
    {
    case GMP_RNDZ: /* b <= x <= b+2^(MPFR_EXP(b)-err) */
      tmp = TMP_ALLOC(tn * BYTES_PER_MP_LIMB); 
      cc = (bp[bn-1] >> l1) & 1;
      cc ^= mpfr_round_raw2(bp, bn, neg, rnd2, prec);

    /* now round b+2^(MPFR_EXP(b)-err) */
      if (bn > k)
        MPN_COPY (tmp, bp, bn - k);
      cc2 = mpn_add_1 (tmp+bn-k, bp+bn-k, k, MP_LIMB_T_ONE << l);
      /* if cc2=1, then all bits up to err were to 1, and we can round only
         if cc==0 and mpfr_round_raw2 returns 0 below */
      if (cc2 && cc)
        {
          TMP_FREE(marker);
          return 0;
        }
      cc2 = (tmp[bn-1]>>l1) & 1; /* gives 0 when carry */
      cc2 ^= mpfr_round_raw2(tmp, bn, neg, rnd2, prec);

    TMP_FREE(marker);
    return (cc == cc2);

    case GMP_RNDU: /* b-2^(MPFR_EXP(b)-err) <= x <= b */
      tmp = TMP_ALLOC(tn * BYTES_PER_MP_LIMB); 
      /* first round b */
      cc = (bp[bn-1]>>l1) & 1;
      cc ^= mpfr_round_raw2(bp, bn, neg, rnd2, prec);

      /* now round b-2^(MPFR_EXP(b)-err) */
      if (bn > k)
        MPN_COPY (tmp, bp, bn - k);
      cc2 = mpn_sub_1(tmp+bn-k, bp+bn-k, k, MP_LIMB_T_ONE << l);
      /* if cc2=1, then all bits up to err were to 0, and we can round only
         if cc==0 and mpfr_round_raw2 returns 1 below */
      if (cc2 && cc)
        {
          TMP_FREE(marker);
          return 0;
        }
      cc2 = (tmp[bn-1]>>l1) & 1; /* gives 1 when carry */
      cc2 ^= mpfr_round_raw2(tmp, bn, neg, rnd2, prec);

      TMP_FREE(marker);
      return (cc == cc2);

    case GMP_RNDN: /* b-2^(MPFR_EXP(b)-err) <= x <= b+2^(MPFR_EXP(b)-err) */
      tmp = TMP_ALLOC(tn * BYTES_PER_MP_LIMB);

      if (bn > k)
        MPN_COPY (tmp, bp, bn - k);
      /* first round b+2^(MPFR_EXP(b)-err) */
      cc = mpn_add_1 (tmp + bn - k, bp + bn - k, k, MP_LIMB_T_ONE << l);
      cc = (tmp[bn - 1] >> l1) & 1; /* gives 0 when cc=1 */
      cc ^= mpfr_round_raw2 (tmp, bn, neg, rnd2, prec);

      /* now round b-2^(MPFR_EXP(b)-err) */
      cc2 = mpn_sub_1 (tmp + bn - k, bp + bn - k, k, MP_LIMB_T_ONE << l);
      /* if cc2=1, then all bits up to err were to 0, and we can round only
         if cc==0 and mpfr_round_raw2 returns 1 below */
      if (cc2 && cc)
        {
          TMP_FREE(marker);
          return 0;
        }
      cc2 = (tmp[bn - 1] >> l1) & 1; /* gives 1 when cc2=1 */
      cc2 ^= mpfr_round_raw2 (tmp, bn, neg, rnd2, prec);

      TMP_FREE(marker); 
      return (cc == cc2);
    }
  return 0;
}
