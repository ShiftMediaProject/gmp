/* mpfr_add -- add two floating-point numbers

Copyright (C) 1999 Free Software Foundation.

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
#include "mpfr-impl.h"

/* #define DEBUG */

extern void mpfr_sub1 _PROTO((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, 
			      mp_rnd_t, int));
void mpfr_add1 _PROTO((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mp_rnd_t, int));

#define ONE ((mp_limb_t) 1)

/* signs of b and c are supposed equal,
   diff_exp is the difference between the exponents of b and c,
   which is supposed >= 0 */

void 
#if __STDC__
mpfr_add1 (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, 
	   mp_rnd_t rnd_mode, int diff_exp) 
#else
mpfr_add1 (a, b, c, rnd_mode, diff_exp) 
     mpfr_ptr a;
     mpfr_srcptr b;
     mpfr_srcptr c;
     mp_rnd_t rnd_mode;
     int diff_exp;
#endif
{
  mp_limb_t *ap, *bp, *cp, cc, c2, c3=0; unsigned int an,bn,cn; int sh,dif,k;
  TMP_DECL(marker); 

  TMP_MARK(marker); 
  ap = MPFR_MANT(a);
  bp = MPFR_MANT(b);
  cp = MPFR_MANT(c);
  if (ap == bp) {
    bp = (mp_ptr) TMP_ALLOC(MPFR_ABSSIZE(b) * BYTES_PER_MP_LIMB); 
    MPN_COPY (bp, ap, MPFR_ABSSIZE(b));
    if (ap == cp) { cp = bp; }
  }
  else if (ap == cp)
    {
      cp = (mp_ptr) TMP_ALLOC (MPFR_ABSSIZE(c) * BYTES_PER_MP_LIMB);
      MPN_COPY(cp, ap, MPFR_ABSSIZE(c)); 
    }

  an = (MPFR_PREC(a)-1)/BITS_PER_MP_LIMB+1; /* number of significant limbs of a */

  sh = an*BITS_PER_MP_LIMB-MPFR_PREC(a); /* non-significant bits in low limb */
  bn = (MPFR_PREC(b)-1)/BITS_PER_MP_LIMB + 1; /* number of significant limbs of b */
  cn = (MPFR_PREC(c)-1)/BITS_PER_MP_LIMB + 1;
  MPFR_EXP(a) = MPFR_EXP(b);

  if (MPFR_SIGN(a) * MPFR_SIGN(b) < 0) MPFR_CHANGE_SIGN(a);

  /* case 1: diff_exp>=prec(a), i.e. c only affects the last bit
     through rounding */
  dif = MPFR_PREC(a)-diff_exp;

#ifdef DEBUG
  printf("diff_exp=%u dif=MPFR_PREC(a)-diff_exp=%d\n", diff_exp, dif);
  printf("b= "); mpfr_print_raw(b); putchar('\n');
  printf("c="); for (k=0;k<diff_exp;k++) putchar(' ');
  if (MPFR_SIGN(c)>0) putchar(' '); mpfr_print_raw(c); putchar('\n');
#endif
  if (dif<=0) { 
    
    /* diff_exp>=MPFR_PREC(a): c does not overlap with a */
    /* either MPFR_PREC(b)<=MPFR_PREC(a), and we can copy the mantissa of b directly 
       into that of a, or MPFR_PREC(b)>MPFR_PREC(a) and we have to round b+c */

    if (MPFR_PREC(b)<=MPFR_PREC(a)) {

      MPN_COPY(ap+(an-bn), bp, bn);
      /* fill low significant limbs with zero */

      for (bp=ap;bn<an;bn++) *bp++=0;

      /* now take c into account */
      if (rnd_mode==GMP_RNDN) { 
	
	/* to nearest */
	/* if diff_exp > MPFR_PREC(a), no change */

	if (diff_exp==MPFR_PREC(a)) {

	  /* if c is not zero, then as it is normalized, we have to add
	     one to the lsb of a if c>1/2, or c=1/2 and lsb(a)=1 (round to
	     even) */
	  
	  if (MPFR_NOTZERO(c)) { 

	    /* c is not zero */
	    /* check whether mant(c)=1/2 or not */

	    cc = *cp - (ONE<<(BITS_PER_MP_LIMB-1));
	    if (cc==0) {
	      bp = cp+(MPFR_PREC(c)-1)/BITS_PER_MP_LIMB;
	      while (cp<bp && cc==0) cc = *++cp;
	    }

	    if (cc || (ap[an-1] & (ONE<<sh))) goto add_one_ulp;
	    /* mant(c) != 1/2 or mant(c) = 1/2: add 1 iff lsb(a)=1 */
	  }
	}
      }
      else if ((MPFR_ISNONNEG(b) && rnd_mode==GMP_RNDU) || 
	       (MPFR_ISNEG(b) && rnd_mode==GMP_RNDD)) {
	
	/* round up */
	if (MPFR_NOTZERO(c)) goto add_one_ulp;
      }
      /* in the other cases (round to zero, or up/down with sign -/+),
         nothing to do */
    }
    else { 

      /* MPFR_PREC(b)>MPFR_PREC(a) : we have to round b+c */      
      k=bn-an;

      /* first copy the 'an' most significant limbs of b to a */
      MPN_COPY(ap, bp+k, an);
      { /* treat all rounding modes together */
	mp_limb_t c2old; long int cout=0; int sign=0;
	if (sh) {
	  cc = *ap & ((ONE<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	}
	else cc=0;

        dif += sh; 
	if (dif>0) {
	  cn--;
	  c2old = cp[cn]; /* last limb from c considered */
	  cout += mpn_add_1(&cc, &cc, 1, c2old >> (BITS_PER_MP_LIMB-dif));
	}
	else c2 = c2old = 0;
	if (sh && rnd_mode==GMP_RNDN)
	    cout -= mpn_sub_1(&cc, &cc, 1, ONE<<(sh-1));
	if (cout==0) {
	   dif += BITS_PER_MP_LIMB;
	   while (cout==0 && (k || cn)) {
	     cout = (cc>(mp_limb_t)1) ? 2 : cc;
	     cc = (k) ? bp[--k] : 0;
	     if (sh==0) {
	       cout -= mpn_sub_1(&cc, &cc, 1, ONE << (BITS_PER_MP_LIMB-1));
	       sh = 0;
	     }
	     /* next limb from c to consider is cp[cn-1], with lower part of
		c2old */
	     c2 = c2old << dif;
	     if (cn && (dif>=0)) {
	       cn--; 
	       c2old = cp[cn];
	       c2 += c2old >> (BITS_PER_MP_LIMB-dif);
	     }
	     else dif += BITS_PER_MP_LIMB;
	     cout += mpn_add_1(&cc, &cc, 1, c2);
	   }
	}
	if (cout==0) cout=(cc!=0);
        sign = (MPFR_ISNONNEG(b) && rnd_mode==GMP_RNDU)
	  || (MPFR_ISNEG(b) && rnd_mode==GMP_RNDD) || (rnd_mode==GMP_RNDN);
	/* round towards infinity if dif=1, towards zero otherwise */
	if ((sign==1) && (cout>0)) goto add_one_ulp;
	else if (rnd_mode==GMP_RNDN && cout==0 && (*ap & (ONE<<sh)))
	  goto add_one_ulp;
      }
    }
  }
  else { 
    /* diff_exp < MPFR_PREC(a) : c overlaps with a by dif bits */
    /* first copy upper part of c into a (after shift) */
    unsigned char overlap;
    
    k = (dif-1)/BITS_PER_MP_LIMB + 1; /* only the highest k limbs from c
					 have to be considered */
    cn = (MPFR_PREC(c)-1)/BITS_PER_MP_LIMB + 1;
    MPN_ZERO(ap+k, an-k); /* do it now otherwise ap[k] may be destroyed
			     in case dif<0 */

#ifdef DEBUG
    printf("MPFR_PREC(c)=%d\n", MPFR_PREC(c));
#endif
    if (dif<=MPFR_PREC(c)) { 
      /* c has to be truncated */
      dif = dif % BITS_PER_MP_LIMB;
      dif = (dif) ? BITS_PER_MP_LIMB-dif-sh : -sh;

      /* we have to shift by dif bits to the right */

      if (dif>0) mpn_rshift(ap, cp+(cn-k), k, dif);
      else if (dif<0) {
	ap[k] = mpn_lshift(ap, cp+(cn-k), k, -dif);

	/* put the non-significant bits in low limb for further rounding */

	if (cn >= k+1)
	  ap[0] += cp[cn-k-1]>>(BITS_PER_MP_LIMB+dif);
      }
      else MPN_COPY(ap, cp+(cn-k), k);
      overlap=1;
    }
    else { 

      /* c is not truncated, but we have to fill low limbs with 0 */

      k = diff_exp/BITS_PER_MP_LIMB;
      overlap = diff_exp%BITS_PER_MP_LIMB;

      /* warning: a shift of zero bit is not allowed */
      MPN_ZERO(ap, an-k-cn); 
      if (overlap) { 
	cc=mpn_rshift(ap+(an-k-cn), cp, cn, overlap); 
	if (an-k-cn>0) ap[an-k-cn-1]=cc;
      }
      else MPN_COPY(ap+(an-k-cn), cp, cn);
      overlap=0;
    }

    /* here overlap=1 iff ulp(c)<ulp(a) */
    /* then put high limbs to zero */
    /* now add 'an' upper limbs of b in place */

    if (MPFR_PREC(b)<=MPFR_PREC(a)) {
      overlap += 2;
      cc = mpn_add_n(ap+(an-bn), ap+(an-bn), bp, bn);
    }
    else
      /* MPFR_PREC(b) > MPFR_PREC(a): we have to truncate b */
      cc = mpn_add_n(ap, ap, bp+(bn-an), an); 

    if (cc) { 

      /* shift one bit to the right */

      c3 = (ap[0]&1) && (MPFR_PREC(a)%BITS_PER_MP_LIMB==0);
      mpn_rshift(ap, ap, an, 1);
      ap[an-1] += ONE<<(BITS_PER_MP_LIMB-1);
      MPFR_EXP(a)++;
    }
    
    /* remains to do the rounding */

#ifdef DEBUG
    printf("overlap=%d\n", overlap);
#endif
    if (rnd_mode==GMP_RNDN || (MPFR_ISNONNEG(b) && rnd_mode==GMP_RNDU)
	|| (MPFR_ISNEG(b) && rnd_mode==GMP_RNDD)) {

      int kc;
      
      /* four cases: overlap =
         (0) MPFR_PREC(b) > MPFR_PREC(a) and diff_exp+MPFR_PREC(c) <= MPFR_PREC(a)
         (1) MPFR_PREC(b) > MPFR_PREC(a) and diff_exp+MPFR_PREC(c) > MPFR_PREC(a)
         (2) MPFR_PREC(b) <= MPFR_PREC(a) and diff_exp+MPFR_PREC(c) <= MPFR_PREC(a)
         (3)  MPFR_PREC(b) <= MPFR_PREC(a) and diff_exp+MPFR_PREC(c) > MPFR_PREC(a) */

      switch (overlap)
	{ mp_limb_t cout;
        case 1: /* both b and c to round */
	  kc = cn-k; /* remains kc limbs from c */
	  k = bn-an; /* remains k limbs from b */

	  /* truncate last bits and store the difference with 1/2*ulp in cc */

	  cc = *ap & ((ONE<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	  if (rnd_mode==GMP_RNDN)
	    cout = -mpn_sub_1(&cc, &cc, 1, ONE<<(sh-1));
	  else cout=0;
	  if ((~cout==0) && (~cc)) break;
	  cout = cc;
	  while ((cout==0 || cout==-1) && k!=0 && kc!=0) {
	    kc--;
	    cout += mpn_add_1(&cc, bp+(--k), 1,(cp[kc+1]<<(BITS_PER_MP_LIMB-dif))
			    +(cp[kc]>>dif));
	    if (cout==0 || (~cout==0)) cout=cc;
	  }
	  if (kc==0 && dif) {
	    /* it still remains cp[0]<<(BITS_PER_MP_LIMB-dif) */
	    if (k!=0) cout += mpn_add_1(&cc, bp+(--k), 1, 
				      cp[0]<<(BITS_PER_MP_LIMB-dif));
	    else cc = cp[0]<<(BITS_PER_MP_LIMB-dif);
	    if ((cout==0 && cc==0) || (~cout==0 && ~cc==0)) cout=cc;
	  }
	  if ((long)cout>0 || (cout==0 && cc)) goto add_one_ulp;
	  else if ((long)cout<0)
	    { TMP_FREE(marker); return; /* no carry possible any more */ }
	  else if (kc==0) {
	    while (k && cout==0) cout=bp[--k];
	    if ((~cout) && (cout || (rnd_mode==GMP_RNDN && (*ap & (ONE<<sh)))))
	      goto add_one_ulp;
	    else goto end_of_add;
	  }

	  /* else round c: go through */

	case 3: /* only c to round */
	  bp=cp; k=cn-k; bn=cn;
	  goto to_nearest;

	case 0: /* only b to round */
	  k=bn-an; dif=0;
	  goto to_nearest;
        
	  /* otherwise the result is exact: nothing to do */
	}
    }
    /* else nothing to do: round towards zero, i.e. truncate last sh bits */
    else 
      *ap &= ~((ONE<<sh)-1);
  }
  goto end_of_add;
    
  to_nearest: /* 0 <= sh < BITS_PER_MP_LIMB : number of bits of a to truncate
                 bp[k] : last significant limb from b
		 bn : number of limbs of b
	      */
        /* c3=1 whenever b+c gave a carry out in most significant limb 
	   and the least significant bit (shifted right) was 1.
	   This can occur only when BITS_PER_MP_LIMB divides MPFR_PREC(a),
	   i.e. sh=0.
	 */
        if (sh) {
	  cc = *ap & ((ONE<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	  c2 = (rnd_mode==GMP_RNDN) ? ONE<<(sh-1) : 0;
	}
	else /* sh=0: no bit to truncate */
	  { 
	    if (k) cc = bp[--k]; else cc = 0;
	    c2 = (rnd_mode==GMP_RNDN) ? ONE<<(BITS_PER_MP_LIMB-1) : 0;
	    if (c3 && (cc || c2==0)) cc=c2+1; /* will force adding one ulp */
	  }
	if (cc>c2) goto add_one_ulp; /* trunc(b)>1/2*lsb(a) -> round up */
	else if (cc==c2) {
	  /* special case of rouding c shifted to the right */
	  if (dif>0 && k<bn) cc=bp[k]<<(BITS_PER_MP_LIMB-dif);
	  else cc=0;
	  while (k && cc==0) cc=bp[--k];
	  /* now if the truncated part of b = 1/2*lsb(a), check whether c=0 */
	  if (cc || (rnd_mode==GMP_RNDN && (*ap & (ONE<<sh))))
	    goto add_one_ulp;
	}
        goto end_of_add;

  add_one_ulp: /* add one unit in last place to a */
    cc = mpn_add_1(ap, ap, an, ONE<<sh);
    if (cc) {
      ap[an-1] = (mp_limb_t)1 << (BITS_PER_MP_LIMB-1);
      MPFR_EXP(a)++;
    }

 end_of_add:
  TMP_FREE(marker); 
  return;
}

void
#if __STDC__
mpfr_add (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, mp_rnd_t rnd_mode) 
#else
mpfr_add (a, b, c, rnd_mode)
     mpfr_ptr a;
     mpfr_srcptr b;
     mpfr_srcptr c;
     mp_rnd_t rnd_mode;
#endif
{
  int diff_exp;

  if (MPFR_IS_NAN(b) || MPFR_IS_NAN(c)) {
    MPFR_SET_NAN(a); return;
  }
  
  MPFR_CLEAR_NAN(a);

  if (MPFR_IS_INF(b)) 
    { 
      if (MPFR_IS_INF(c)) 
	{
	  if (MPFR_SIGN(b) == MPFR_SIGN(c)) 
	    { 
	      MPFR_SET_INF(a); 
	      if (MPFR_SIGN(a) != MPFR_SIGN(b)) MPFR_CHANGE_SIGN(a);
	    }
	  else
	    MPFR_SET_NAN(a); 
	}
      else
	{
	  MPFR_SET_INF(a); 
	  if (MPFR_SIGN(b) != MPFR_SIGN(a)) MPFR_CHANGE_SIGN(a);
	}
      return;
    }
  else 
    if (MPFR_IS_INF(c))
      {
	MPFR_SET_INF(a); 
	if (MPFR_SIGN(c) != MPFR_SIGN(a)) MPFR_CHANGE_SIGN(a);
	return;
      }

  MPFR_CLEAR_INF(a); /* clear Inf flag */

  if (!MPFR_NOTZERO(b)) { mpfr_set(a, c, rnd_mode); return; }
  if (!MPFR_NOTZERO(c)) { mpfr_set(a, b, rnd_mode); return; }

  diff_exp = MPFR_EXP(b)-MPFR_EXP(c);
  if (MPFR_SIGN(b) * MPFR_SIGN(c) < 0) { /* signs differ, it's a subtraction */
    if (diff_exp<0) {
      mpfr_sub1(a, c, b, rnd_mode, -diff_exp);
    }
    else if (diff_exp>0) mpfr_sub1(a, b, c, rnd_mode, diff_exp);
    else { /* diff_exp=0 */
      diff_exp = mpfr_cmp3(b,c,-1);
      /* if b>0 and diff_exp>0 or b<0 and diff_exp<0: abs(b) > abs(c) */
      if (diff_exp==0) MPFR_SET_ZERO(a);
      else if (diff_exp * MPFR_SIGN(b)>0) mpfr_sub1(a, b, c, rnd_mode, 0);
      else mpfr_sub1(a, c, b, rnd_mode, 0);
    }
  }
  else /* signs are equal, it's an addition */
    if (diff_exp<0) mpfr_add1(a, c, b, rnd_mode, -diff_exp);
    else mpfr_add1(a, b, c, rnd_mode, diff_exp);
}

