/* mpfr_sub -- subtract two floating-point numbers

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

#define ONE ((mp_limb_t) 1)

extern void mpfr_add1 _PROTO((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, 
			      mp_rnd_t, int));
mp_limb_t mpn_sub_lshift_n _PROTO ((mp_limb_t *, mp_limb_t *, int, int, int));
void mpfr_sub1 _PROTO ((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, mp_rnd_t, int));

/* put in ap[0]..ap[an-1] the value of bp[0]..bp[n-1] shifted by sh bits
   to the left minus ap[0]..ap[n-1], with 0 <= sh < BITS_PER_MP_LIMB, and
   returns the borrow.
*/
mp_limb_t
#if __STDC__ 
mpn_sub_lshift_n (mp_limb_t *ap, mp_limb_t *bp, int n, int sh, int an)
#else
mpn_sub_lshift_n (ap, bp, n, sh, an)
     mp_limb_t *ap, *bp;
     int n,sh,an;
#endif
{
  mp_limb_t c, bh=0;

  /* shift b to the left */
  if (sh) bh = mpn_lshift(bp, bp, n, sh);
  c = (n<an) ? mpn_sub_n(ap, bp, ap, n) : mpn_sub_n(ap, bp+(n-an), ap, an);
  /* shift back b to the right */
  if (sh) {
    mpn_rshift(bp, bp, n, sh);
    bp[n-1] += bh<<(BITS_PER_MP_LIMB-sh);
  }
  return c;
}

/* signs of b and c differ, abs(b)>=abs(c), diff_exp>=0 */
void 
#if __STDC__
mpfr_sub1 (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c,
	   mp_rnd_t rnd_mode, int diff_exp) 
#else
mpfr_sub1 (a, b, c, rnd_mode, diff_exp) 
     mpfr_ptr a;
     mpfr_srcptr b;
     mpfr_srcptr c;
     mp_rnd_t rnd_mode;
     int diff_exp;
#endif
{
  mp_limb_t *ap, *bp, *cp, cc, c2; unsigned int an, bn, cn;
  int sh,dif,k,cancel,cancel1,cancel2;
  TMP_DECL(marker);

#ifdef DEBUG
  printf("b=  "); if (MPFR_SIGN(b)>=0) putchar(' ');
  mpfr_print_raw(b); putchar('\n');
  printf("c=  "); if (MPFR_SIGN(c)>=0) putchar(' ');
  for (k=0;k<diff_exp;k++) putchar(' '); mpfr_print_raw(c);
  putchar('\n');
  printf("b=%1.20e c=%1.20e\n",mpfr_get_d(b),mpfr_get_d(c));
#endif
  cancel = mpfr_cmp2(b, c);
  /* we have to take into account the first (MPFR_PREC(a)+cancel) bits from b */
  cancel1 = cancel/BITS_PER_MP_LIMB; cancel2 = cancel%BITS_PER_MP_LIMB;
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
  bn = (MPFR_PREC(b)-1)/BITS_PER_MP_LIMB+1; /* number of significant limbs of b */
  cn = (MPFR_PREC(c)-1)/BITS_PER_MP_LIMB + 1;
  MPFR_EXP(a) = MPFR_EXP(b)-cancel;
  /* adjust sign to that of b */
  if (MPFR_SIGN(a)*MPFR_SIGN(b)<0) MPFR_CHANGE_SIGN(a);
  /* case 1: diff_exp>=prec(a), i.e. c only affects the last bit
     through rounding */
  dif = MPFR_PREC(a)-diff_exp;
#ifdef DEBUG
  printf("MPFR_PREC(a)=%d an=%u MPFR_PREC(b)=%d bn=%u MPFR_PREC(c)=%d diff_exp=%u dif=%d cancel=%d\n",
	 MPFR_PREC(a),an,MPFR_PREC(b),bn,MPFR_PREC(c),diff_exp,dif,cancel);
#endif
  if (dif<=0) { /* diff_exp>=MPFR_PREC(a): c does not overlap with a */
    /* either MPFR_PREC(b)<=MPFR_PREC(a), and we can copy the mantissa of b directly 
       into that of a, or MPFR_PREC(b)>MPFR_PREC(a) and we have to round b-c */
    if (MPFR_PREC(b)<=MPFR_PREC(a)+cancel) {
      if (cancel2) mpn_lshift(ap+(an-bn+cancel1), bp, bn-cancel1, cancel2);
      else MPN_COPY(ap+(an-bn+cancel1), bp, bn-cancel1);
      /* fill low significant limbs with zero */
      MPN_ZERO(ap, an-bn+cancel1);
      /* now take c into account */
      if (rnd_mode==GMP_RNDN) { /* to nearest */
	/* if diff_exp > MPFR_PREC(a), no change */
	if (diff_exp==MPFR_PREC(a)) {
	  /* if c is not zero, then as it is normalized, we have to subtract
	     one to the lsb of a if c>1/2, or c=1/2 and lsb(a)=1 (round to
	     even) */
	  if (MPFR_NOTZERO(c)) { /* c is not zero */
	    /* check whether mant(c)=1/2 or not */
	    cc = *cp - (ONE<<(BITS_PER_MP_LIMB-1));
	    if (cc==0) {
	      bp = cp+(MPFR_PREC(c)-1)/BITS_PER_MP_LIMB;
	      while (cp<bp && cc==0) cc = *++cp;
	    }
	    if (cc || (ap[an-1] & ONE<<sh)) goto sub_one_ulp;
	      /* mant(c) > 1/2 or mant(c) = 1/2: subtract 1 iff lsb(a)=1 */
	  }
	}
	else if (ap[an-1]==0) { /* case b=2^n */
	  ap[an-1] = ONE << (BITS_PER_MP_LIMB-1);
	  MPFR_EXP(a)++;
	}
      }
      else if ((MPFR_ISNONNEG(b) && rnd_mode==GMP_RNDU) || 
	       (MPFR_ISNEG(b) && rnd_mode==GMP_RNDD)) {
	/* round up: nothing to do */
	if (ap[an-1]==0) { /* case b=2^n */
	  ap[an-1] = ONE << (BITS_PER_MP_LIMB-1);
	  MPFR_EXP(a)++;
	}
      }
      else {
	/* round down: subtract 1 ulp iff c<>0 */
	if (MPFR_NOTZERO(c)) goto sub_one_ulp;
      }
    }
    else { /* MPFR_PREC(b)>MPFR_PREC(a) : we have to round b-c */
      k=bn-an;
      /* first copy the 'an' most significant limbs of b to a */
      MPN_COPY(ap, bp+k, an);
      { /* treat all rounding modes together */
	mp_limb_t c2old; int sign=0; long int cout=0;

	if (sh) {
	  cc = *ap & ((ONE<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	}
	else cc=0;

        dif += sh;
	if (dif>0) { /* c overlaps by dif bits with that last unused sh bits
			from least significant limb of b */
	  cn--;
	  c2old = cp[cn]; /* last limb from c considered */
	  cout -= mpn_sub_1(&cc, &cc, 1, c2old >> (BITS_PER_MP_LIMB-dif));
	}
	else c2old = 0;

	/* trailing bits from b - c are -cout*2^BITS_PER_MP_LIMB + cc,
	   last considered limb of c is c2old, remains to take into account
	   its BITS_PER_MP_LIMB-dif low bits */

	if (sh && rnd_mode==GMP_RNDN) {
	  if (cout>=0) {
	    sign = 1;
	    cout -= mpn_sub_1(&cc, &cc, 1, ONE<<(sh-1));
	  }
	  else {
	    sign = -1;
	    cout += mpn_add_1(&cc, &cc, 1, ONE<<(sh-1));
	  }
	}

	if (cout==0) { /* if cout<0, it will remain negative */
	   if (dif<0) dif += BITS_PER_MP_LIMB;
	   while (cout==0 && (k || cn)) {
	     cout = cc;
	     cc = (k) ? bp[--k] : 0;
	     if (sh==0) {
	       if (cout>=0) {
		 sign = 1;
		 cout -= mpn_sub_1(&cc, &cc, 1, ONE << (BITS_PER_MP_LIMB-1));
	       }
	       else {
		 sign = -1;
		 cout += mpn_add_1(&cc, &cc, 1, ONE << (BITS_PER_MP_LIMB-1));
	       }
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
	     cout -= mpn_sub_1(&cc, &cc, 1, c2);
	   }
	}
	if (cout==0) cout=(cc!=0);
        if (rnd_mode==GMP_RNDU) sign=1; 
	else if (rnd_mode==GMP_RNDD || rnd_mode==GMP_RNDZ) sign=-1;
	/* even rounding rule: */
	if (rnd_mode==GMP_RNDN) {
	  if (cout==0 && (*ap & (ONE<<sh))) cout=sign;
	}
	else /* rounding does not depend from sign of b for GMP_RNDN */
	  if (MPFR_ISNEG(b)) sign=-sign;
	/* round towards infinity if sign=1, towards zero otherwise */
	if ((sign==1) && cout>0) goto add_one_ulp;
	else if ((sign==-1) && cout<0) goto sub_one_ulp;
      }
    }
  }
  else { /* case 2: diff_exp < MPFR_PREC(a) : c overlaps with a by dif bits */
    /* first copy upper part of c into a (after shift) */
    int overlap;
    dif += cancel;
    k = (dif-1)/BITS_PER_MP_LIMB + 1; /* only the highest k limbs from c
					 have to be considered */
    if (k<an) {
      MPN_ZERO(ap+k, an-k); /* do it now otherwise ap[k] may be 
				       destroyed in case dif<0 */
    }
#ifdef DEBUG
    printf("cancel=%d dif=%d k=%d cn=%d sh=%d\n",cancel,dif,k,cn,sh);
#endif
    if (dif<=MPFR_PREC(c)) { /* c has to be truncated */
      dif = dif % BITS_PER_MP_LIMB;
      dif = (dif) ? BITS_PER_MP_LIMB-dif-sh : -sh;
      /* we have to shift by dif bits to the right */
      if (dif>0) {
	mpn_rshift(ap, cp+(cn-k), (k<=an) ? k : an, dif);
        if (k>an) ap[an-1] += cp[cn-k+an]<<(BITS_PER_MP_LIMB-dif);
      }
      else if (dif<0) {
	cc = mpn_lshift(ap, cp+(cn-k), (k<=an) ? k : an, -dif);
	if (k<an) ap[k]=cc;
	/* put the non-significant bits in low limb for further rounding */
	if (cn >= k+1)
	  ap[0] += cp[cn-k-1]>>(BITS_PER_MP_LIMB+dif);
      }
      else MPN_COPY(ap, cp+(cn-k), (k<=an) ? k : an);
      overlap=1;
    }
    else { /* c is not truncated, but we have to fill low limbs with 0 */
      MPN_ZERO(ap, (k-cn<an) ? k-cn : an);
      overlap = cancel-diff_exp;
#ifdef DEBUG
      printf("0:a="); mpfr_print_raw(a); putchar('\n');
      printf("overlap=%d\n",overlap);
#endif
      if (overlap>=0) {
        if (overlap/BITS_PER_MP_LIMB <= cn) 
	  cn -= overlap/BITS_PER_MP_LIMB;
	else cn=0;
	overlap %= BITS_PER_MP_LIMB;
	/* warning: a shift of zero with mpn_lshift is not allowed */
	if (overlap) {
	  if (an<cn) {
	    mpn_lshift(ap, cp+(cn-an), an, overlap);
	    ap[0] += cp[cn-an-1]>>(BITS_PER_MP_LIMB-overlap);
	  }
	  else {
	    /* warning: mpn_lshift doesn't seem to like cn=0 */
	    if (cn) mpn_lshift(ap+(an-cn), cp, cn, overlap); 
	  }
	}
	else MPN_COPY(ap+(an-cn), cp, cn);
      }
      else { /* shift to the right by -overlap bits */
	overlap = -overlap;
	k = overlap/BITS_PER_MP_LIMB;
	overlap = overlap % BITS_PER_MP_LIMB;
	if (overlap) cc = mpn_rshift(ap+(an-k-cn), cp, cn, overlap);
	else {
	  MPN_COPY(ap+(an-k-cn), cp, cn);
	  cc = 0;
	}
	if (an>k+cn) ap[an-k-cn-1]=cc;
      }
      overlap=0;
    }
#ifdef DEBUG
      printf("1:a="); mpfr_print_raw(a); putchar('\n');
#endif
    /* here overlap=1 iff ulp(c)<ulp(a) */
    /* then put high limbs to zero */
    /* now add 'an' upper limbs of b in place */
    if (MPFR_PREC(b)<=MPFR_PREC(a)+cancel) { int i, imax;
      overlap += 2;
      /* invert low limbs */
      imax = (int)an - (int)bn + cancel1;
      if (imax > (int)an) imax = an;
      for (i=0;i<imax;i++) ap[i] = ~ap[i];
      cc = (i) ? mpn_add_1(ap, ap, i, 1) : 1;
      if (cancel1 < bn) mpn_sub_lshift_n(ap+i, bp, bn-cancel1, cancel2, an);
      /* warning: mpn_sub_1 doesn't accept a zero length */
      if (i < an) mpn_sub_1(ap+i, ap+i, an-i, ONE-cc);
    }
    else /* MPFR_PREC(b) > MPFR_PREC(a): we have to truncate b */
      mpn_sub_lshift_n(ap, bp+(bn-an-cancel1), an, cancel2, an);
    /* remains to do the rounding */
#ifdef DEBUG
      printf("2:a="); mpfr_print_raw(a); putchar('\n');
      printf("overlap=%d\n",overlap);
#endif
    if (rnd_mode==GMP_RNDN) { /* to nearest */
      int kc;
      /* four cases: overlap =
         (0) MPFR_PREC(b) > MPFR_PREC(a) and diff_exp+MPFR_PREC(c) <= MPFR_PREC(a)
         (1) MPFR_PREC(b) > MPFR_PREC(a) and diff_exp+MPFR_PREC(c) > MPFR_PREC(a)
         (2) MPFR_PREC(b) <= MPFR_PREC(a) and diff_exp+MPFR_PREC(c) <= MPFR_PREC(a)
         (3)  MPFR_PREC(b) <= MPFR_PREC(a) and diff_exp+MPFR_PREC(c) > MPFR_PREC(a) */
      switch (overlap)
	{
        case 1: /* both b and c to round */
	  kc = cn-k; /* remains kc limbs from c */
	  k = bn-an; /* remains k limbs from b */
	  /* truncate last bits and store the difference with 1/2*ulp in cc */
	  c2 = *ap & ((ONE<<sh)-1);
	  *ap &= ~c2; /* truncate last bits */
	  cc = -mpn_sub_1(&c2, &c2, 1, ONE<<(sh-1));
	  if (cc==0) cc=c2;
	  /* loop invariant: cc*2^BITS_PER_MP_LIMB+c2 is the current difference
	     between b - 1/2*ulp(a) and c shifted by dif bits to the right.
	     cc > 0 ==> add one ulp
	     cc < 0 ==> truncate
	     cc = 0 ==> go to next limb
	  */
	  while ((cc==0) && (k>=0 || kc>=0)) {
	    k--; kc--;
	    if (k>=0) {
	       if (kc>=0) cc -= mpn_sub_1(&c2, bp+k, 1, (cp[kc]>>dif) +
					 (cp[kc+1]<<(BITS_PER_MP_LIMB-dif)));
	       else /* don't forget last right chunck from c */
		 cc -= mpn_sub_1(&c2, bp+k, 1, cp[0]<<(BITS_PER_MP_LIMB-dif));
	    }
	    else { /* no more limb from b */
	      /* warning: if k was 0, kc can be negative here */
	      if ((kc+1>=0) && (cp[kc+1]<<(BITS_PER_MP_LIMB-dif))) cc=-1;
	      else while ((cc==0) && (kc>=0)) {
		if (cp[kc]) cc=-1;
		kc--;
	      }
	    }
	    if (cc==0) cc=c2;
	  }
	  /* cc should either 0 or -1 here */
	  if ((int)cc>0) goto add_one_ulp;
	  else if ((int)cc<0) goto end_of_sub; /* carry can be at most 1 */
	  else /* cc=0 */
	    {
	      if (c2 || (*ap & (ONE<<sh))) goto add_one_ulp;
	      else goto end_of_sub;
	    }
	  /* else round c: go through */
	case 3: /* only c to round */
	  bp=cp; k=cn-k; goto to_nearest;
	case 0: /* only b to round */
	  k=bn-an; dif=0; goto to_nearest;
        /* otherwise the result is exact: nothing to do */
	}
    }
    else if ((MPFR_ISNONNEG(b) && rnd_mode==GMP_RNDU) || 
	     (MPFR_ISNEG(b) && rnd_mode==GMP_RNDD)) {
      cc = *ap & ((ONE<<sh)-1);
      *ap &= ~cc; /* truncate last bits */
      if (cc) goto add_one_ulp; /* will happen most of the time */
      else { /* same four cases too */
	int kc = cn-k; /* remains kc limbs from c */
	switch (overlap)
	{
        case 1: /* both b and c to round */
	  k = bn-an; /* remains k limbs from b */
          dif = diff_exp % BITS_PER_MP_LIMB;
	  while (cc==0 && k!=0 && kc!=0) {
	    kc--;
	    cc = bp[--k] - (cp[kc]>>dif);
	    if (dif) cc -= (cp[kc+1]<<(BITS_PER_MP_LIMB-dif));
	  }
	  if (cc) goto add_one_ulp;
	  else if (kc==0) goto round_b2;
	  /* else round c: go through */
	case 3: /* only c to round: nothing to do */
	  /* while (kc) if (cp[--kc]) goto add_one_ulp; */
	  /* if dif>0 : remains to check last dif bits from c */
	  /* if (dif>0 && (cp[0]<<(BITS_PER_MP_LIMB-dif))) goto add_one_ulp; */
	  break;
	case 0: /* only b to round */
        round_b2:
	  k=bn-an;
	  while (k) if (bp[--k]) goto add_one_ulp;
        /* otherwise the result is exact: nothing to do */
	}
      }
    }
    /* else round to zero: remove 1 ulp if neglected bits from b-c are < 0 */
    else { int kc=cn-k;
      cc = *ap & ((ONE<<sh)-1); 
      *ap &= ~cc;
      if (cc==0) { /* otherwise neglected difference cannot be < 0 */
	/* take into account bp[0]..bp[bn-cancel1-1] shifted by cancel2 to left
	   and cp[0]..cp[cn-k-1] shifted by dif bits to right */
	switch (overlap) { 
	case 0:
	case 2:
	  break; /* c is not truncated ==> no borrow */
	case 1: /* both b and c are truncated */
	  k = bn-an; /* remains k limbs from b */
          dif = diff_exp % BITS_PER_MP_LIMB;
	  while (k!=0 && kc!=0) {
	    kc--;
	    cc = cp[kc]>>dif;
	    if (dif) cc += cp[kc+1]<<(BITS_PER_MP_LIMB-dif);
	    k--;
	    if (bp[k]>cc) goto end_of_sub;
	    else if (bp[k]<cc) goto sub_one_ulp;
	  }
	  if (k==0) { /* is the remainder from c zero or not? */
	    while (kc!=0) {
	      kc--;
	      cc = cp[kc]>>dif;
	      if (dif) cc += cp[kc+1]<<(BITS_PER_MP_LIMB-dif);
	      if (cc) goto sub_one_ulp;
	    }
	    if (cp[0]<<(BITS_PER_MP_LIMB-dif)) goto sub_one_ulp;
	  }
	  break;
	case 3: /* only c is truncated */
	  cn -= k; /* take into account cp[0]..cp[cn-1] shifted by dif bits
		      to the right */
	  cc = (dif>0) ? cp[cn]<<(BITS_PER_MP_LIMB-dif) : 0;
	  while (cc==0 && cn>0) cc = cp[--cn];
          if (cc) goto sub_one_ulp;
	  break;
	}
      }
    }
  }
  goto end_of_sub;
    
  to_nearest: /* 0 <= sh < BITS_PER_MP_LIMB : number of bits of a to truncate
                 bp[k] : last significant limb from b */
#ifdef DEBUG
mpfr_print_raw(a); putchar('\n');
#endif
        if (sh) {
	  cc = *ap & ((ONE<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	  c2 = ONE<<(sh-1);
	}
	else /* no bit to truncate */
	  { if (k) cc = bp[--k]; else cc = 0; c2 = ONE<<(BITS_PER_MP_LIMB-1); }
#ifdef DEBUG
	printf("cc=%lu c2=%lu k=%u\n",cc,c2,k);
#endif
	if (cc>c2) goto add_one_ulp; /* trunc(b)>1/2*lsb(a) -> round up */
	else if (cc==c2) {
	  cc=0; while (k && cc==0) cc=bp[--k];
#ifdef DEBUG
	  printf("cc=%lu\n",cc);
#endif
	  /* special case of rouding c shifted to the right */
	  if (cc==0 && dif>0) cc=bp[0]<<(BITS_PER_MP_LIMB-dif);
	  /* now if the truncated part of b = 1/2*lsb(a), check whether c=0 */
          if (bp!=cp) { 
	    if (cc || (*ap & (ONE<<sh))) goto add_one_ulp;
	  }
	  else {
	    /* subtract: if cc>0, do nothing */
	    if (cc==0 && (*ap & (ONE<<sh))) goto add_one_ulp;
	  }
	}
        goto end_of_sub;

 sub_one_ulp:
    cc = mpn_sub_1(ap, ap, an, ONE<<sh);
  goto end_of_sub;

  add_one_ulp: /* add one unit in last place to a */
    cc = mpn_add_1(ap, ap, an, ONE<<sh);

 end_of_sub:
#ifdef DEBUG
printf("b-c="); if (MPFR_SIGN(a)>0) putchar(' '); mpfr_print_raw(a); putchar('\n');
#endif
  TMP_FREE(marker);
  return;
}

void 
#if __STDC__
mpfr_sub (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, mp_rnd_t rnd_mode)
#else
mpfr_sub (a, b, c, rnd_mode) 
     mpfr_ptr a;
     mpfr_srcptr b;
     mpfr_srcptr c;
     mp_rnd_t rnd_mode;
#endif
{
  int diff_exp;

  if (MPFR_IS_NAN(b) || MPFR_IS_NAN(c)) {
    MPFR_SET_NAN(a);
    return;
  }
  
  MPFR_CLEAR_NAN(a);

  if (MPFR_IS_INF(b)) 
    { 
      if (MPFR_IS_INF(c)) 
	{
	  if (MPFR_SIGN(b) != MPFR_SIGN(c)) 
	    { 
	      MPFR_SET_INF(a); 
	      if (MPFR_SIGN(a) != MPFR_SIGN(b)) { MPFR_CHANGE_SIGN(a); }
	    }
	  else
	    MPFR_SET_NAN(a); 
	}
      else
	{
	  MPFR_SET_INF(a); 
	  if (MPFR_SIGN(b) != MPFR_SIGN(a)) { MPFR_CHANGE_SIGN(a); }
	}
      return;
    }
  else 
    if (MPFR_IS_INF(c))
      {
	MPFR_SET_INF(a); 
	if (MPFR_SIGN(c) == MPFR_SIGN(a)) { MPFR_CHANGE_SIGN(a); }
	return;
      }

  if (!MPFR_NOTZERO(b)) { mpfr_neg(a, c, rnd_mode); return; }
  if (!MPFR_NOTZERO(c)) { mpfr_set(a, b, rnd_mode); return; }

  MPFR_CLEAR_INF(a);

  diff_exp = MPFR_EXP(b)-MPFR_EXP(c);
  if (MPFR_SIGN(b) == MPFR_SIGN(c)) {
    /* signs are equal, it's a real subtraction */
    if (diff_exp<0) {
      /* exchange rounding modes towards +/- infinity */
      if (rnd_mode==GMP_RNDU) rnd_mode=GMP_RNDD;
      else if (rnd_mode==GMP_RNDD) rnd_mode=GMP_RNDU;
      mpfr_sub1(a, c, b, rnd_mode, -diff_exp);
      MPFR_CHANGE_SIGN(a);
    }
    else if (diff_exp>0) mpfr_sub1(a, b, c, rnd_mode, diff_exp);
    else { /* diff_exp=0 */
      diff_exp = mpfr_cmp3(b,c,1);
      /* if b>0 and diff_exp>0 or b<0 and diff_exp<0: abs(b) > abs(c) */
      if (diff_exp==0) MPFR_SET_ZERO(a);
      else if (diff_exp*MPFR_SIGN(b)>0) mpfr_sub1(a, b, c, rnd_mode, 0);
      else { 
	/* exchange rounding modes towards +/- infinity */
	if (rnd_mode==GMP_RNDU) rnd_mode=GMP_RNDD;
	else if (rnd_mode==GMP_RNDD) rnd_mode=GMP_RNDU;
	mpfr_sub1(a, c, b, rnd_mode, 0); 
	MPFR_CHANGE_SIGN(a); 
      }
    }
  }
  else /* signs differ, it's an addition */
    if (diff_exp<0) {
      /* exchange rounding modes towards +/- infinity */
      if (rnd_mode==GMP_RNDU) rnd_mode=GMP_RNDD;
      else if (rnd_mode==GMP_RNDD) rnd_mode=GMP_RNDU;
      mpfr_add1(a, c, b, rnd_mode, -diff_exp);
      MPFR_CHANGE_SIGN(a); 
    }
    else mpfr_add1(a, b, c, rnd_mode, diff_exp);
}

