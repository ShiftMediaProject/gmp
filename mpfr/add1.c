/* mpfr_add1 -- internal function to perform a "real" addition

Copyright 1999, 2000, 2001 Free Software Foundation.
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

/* compute sign(b) * (|b| + |c|)
   diff_exp is the difference between the exponents of b and c,
   which is >= 0 */

int
mpfr_add1 (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c,
	   mp_rnd_t rnd_mode, mp_exp_unsigned_t diff_exp)
{
  mp_limb_t *ap, *bp, *cp;
  mp_prec_t aq, bq, cq, aq2;
  mp_size_t an, bn, cn;
  mp_exp_t difw;
  int sh, rb, fb, inex;
  TMP_DECL(marker);

  MPFR_ASSERTN(MPFR_IS_FP(b) && MPFR_NOTZERO(b));
  MPFR_ASSERTN(MPFR_IS_FP(c) && MPFR_NOTZERO(c));

  TMP_MARK(marker);
  ap = MPFR_MANT(a);
  bp = MPFR_MANT(b);
  cp = MPFR_MANT(c);

  if (ap == bp)
    {
      bp = (mp_ptr) TMP_ALLOC((size_t) MPFR_ABSSIZE(b) * BYTES_PER_MP_LIMB);
      MPN_COPY (bp, ap, MPFR_ABSSIZE(b));
      if (ap == cp)
        { cp = bp; }
    }
  else if (ap == cp)
    {
      cp = (mp_ptr) TMP_ALLOC ((size_t) MPFR_ABSSIZE(c) * BYTES_PER_MP_LIMB);
      MPN_COPY(cp, ap, MPFR_ABSSIZE(c));
    }

  aq = MPFR_PREC(a);
  bq = MPFR_PREC(b);
  cq = MPFR_PREC(c);

  an = (aq-1)/BITS_PER_MP_LIMB + 1; /* number of significant limbs of a */
  aq2 = an * BITS_PER_MP_LIMB;
  sh = aq2 - aq;                    /* non-significant bits in low limb */
  bn = (bq-1)/BITS_PER_MP_LIMB + 1; /* number of significant limbs of b */
  cn = (cq-1)/BITS_PER_MP_LIMB + 1; /* number of significant limbs of c */

  MPFR_EXP(a) = MPFR_EXP(b);
  MPFR_SET_SAME_SIGN(a, b);

  /*
   * 1. Compute the significant part A', the non-significant bits of A
   * are taken into account.
   *
   * 2. Perform the rounding. At each iteration, we remember:
   *     _ r = rounding bit
   *     _ f = following bits (same value)
   * where the result has the form: [number A]rfff...fff + a remaining
   * value in the interval [0,2) ulp. We consider the most significant
   * bits of the remaining value to update the result; a possible carry
   * is immediately taken into account and A is updated accordingly. As
   * soon as the bits f don't have the same value, A can be rounded.
   * Variables:
   *     _ rb = rounding bit (0 or 1).
   *     _ fb = following bits (0 or 1), then sticky bit.
   * If fb == 0, the only thing that can change is the sticky bit.
   */

  rb = fb = -1; /* means: not initialized */

  if (aq2 <= diff_exp)
    { /* c does not overlap with a' */
      if (an > bn)
        { /* a has more limbs than b */
          /* copy b to the most significant limbs of a */
          MPN_COPY(ap + (an - bn), bp, bn);
          /* zero the least significant limbs of a */
          MPN_ZERO(ap, an - bn);
        }
      else /* an <= bn */
        {
          /* copy the most significant limbs of b to a */
          MPN_COPY(ap, bp + (bn - an), an);
        }
    }
  else /* aq2 > diff_exp */
    { /* c overlaps with a' */
      mp_limb_t *a2p;
      mp_limb_t cc;
      mp_prec_t dif;
      mp_size_t difn, k;
      int shift;

      /* copy c (shifted) into a */

      dif = aq2 - diff_exp;
      /* dif is the number of bits of c which overlap with a' */

      difn = (dif-1)/BITS_PER_MP_LIMB + 1;
      /* only the highest difn limbs from c have to be considered */
      if (difn > cn)
        {
          /* c doesn't have enough limbs; take into account the virtual
             zero limbs now by zeroing the least significant limbs of a' */
          MPFR_ASSERTN(difn - cn <= an);
          MPN_ZERO(ap, difn - cn);
          difn = cn;
        }

      k = diff_exp / BITS_PER_MP_LIMB;

      /* zero the most significant k limbs of a */
      a2p = ap + (an - k);
      MPN_ZERO(a2p, k);

      shift = diff_exp % BITS_PER_MP_LIMB;

      if (shift)
        {
          MPFR_ASSERTN(a2p - difn >= ap);
          cc = mpn_rshift(a2p - difn, cp + (cn - difn), difn, shift);
          if (a2p - difn > ap)
            *(a2p - difn - 1) = cc;
        }
      else
        MPN_COPY(a2p - difn, cp + (cn - difn), difn);

      /* add b to a */

      cc = an > bn
        ? mpn_add_n(ap + (an - bn), ap + (an - bn), bp, bn)
        : mpn_add_n(ap, ap, bp + (bn - an), an);

      if (cc) /* carry */
        {
          mp_exp_t exp = MPFR_EXP(a);
          if (exp == __mpfr_emax)
            {
              inex = mpfr_set_overflow(a, rnd_mode, MPFR_SIGN(a));
              goto end_of_add;
            }
          MPFR_EXP(a)++;
          rb = (ap[0] >> sh) & 1; /* LSB(a) --> rounding bit after the shift */
          if (sh)
            {
              mp_limb_t mask, bb;

              mask = (MP_LIMB_T_ONE << sh) - 1;
              bb = ap[0] & mask;
              ap[0] &= (~mask) << 1;
              if (bb == 0)
                fb = 0;
              else if (bb == mask)
                fb = 1;
            }
          mpn_rshift(ap, ap, an, 1);
          ap[an-1] += GMP_LIMB_HIGHBIT;
          if (sh && fb < 0)
            goto rounding;
        } /* cc */
    } /* aq2 > diff_exp */

  /* non-significant bits of a */
  if (rb < 0 && sh)
    {
      mp_limb_t mask, bb;

      mask = (MP_LIMB_T_ONE << sh) - 1;
      bb = ap[0] & mask;
      ap[0] &= ~mask;
      rb = bb >> (sh - 1);
      if (sh > 1)
        {
          mask >>= 1;
          bb &= mask;
          if (bb == 0)
            fb = 0;
          else if (bb == mask)
            fb = 1;
          else
            goto rounding;
        }
    }

  /* determine rounding and sticky bits (and possible carry) */

  difw = (mp_exp_t) an - (mp_exp_t) (diff_exp / BITS_PER_MP_LIMB);
  /* difw is the number of limbs from b (regarded as having an infinite
     precision) that have already been combined with c; -n if the next
     n limbs from b won't be combined with c. */

  if (bn > an)
    { /* there are still limbs from b that haven't been taken into account */
      mp_size_t bk;

      if (fb == 0 && difw <= 0)
        {
          fb = 1; /* c hasn't been taken into account ==> sticky bit != 0 */
          goto rounding;
        }

      bk = bn - an; /* index of lowest considered limb from b, > 0 */
      while (difw < 0)
        { /* ulp(next limb from b) > msb(c) */
          mp_limb_t bb;

          bb = bp[--bk];

          MPFR_ASSERTD(fb != 0);
          if (fb > 0)
            {
              if (bb != MP_LIMB_T_MAX)
                {
                  fb = 1; /* c hasn't been taken into account
                             ==> sticky bit != 0 */
                  goto rounding;
                }
            }
          else /* fb not initialized yet */
            {
              if (rb < 0) /* rb not initialized yet */
                {
                  rb = bb >> (BITS_PER_MP_LIMB - 1);
                  bb |= GMP_LIMB_HIGHBIT;
                }
              fb = 1;
              if (bb != MP_LIMB_T_MAX)
                goto rounding;
            }

          if (bk == 0)
            { /* b has entirely been read */
              fb = 1; /* c hasn't been taken into account
                         ==> sticky bit != 0 */
              goto rounding;
            }

          difw++;
        } /* while */
      MPFR_ASSERTN(bk > 0 && difw >= 0);

      if (difw <= cn)
        {
          mp_size_t ck;
          mp_limb_t cprev;
          int difs;

          ck = cn - difw;
          difs = diff_exp % BITS_PER_MP_LIMB;

          if (difs == 0 && ck == 0)
            goto c_read;

          cprev = ck == cn ? 0 : cp[ck];

          if (fb < 0)
            {
              mp_limb_t bb, cc;

              if (difs)
                {
                  cc = cprev << (BITS_PER_MP_LIMB - difs);
                  if (--ck >= 0)
                    {
                      cprev = cp[ck];
                      cc += cprev >> difs;
                    }
                }
              else
                cc = cp[--ck];

              bb = bp[--bk] + cc;

              if (bb < cc /* carry */
                  && (rb < 0 || (rb ^= 1) == 0)
                  && mpn_add_1(ap, ap, an, MP_LIMB_T_ONE << sh))
                {
                  mp_exp_t exp = MPFR_EXP(a);
                  if (exp == __mpfr_emax)
                    {
                      inex = mpfr_set_overflow(a, rnd_mode, MPFR_SIGN(a));
                      goto end_of_add;
                    }
                  MPFR_EXP(a)++;
                  ap[an-1] = GMP_LIMB_HIGHBIT;
                  rb = 0;
                }

              if (rb < 0) /* rb not initialized yet */
                {
                  rb = bb >> (BITS_PER_MP_LIMB - 1);
                  bb <<= 1;
                  bb |= bb >> (BITS_PER_MP_LIMB - 1);
                }

              fb = bb != 0;
              if (fb && bb != MP_LIMB_T_MAX)
                goto rounding;
            } /* fb < 0 */

          while (bk > 0)
            {
              mp_limb_t bb, cc;

              if (difs)
                {
                  if (ck < 0)
                    goto c_read;
                  cc = cprev << (BITS_PER_MP_LIMB - difs);
                  if (--ck >= 0)
                    {
                      cprev = cp[ck];
                      cc += cprev >> difs;
                    }
                }
              else
                {
                  if (ck == 0)
                    goto c_read;
                  cc = cp[--ck];
                }

              bb = bp[--bk] + cc;
              if (bb < cc) /* carry */
                {
                  fb ^= 1;
                  if (fb)
                    goto rounding;
                  rb ^= 1;
                  if (rb == 0 && mpn_add_1(ap, ap, an, MP_LIMB_T_ONE << sh))
                    {
                      mp_exp_t exp = MPFR_EXP(a);
                      if (exp == __mpfr_emax)
                        {
                          inex = mpfr_set_overflow(a, rnd_mode, MPFR_SIGN(a));
                          goto end_of_add;
                        }
                      MPFR_EXP(a)++;
                      ap[an-1] = GMP_LIMB_HIGHBIT;
                    }
                } /* bb < cc */

              if (!fb && bb != 0)
                {
                  fb = 1;
                  goto rounding;
                }
              if (fb && bb != MP_LIMB_T_MAX)
                goto rounding;
            } /* while */

          /* b has entirely been read */

          if (fb || ck < 0)
            goto rounding;
          if (difs && cprev << (BITS_PER_MP_LIMB - difs))
            {
              fb = 1;
              goto rounding;
            }
          while (ck)
            {
              if (cp[--ck])
                {
                  fb = 1;
                  goto rounding;
                }
            } /* while */
        } /* difw <= cn */
      else
        { /* c has entirely been read */
        c_read:
          if (fb < 0) /* fb not initialized yet */
            {
              mp_limb_t bb;

              MPFR_ASSERTN(bk > 0);
              bb = bp[--bk];
              if (rb < 0) /* rb not initialized yet */
                {
                  rb = bb >> (BITS_PER_MP_LIMB - 1);
                  bb &= ~GMP_LIMB_HIGHBIT;
                }
              fb = bb != 0;
            } /* fb < 0 */
          if (fb)
            goto rounding;
          while (bk)
            {
              if (bp[--bk])
                {
                  fb = 1;
                  goto rounding;
                }
            } /* while */
        } /* difw > cn */
    } /* bn > an */
  else if (fb != 1) /* if fb == 1, the sticky bit is 1 (no possible carry) */
    { /* b has entirely been read */
      if (difw > cn)
        { /* c has entirely been read */
          if (rb < 0)
            rb = 0;
          fb = 0;
        }
      else if (diff_exp > aq2)
        { /* b is followed by at least a zero bit, then by c */
          if (rb < 0)
            rb = 0;
          fb = 1;
        }
      else
        {
          mp_size_t ck;
          int difs;

          MPFR_ASSERTN(difw >= 0 && cn >= difw);
          ck = cn - difw;
          difs = diff_exp % BITS_PER_MP_LIMB;

          if (difs == 0 && ck == 0)
            { /* c has entirely been read */
              if (rb < 0)
                rb = 0;
              fb = 0;
            }
          else
            {
              mp_limb_t cc;

              cc = difs ? (MPFR_ASSERTN(ck < cn),
                           cp[ck] << (BITS_PER_MP_LIMB - difs)) : cp[--ck];
              if (rb < 0)
                {
                  rb = cc >> (BITS_PER_MP_LIMB - 1);
                  cc &= ~GMP_LIMB_HIGHBIT;
                }
              while (cc == 0)
                {
                  if (ck == 0)
                    {
                      fb = 0;
                      goto rounding;
                    }
                  cc = cp[--ck];
                } /* while */
              fb = 1;
            }
        }
    } /* fb != 1 */

 rounding:
  switch (rnd_mode)
    {
    case GMP_RNDN:
      if (fb == 0)
        {
          if (rb == 0)
            {
              inex = 0;
              goto end_of_add;
            }
          /* round to even */
          if (ap[0] & (MP_LIMB_T_ONE << sh))
            goto rndn_away;
          else
            goto rndn_zero;
        }
      if (rb == 0)
        {
        rndn_zero:
          inex = MPFR_ISNEG(a) ? 1 : -1;
          goto end_of_add;
        }
      else
        {
        rndn_away:
          inex = MPFR_ISNONNEG(a) ? 1 : -1;
          goto add_one_ulp;
        }

    case GMP_RNDZ:
      inex = rb || fb ? (MPFR_ISNEG(a) ? 1 : -1) : 0;
      goto end_of_add;

    case GMP_RNDU:
      inex = rb || fb;
      if (inex && MPFR_ISNONNEG(a))
        goto add_one_ulp;
      else
        goto end_of_add;

    case GMP_RNDD:
      inex = - (rb || fb);
      if (inex && MPFR_ISNEG(a))
        goto add_one_ulp;
      else
        goto end_of_add;

    default:
      MPFR_ASSERTN(0);
      inex = 0;
      goto end_of_add;
    }

 add_one_ulp: /* add one unit in last place to a */
  if (mpn_add_1(ap, ap, an, MP_LIMB_T_ONE << sh))
    {
      mp_exp_t exp = MPFR_EXP(a);
      if (exp == __mpfr_emax)
        inex = mpfr_set_overflow(a, rnd_mode, MPFR_SIGN(a));
      else
        {
          MPFR_EXP(a)++;
          ap[an-1] = GMP_LIMB_HIGHBIT;
        }
    }

 end_of_add:
  TMP_FREE(marker);
  return inex;
}
