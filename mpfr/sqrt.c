/* mpfr_sqrt -- square root of a floating-point number

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

/* #define DEBUG */

int
mpfr_sqrt (mpfr_ptr r, mpfr_srcptr u, mp_rnd_t rnd_mode)
{
  mp_ptr up, rp, tmp, remp;
  mp_size_t usize, rrsize;
  mp_size_t rsize;
  mp_size_t err;
  mp_limb_t q_limb;
  int odd_exp_u;
  long rw, nw, k; 
  int inexact = 0, t;
  unsigned long cc = 0; 
  int can_round = 0;

  TMP_DECL(marker);

    if (MPFR_IS_NAN(u))
      {
        MPFR_SET_NAN(r);
        MPFR_RET_NAN;
      }

    if (MPFR_SIGN(u) < 0)
      {
        if (MPFR_IS_INF(u) || MPFR_NOTZERO(u))
          {
            MPFR_SET_NAN(r);
            MPFR_RET_NAN;
          }
        else
          { /* sqrt(-0) = -0 */
            MPFR_CLEAR_FLAGS(r);
            MPFR_SET_ZERO(r);
            MPFR_SET_NEG(r);
            MPFR_RET(0);
          }
      }

    MPFR_CLEAR_NAN(r);
    MPFR_SET_POS(r);

    if (MPFR_IS_INF(u))
      { 
        MPFR_SET_INF(r);
        MPFR_RET(0);
      }

    MPFR_CLEAR_INF(r);

    if (MPFR_IS_ZERO(u))
      {
        MPFR_SET_ZERO(r);
        MPFR_RET(0); /* zero is exact */
      }

    up = MPFR_MANT(u);
    usize = (MPFR_PREC(u) - 1)/BITS_PER_MP_LIMB + 1;

#ifdef DEBUG
    printf("Entering square root : ");
    for(k = usize - 1; k >= 0; k--) { printf("%lu ", up[k]); }
    printf(".\n");
#endif

    /* Compare the mantissas */

    odd_exp_u = (unsigned int) MPFR_EXP(u) & 1;
    MPFR_ASSERTN(MPFR_PREC(r) <= MPFR_INTPREC_MAX - 3);
    rrsize = (MPFR_PREC(r) + 2 + odd_exp_u) / BITS_PER_MP_LIMB + 1;
    MPFR_ASSERTN(rrsize <= MP_SIZE_T_MAX/2);
    rsize = rrsize << 1;
    /* One extra bit is needed in order to get the square root with enough
       precision ; take one extra bit for rrsize in order to solve more
       easily the problem of rounding to nearest.
       Need to have 2*rrsize = rsize...
       Take one extra bit if the exponent of u is odd since we shall have
       to shift then.
    */

    TMP_MARK(marker);
    if (odd_exp_u) /* Shift u one bit to the right */
      {
        if (MPFR_PREC(u) & (BITS_PER_MP_LIMB - 1))
          {
            up = TMP_ALLOC(usize * BYTES_PER_MP_LIMB);
            mpn_rshift(up, MPFR_MANT(u), usize, 1);
          }
        else
          {
            up = TMP_ALLOC((usize + 1) * BYTES_PER_MP_LIMB);
            if (mpn_rshift(up + 1, MPFR_MANT(u), usize, 1))
              up[0] = GMP_LIMB_HIGHBIT;
            else
              up[0] = 0;
            usize++;
          }
      }

    MPFR_EXP(r) = MPFR_EXP(u) != MPFR_EMAX_MAX
      ? (MPFR_EXP(u) + odd_exp_u) / 2
      : (MPFR_EMAX_MAX - 1) / 2 + 1;

    do
      {

        err = rsize * BITS_PER_MP_LIMB;
        if (rsize < usize)
          err--;
        if (err > rrsize * BITS_PER_MP_LIMB)
          err = rrsize * BITS_PER_MP_LIMB;
      
        tmp = (mp_ptr) TMP_ALLOC (rsize * BYTES_PER_MP_LIMB);
        rp = (mp_ptr) TMP_ALLOC (rrsize * BYTES_PER_MP_LIMB);
        remp = (mp_ptr) TMP_ALLOC (rsize * BYTES_PER_MP_LIMB);

        if (usize >= rsize)
          {
            MPN_COPY (tmp, up + usize - rsize, rsize);
          }
        else
          {
            MPN_COPY (tmp + rsize - usize, up, usize);
            MPN_ZERO (tmp, rsize - usize);
          }

        /* Do the real job */
 
#ifdef DEBUG
        printf("Taking the sqrt of : ");
        for(k = rsize - 1; k >= 0; k--)
          printf("+%lu*2^%lu",tmp[k],k*BITS_PER_MP_LIMB);
        printf(".\n");
#endif

        q_limb = mpn_sqrtrem (rp, remp, tmp, rsize);

#ifdef DEBUG
        printf ("The result is : \n");
        printf ("sqrt : ");
        for (k = rrsize - 1; k >= 0; k--)
          printf ("%lu ", rp[k]);
        printf ("(inexact = %lu)\n", q_limb);
#endif

        can_round = mpfr_can_round_raw(rp, rrsize, 1, err,
                                       GMP_RNDZ, rnd_mode, MPFR_PREC(r));

        /* If we used all the limbs of both the dividend and the divisor,
           then we have the correct RNDZ rounding */

        if (!can_round && (rsize < 2*usize))
          {
#ifdef DEBUG
            printf("Increasing the precision.\n");
#endif
          }
      }
    while (!can_round && (rsize < 2*usize) && (rsize += 2) && (rrsize++));
#ifdef DEBUG
    printf ("can_round = %d\n", can_round);
#endif

    /* This part may be deplaced upper to avoid a few mpfr_can_round_raw */
    /* when the square root is exact. It is however very unprobable that */
    /* it would improve the behaviour of the present code on average.    */

    if (!q_limb) /* the sqrtrem call was exact, possible exact square root */
      {
        /* if we have taken into account the whole of up */
        for (k = usize - rsize - 1; k >= 0; k++)
          if (up[k] != 0)
            break;

        if (k < 0)
          goto fin; /* exact square root ==> inexact = 0 */
      }

    if (can_round)
      {
        cc = mpfr_round_raw (rp, rp, err, 0, MPFR_PREC(r), rnd_mode, &inexact);
        if (inexact == 0) /* exact high part: inex flag depends on remainder */
          inexact = -q_limb;
        rrsize = (MPFR_PREC(r) - 1)/BITS_PER_MP_LIMB + 1;
      }
    else
      {
        /* Use the return value of sqrtrem to decide of the rounding */
        /* Note that at this point the sqrt has been computed        */
        /* EXACTLY.                                                  */
        switch (rnd_mode)
          {
          case GMP_RNDZ :
          case GMP_RNDD :
            inexact = -1; /* result is truncated */
            break;

          case GMP_RNDN :
            /* Not in the situation ...0 111111 */
            rw = (MPFR_PREC(r) + 1) & (BITS_PER_MP_LIMB - 1);
            if (rw != 0)
              {
                rw = BITS_PER_MP_LIMB - rw;
                nw = 0;
              }
            else
              nw = 1;
            if ((rp[nw] >> rw) & 1 &&          /* Not 0111111111 */
                (q_limb ||                     /* Nonzero remainder */
                 (rw ? (rp[nw] >> (rw + 1)) & 1 :
                  (rp[nw] >> (BITS_PER_MP_LIMB - 1)) & 1))) /* or even r. */
              {
                cc = mpn_add_1 (rp + nw, rp + nw, rrsize, MP_LIMB_T_ONE << rw);
                inexact = 1;
              }
            else
              inexact = -1;
            break;
 
          case GMP_RNDU:
            /* we should arrive here only when the result is inexact, i.e.
               either q_limb > 0 (the remainder from mpn_sqrtrem is non-zero)
               or up[0..usize-rsize-1] is non zero, thus we have to add one
               ulp, and inexact = 1 */
            inexact = 1;
            t = MPFR_PREC(r) & (BITS_PER_MP_LIMB - 1);
            rsize = (MPFR_PREC(r) - 1)/BITS_PER_MP_LIMB + 1;
	    cc = mpn_add_1 (rp + rrsize - rsize, rp + rrsize - rsize, rsize,
                            t != 0 ?
                            MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - t) :
                            MP_LIMB_T_ONE);
          }
      }

    if (cc)
      {
        /* Is a shift necessary here? Isn't the result 1.0000...? */
        mpn_rshift (rp, rp, rrsize, 1);
        rp[rrsize-1] |= GMP_LIMB_HIGHBIT;
        MPFR_EXP(r)++;
      }

 fin:
    rsize = rrsize;
    rrsize = (MPFR_PREC(r) - 1)/BITS_PER_MP_LIMB + 1;
    MPN_COPY(MPFR_MANT(r), rp + rsize - rrsize, rrsize);

    if (MPFR_PREC(r) & (BITS_PER_MP_LIMB - 1))
      MPFR_MANT(r)[0] &= ~((MP_LIMB_T_ONE <<
                            (BITS_PER_MP_LIMB - 
                             (MPFR_PREC(r) & (BITS_PER_MP_LIMB - 1)))) - 1);

  TMP_FREE(marker);
  return inexact;
}
