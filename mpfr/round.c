/* mpfr_round_raw2, mpfr_round_raw, mpfr_round, mpfr_can_round, 
   mpfr_can_round_raw -- various rounding functions

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"

#ifdef Exp
#include "longlong.h"
#endif

/* returns 0 if round(sign*xp[0..xn-1], prec, rnd) = 
   round(sign*xp[0..xn-1], prec, GMP_RNDZ), 1 otherwise,
   where sign=1 if neg=0, sign=-1 otherwise.

   Does *not* modify anything.
*/
int 
#if __STDC__
mpfr_round_raw2(mp_limb_t *xp, unsigned long xn, 
		char neg, char rnd, unsigned long prec)
#else
mpfr_round_raw2(xp, xn, neg, rnd, prec)
     mp_limb_t *xp; 
     unsigned long xn; 
     char neg; 
     char rnd; 
     unsigned long prec; 
#endif
{
  unsigned long nw; long wd; char rw; short l; mp_limb_t mask;

  nw = prec / BITS_PER_MP_LIMB; rw = prec & (BITS_PER_MP_LIMB - 1); 
  if (rw) nw++; 
  if (rnd==GMP_RNDZ || xn<nw || (rnd==GMP_RNDU && neg)
      || (rnd==GMP_RNDD && neg==0)) return 0;

  mask = ~((((mp_limb_t)1)<<(BITS_PER_MP_LIMB - rw)) - 1);
  switch (rnd)
    {
    case GMP_RNDU:
    case GMP_RNDD:
      if (xp[xn - nw] & ~mask) return 1;
      for (l = nw + 1;l <= xn; l++)
	if (xp[xn - l]) break;
      return (l <= xn);

    case GMP_RNDN:
    /* First check if we are just halfway between two representable numbers */
      wd = xn - nw;
      if (!rw)
	{
	  if (!wd) /* all bits are significative */ return 0; 
	  wd--;
	  if (xp[wd] == ((mp_limb_t)1 << (BITS_PER_MP_LIMB - 1)))
	    {
	      do wd--; while (wd > 0 && !xp[wd]);
	      if (!wd) { return 1; } else return xp[xn - nw] & 1;
	    }

	  return xp[wd]>>(BITS_PER_MP_LIMB - 1);
	}
      else
      if (rw + 1 < BITS_PER_MP_LIMB)
	{
	  if ((xp[wd] & (~mask)) == (((mp_limb_t)1) << (BITS_PER_MP_LIMB - rw - 1)))
	      do { wd--; } while (wd >= 0 && !xp[wd]);
	  else return ((xp[wd]>>(BITS_PER_MP_LIMB - rw - 1)) & 1);
	  
	  /* first limb was in the middle, and others down to wd+1 were 0 */
	  if (wd>=0) return 1;
	  else
	      return ((xp[xn - nw] & mask) >> (BITS_PER_MP_LIMB - rw)) & 1;
	}
      else
	/* Modified PZ, 27 May 1999:
	   rw, i.e. the number of bits stored in xp[xn-nw], is 
	   BITS_PER_MP_LIMB-1, i.e. there is exactly one non significant bit. 
	   We are just halfway iff xp[wd] has its low significant bit 
	   set and all limbs xp[0]...xp[wd-1] are zero */
	{
	  if (xp[wd] & 1)
	      do wd--; while (wd >= 0 && !xp[wd]);
	  return ((wd<0) ? xp[xn-nw]>>1 : xp[xn-nw]) & 1;
	}
    default: return 0;
    }
}

/* puts in y the value of xp (with precision xprec and sign 1 if negative=0,
   -1 otherwise) rounded to precision yprec and direction RND_MODE 
   Supposes x is not zero nor NaN nor +/- Infinity (i.e. *xp != 0).
*/
int
#if __STDC__
mpfr_round_raw(mp_limb_t *y, mp_limb_t *xp, unsigned long xprec, char negative,
	       unsigned long yprec, char RND_MODE)
#else
mpfr_round_raw(y, xp, xprec, negative, yprec, RND_MODE)
     mp_limb_t *y; 
     mp_limb_t *xp; 
     unsigned long xprec; 
     char negative;
     unsigned long yprec; 
     char RND_MODE; 
#endif
{
  unsigned long nw, xsize; mp_limb_t mask;
  char rw, xrw, carry = 0;

  xsize = (xprec-1)/BITS_PER_MP_LIMB + 1;
  xrw = xprec % BITS_PER_MP_LIMB; if (xrw == 0) { xrw = BITS_PER_MP_LIMB; }

#ifdef Exp
  count_leading_zeros(flag, xp[xsize-1]); 
  yprec += flag; 
#endif

  nw = yprec / BITS_PER_MP_LIMB; rw = yprec & (BITS_PER_MP_LIMB - 1); 
  if (rw) nw++; 
  /* number of words needed to represent x */

  mask = ~((((mp_limb_t)1)<<(BITS_PER_MP_LIMB - rw)) - (mp_limb_t)1); 

  /* precision is larger than the size of x. No rounding is necessary. 
     Just add zeroes at the end */
  if (xsize < nw) { 
    MPN_COPY(y + nw - xsize, xp, xsize);
    MPN_ZERO(y, nw - xsize); /* PZ 27 May 99 */
    y[0] &= mask;
    return 0; 
  }

  if (mpfr_round_raw2(xp, xsize, negative, RND_MODE, yprec))
    carry = mpn_add_1(y, xp + xsize - nw, nw,
                          ((mp_limb_t)1) << (BITS_PER_MP_LIMB - rw));
  else MPN_COPY(y, xp + xsize - nw, nw);

  y[0] &= mask;
  return carry; 
}

void
#if __STDC__
mpfr_round(mpfr_t x, char RND_MODE, unsigned long prec)
#else
mpfr_round(x, RND_MODE, prec)
     mpfr_t x; 
     char RND_MODE; 
     unsigned long prec; 
#endif
{
  mp_limb_t *tmp; int carry; unsigned long nw; 
  TMP_DECL(marker); 

  nw = prec / BITS_PER_MP_LIMB; 
  if (prec & (BITS_PER_MP_LIMB - 1)) nw++;
  TMP_MARK(marker); 
  tmp = TMP_ALLOC (nw * BYTES_PER_MP_LIMB);
  carry = mpfr_round_raw(tmp, MANT(x), PREC(x), (SIGN(x)<0), prec, RND_MODE);

  if (carry)
    {      
      mpn_rshift(tmp, tmp, nw, 1); 
      tmp [nw-1] |= (((mp_limb_t)1) << (BITS_PER_MP_LIMB - 1)); 
      EXP(x)++; 
    }

  if (SIGN(x)<0) { SIZE(x)=nw; CHANGE_SIGN(x); } else SIZE(x)=nw;
  PREC(x) = prec; 
  MPN_COPY(MANT(x), tmp, nw); 
  TMP_FREE(marker); 
}

/* hypotheses : BITS_PER_MP_LIMB est une puissance de 2 
                dans un test, la premiere partie du && est evaluee d'abord */


/* assuming b is an approximation of x in direction rnd1 
   with error at most 2^(EXP(b)-err), returns 1 if one is 
   able to round exactly x to precision prec with direction rnd2,
   and 0 otherwise.

   Side effects: none.
*/

int 
#if __STDC__
mpfr_can_round(mpfr_t b, unsigned long err, unsigned char rnd1, 
	       unsigned char rnd2, unsigned long prec)
#else
mpfr_can_round(b, err, rnd1, rnd2, prec) 
     mpfr_t b; 
     unsigned long err;
     unsigned char rnd1;
     unsigned char rnd2; 
     unsigned long prec;
#endif
{
  return mpfr_can_round_raw(MANT(b), (PREC(b) - 1)/BITS_PER_MP_LIMB + 1, 
			    SIGN(b), err, rnd1, rnd2, prec); 
}

int
#if __STDC__
mpfr_can_round_raw(mp_limb_t *bp, unsigned long bn, int neg, 
		   unsigned long err, unsigned char rnd1, unsigned char rnd2, 
		   unsigned long prec)
#else
mpfr_can_round_raw(bp, bn, neg, err, rnd1, rnd2, prec)
     mp_limb_t *bp;
     unsigned long bn;
     int neg; 
     unsigned long err; 
     unsigned char rnd1;
     unsigned char rnd2; 
     unsigned long prec; 
#endif
{
  int k, k1, l, l1, tn; mp_limb_t cc, cc2, *tmp;
  TMP_DECL(marker); 

  if (err<=prec) return 0;
  neg = (neg > 0 ? 0 : 1); 

  /* warning: if k = m*BITS_PER_MP_LIMB, consider limb m-1 and not m */
  k = (err-1)/BITS_PER_MP_LIMB;
  l = err % BITS_PER_MP_LIMB; if (l) l = BITS_PER_MP_LIMB-l;
  /* the error corresponds to bit l in limb k */
  k1 = (prec-1)/BITS_PER_MP_LIMB;
  l1 = prec%BITS_PER_MP_LIMB; if (l1) l1 = BITS_PER_MP_LIMB-l1;

  /* the last significant bit is bit l1 in limb k1 */

  /* don't need to consider the k1 most significant limbs */
  k -= k1; bn -= k1; prec -= k1*BITS_PER_MP_LIMB; k1=0;

  if (rnd1==GMP_RNDU) { if (neg) rnd1=GMP_RNDZ; }
  if (rnd1==GMP_RNDD) { if (neg) rnd1=GMP_RNDU; else rnd1=GMP_RNDZ; }

  /* in the sequel, RNDU = towards infinity, RNDZ = towards zero */

  TMP_MARK(marker);
  tn = bn;
  k++; /* since we work with k+1 everywhere */

  switch (rnd1) {
    
  case GMP_RNDZ: /* b <= x <= b+2^(EXP(b)-err) */
    tmp = TMP_ALLOC(tn*BYTES_PER_MP_LIMB); 
    cc = (bp[bn-1]>>l1) & 1;
    cc ^= mpfr_round_raw2(bp, bn, neg, rnd2, prec);

    /* now round b+2^(EXP(b)-err) */
    cc2 = mpn_add_1(tmp+bn-k, bp+bn-k, k, (mp_limb_t)1<<l);
    /* if carry, then all bits up to err were to 1, and we can round only
       if cc==0 and mpfr_round_raw2 returns 0 below */
    if (cc2 && cc) { TMP_FREE(marker); return 0; }
    cc2 = (tmp[bn-1]>>l1) & 1; /* gives 0 when carry */
    cc2 ^= mpfr_round_raw2(tmp, bn, neg, rnd2, prec);

    TMP_FREE(marker); 
    return (cc == cc2);

  case GMP_RNDU: /* b-2^(EXP(b)-err) <= x <= b */
    tmp = TMP_ALLOC(tn*BYTES_PER_MP_LIMB); 
    /* first round b */
    cc = (bp[bn-1]>>l1) & 1;
    cc ^= mpfr_round_raw2(bp, bn, neg, rnd2, prec);

    /* now round b-2^(EXP(b)-err) */
    cc2 = mpn_sub_1(tmp+bn-k, bp+bn-k, k, (mp_limb_t)1<<l);
    /* if borrow, then all bits up to err were to 0, and we can round only
       if cc==0 and mpfr_round_raw2 returns 1 below */
    if (cc2 && cc) { TMP_FREE(marker); return 0; }
    cc2 = (tmp[bn-1]>>l1) & 1; /* gives 1 when carry */
    cc2 ^= mpfr_round_raw2(tmp, bn, neg, rnd2, prec);

    TMP_FREE(marker); 
    return (cc == cc2);

  case GMP_RNDN: /* b-2^(EXP(b)-err-1) <= x <= b+2^(EXP(b)-err-1) */
    if (l==0) tn++; 
    tmp = TMP_ALLOC(tn*BYTES_PER_MP_LIMB); 

    /* this case is the same than GMP_RNDZ, except we first have to
       subtract 2^(EXP(b)-err-1) from b */

    if (l) { 
      l--; /* tn=bn */
      mpn_sub_1(tmp+tn-k, bp+bn-k, k, (mp_limb_t)1<<l);
    } 
    else { 
      MPN_COPY(tmp+1, bp, bn); *tmp=0; /* extra limb to add or subtract 1 */
      k++; l=BITS_PER_MP_LIMB-1; 
      mpn_sub_1(tmp+tn-k, tmp+tn-k, k, (mp_limb_t)1<<l);
    }

    /* round b-2^(EXP(b)-err-1) */
    /* we can disregard borrow, since we start from tmp in 2nd case too */
    cc = (tmp[tn-1]>>l1) & 1;
    cc ^= mpfr_round_raw2(tmp, tn, neg, rnd2, prec);

    if (l==BITS_PER_MP_LIMB-1) { l=0; k--; } else l++;

    /* round b+2^(EXP(b)-err-1) = b-2^(EXP(b)-err-1) + 2^(EXP(b)-err) */    
    cc2 = mpn_add_1(tmp+tn-k, tmp+tn-k, k, (mp_limb_t)1<<l);
    /* if carry, then all bits up to err were to 1, and we can round only
       if cc==0 and mpfr_round_raw2 returns 0 below */
    if (cc2 && cc) { TMP_FREE(marker); return 0; }
    cc2 = (tmp[tn-1]>>l1) & 1; /* gives 0 when carry */
    cc2 ^= mpfr_round_raw2(tmp, tn, neg, rnd2, prec);

    TMP_FREE(marker); 
    return (cc == cc2);
  }
  return 0;
}
