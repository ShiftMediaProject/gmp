/* mpfr_round_raw_generic -- Generic rounding function

Copyright 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

#ifndef flag
# error "ERROR: flag must be defined (0 / 1)"
#endif
#ifndef use_inexp
# error "ERROR: use_enexp must be defined (0 / 1)"
#endif
#ifndef mpfr_round_raw_generic
# error "ERROR: mpfr_round_raw_generic must be defined"
#endif

/*
 * If flag = 0, puts in y the value of xp (with precision xprec and
 * sign 1 if negative=0, -1 otherwise) rounded to precision yprec and
 * direction rnd_mode. Supposes x is not zero nor NaN nor +/- Infinity
 * (i.e. *xp != 0).
 *
 * If inexp != NULL, computes the inexact flag of the rounding.
 * (In case of even rounding when rnd = GMP_RNDN, puts 2 or -2 in *inexp.)
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
                       int neg, mp_prec_t yprec, mp_rnd_t rnd_mode
#if use_inexp != 0
		       , int *inexp
#endif
		       )
{
  mp_size_t xsize, nw;
  mp_limb_t himask, lomask, sb;
  int carry, rw;
#if use_inexp == 0
  int *inexp;
#endif

  if (use_inexp)
    MPFR_ASSERTD(inexp != ((int*) 0));
  MPFR_ASSERTD(neg == 0 || neg == 1);

  if (flag && !use_inexp && 
      (rnd_mode==GMP_RNDZ || xprec <= yprec
       ||MPFR_IS_RNDUTEST_OR_RNDDNOTTEST(rnd_mode, neg)))
    return 0;

  xsize = (xprec-1)/BITS_PER_MP_LIMB + 1;
  nw = yprec / BITS_PER_MP_LIMB;
  rw = yprec & (BITS_PER_MP_LIMB - 1);

  if (MPFR_UNLIKELY(xprec <= yprec))
    { /* No rounding is necessary. */
      /* if yp=xp, maybe an overlap: MPN_COPY_DECR is ok when src <= dst */
      if (MPFR_LIKELY(rw))
	nw++;
      MPFR_ASSERTD(nw >= 1);
      MPFR_ASSERTD(nw >= xsize); 
      if (use_inexp)
        *inexp = 0;
      if (!flag)
	{
	  MPN_COPY_DECR(yp + (nw - xsize), xp, xsize);
	  MPN_ZERO(yp, nw - xsize);
	}
      return 0;
    }

  /* Transform RNDD & RNDU to RND Away */
  if (MPFR_IS_RNDUTEST_OR_RNDDNOTTEST(rnd_mode, neg))
    rnd_mode = GMP_RNDZ;

  if (use_inexp || rnd_mode != GMP_RNDZ)
    {      
      mp_size_t k = xsize - nw - 1;

      if (MPFR_LIKELY(rw))
	{
	  nw++;
	  lomask = ((MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - rw))
		    - MP_LIMB_T_ONE);
	  himask = ~lomask;
	}
      else
	{
	  lomask = -1;
	  himask = -1;
	}
      MPFR_ASSERTD(k >= 0);
      sb = xp[k] & lomask;  /* First non-significant bits */
      /* Rounding to nearest ? */
      if (rnd_mode == GMP_RNDN)
	{
	  /* Rounding to nearest */
	  mp_limb_t rbmask = MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - rw - 1);
	  if (sb & rbmask) /* rounding bit */
	    sb &= ~rbmask; /* it is 1, clear it */
	  else
	    {
	      /*rnd_mode = GMP_RNDZ;*/ 
	      /* it is 0, behave like rounding to 0 */
	      goto rnd_RNDZ;
	    }
	  while (sb == 0 && k > 0)
	    sb = xp[--k];
	  /* rounding to nearest, with rounding bit = 1 */
	  if (MPFR_UNLIKELY(sb == 0)) /* Even rounding. */
	    {
	      /* sb == 0 && rnd_mode == GMP_RNDN */
	      sb = xp[xsize - nw] & (himask ^ (himask << 1));
	      if (sb == 0)
		{
		  if (use_inexp)
		    *inexp = 2*MPFR_EVEN_INEX*neg-MPFR_EVEN_INEX;
		  /* ((neg!=0)^(sb!=0)) ? MPFR_EVEN_INEX  : -MPFR_EVEN_INEX;*/
		  /* Since neg = 0 or 1 and sb=0*/
		  if (flag)
		    return 0 /*sb != 0 && rnd_mode != GMP_RNDZ */;
		  MPN_COPY_INCR(yp, xp + xsize - nw, nw);
		  yp[0] &= himask;
		  return 0;
		}
	      else
		{
		  /* sb != 0 && rnd_mode == GMP_RNDN */
                  if (use_inexp)
                    *inexp = MPFR_EVEN_INEX-2*MPFR_EVEN_INEX*neg;
		  /*((neg!=0)^(sb!=0))? MPFR_EVEN_INEX  : -MPFR_EVEN_INEX; */
		  /*Since neg= 0 or 1 and sb != 0 */
		  goto rnd_RNDN_sb_return;
		}
	    }
	  else /* sb != 0  && rnd_mode == GMP_RNDN*/
	    {
	      if (use_inexp)
		/* *inexp = (neg == 0) ? 1 : -1; but since neg = 0 or 1 */
		*inexp = 1-2*neg;
	    rnd_RNDN_sb_return:
	      if (flag)
		return 1; /*sb != 0 && rnd_mode != GMP_RNDZ;*/
	      carry = mpn_add_1(yp, xp + xsize - nw, nw,
				rw ? 
				MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - rw) 
				: 1);
	      yp[0] &= himask;
	      return carry;
	    }
	}
      /* Rounding to Zero ? */
      else if (rnd_mode == GMP_RNDZ)
	{
	  /* rnd_mode == GMP_RNDZ */
	rnd_RNDZ:
	  while ((sb == 0) && k > 0)
	    sb = xp[--k];
	  if (use_inexp)
	    /* rnd_mode == GMP_RNDZ and neg = 0 or 1 */
	    /* (neg != 0) ^ (rnd_mode != GMP_RNDZ)) ? 1 : -1);*/ 
	    *inexp = (sb == 0) ? 0 : (2*neg-1);
	  if (flag)
	    return 0; /*sb != 0 && rnd_mode != GMP_RNDZ;*/
	  MPN_COPY_INCR(yp, xp + xsize - nw, nw);
	  yp[0] &= himask;	      
	  return 0;
	}
      else 
	{
	  /* rnd_mode = Away */
          while (MPFR_UNLIKELY(sb == 0) && k > 0)
            sb = xp[--k];
	  if (sb == 0)
	    {
	      /* sb = 0 && rnd_mode != GMP_RNDZ */
	      if (use_inexp)
		/* (neg != 0) ^ (rnd_mode != GMP_RNDZ)) ? 1 : -1);*/
		*inexp = 0;
	      if (flag)
		return 0;
	      MPN_COPY_INCR(yp, xp + xsize - nw, nw);
	      yp[0] &= himask;
	      return 0;
	    }
	  else
	    {
              /* sb != 0 && rnd_mode != GMP_RNDZ */
              if (use_inexp)
                /* (neg != 0) ^ (rnd_mode != GMP_RNDZ)) ? 1 : -1);*/
                *inexp = 1-2*neg;
              if (flag)
                return 1;
	      carry = mpn_add_1(yp, xp + xsize - nw, nw,
				rw ? MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - rw)
				: 1);
              yp[0] &= himask;
              return carry;
	    }
	}
    }
  else
    {
      /* sb = 0 */
      if (flag)
	return 0 /*sb != 0 && rnd_mode != GMP_RNDZ*/;
      if (MPFR_LIKELY(rw))
        {
          nw++;
          himask = ~( ((MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - rw))
		       - MP_LIMB_T_ONE));
        }
      else
	himask = -1;
      MPN_COPY_INCR(yp, xp + xsize - nw, nw);
      yp[0] &= himask;
      return 0;
    }
}

#undef flag
#undef use_inexp
#undef mpfr_round_raw_generic
