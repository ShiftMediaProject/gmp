/* mpfr_sub -- subtract two floating-point numbers

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

/* #define DEBUG */

extern void mpfr_add1 _PROTO((mpfr_ptr, mpfr_srcptr, mpfr_srcptr, 
			      unsigned char, int));

/* put in ap[0]..ap[an-1] the value of bp[0]..bp[n-1] shifted by sh bits
   to the left minus ap[0]..ap[n-1], with 0 <= sh < mp_bits_per_limb, and
   returns the borrow.
*/
mp_limb_t
#if __STDC__ 
mpn_sub_lshift_n (mp_limb_t *ap, mp_limb_t *bp, int n, int sh, int an)
#else
mpn_sub_lshift_n (ap, bp, n, sh, an) mp_limb_t *ap, *bp; int n,sh,an;
#endif
{
  mp_limb_t c, bh;

  /* shift b to the left */
  if (sh) bh = mpn_lshift(bp, bp, n, sh);
  c = (n<an) ? mpn_sub_n(ap, bp, ap, n) : mpn_sub_n(ap, bp+(n-an), ap, an);
  /* shift back b to the right */
  if (sh) {
    mpn_rshift(bp, bp, n, sh);
    bp[n-1] += bh<<(mp_bits_per_limb-sh);
  }
  return c;
}

/* signs of b and c differ, abs(b)>=abs(c), diff_exp>=0 */
void 
#if __STDC__
mpfr_sub1(mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, unsigned char rnd_mode, int diff_exp) 
#else
mpfr_sub1(a, b, c, rnd_mode, diff_exp) 
     mpfr_ptr a;
     mpfr_srcptr b;
     mpfr_srcptr c; 
     unsigned char rnd_mode; 
     int diff_exp;
#endif
{
  mp_limb_t *ap, *bp, *cp, cc, c2; unsigned int an,bn,cn; 
  int sh,dif,k,cancel,cancel1,cancel2;
  TMP_DECL(marker);

#ifdef DEBUG
  printf("b=  "); if (SIGN(b)>=0) putchar(' ');
  mpfr_print_raw(b); putchar('\n');
  printf("c=  "); if (SIGN(c)>=0) putchar(' ');
  for (k=0;k<diff_exp;k++) putchar(' '); mpfr_print_raw(c);
  putchar('\n');
  printf("b=%1.20e c=%1.20e\n",mpfr_get_d(b),mpfr_get_d(c));
#endif
  cancel = mpfr_cmp2(b, c);
  /* we have to take into account the first (PREC(a)+cancel) bits from b */
  cancel1 = cancel/mp_bits_per_limb; cancel2 = cancel%mp_bits_per_limb;
  TMP_MARK(marker); 
  ap = MANT(a);
  bp = MANT(b);
  cp = MANT(c);
  if (ap == bp) {
    bp = (mp_ptr) TMP_ALLOC(ABSSIZE(b) * BYTES_PER_MP_LIMB); 
    MPN_COPY (bp, ap, ABSSIZE(b));
    if (ap == cp) { cp = bp; }
  }
  else if (ap == cp)
    {
      cp = (mp_ptr) TMP_ALLOC (ABSSIZE(c) * BYTES_PER_MP_LIMB);
      MPN_COPY(cp, ap, ABSSIZE(c)); 
    }
  an = (PREC(a)-1)/mp_bits_per_limb+1; /* number of significant limbs of a */
  sh = an*mp_bits_per_limb-PREC(a); /* non-significant bits in low limb */
  bn = (PREC(b)-1)/mp_bits_per_limb+1; /* number of significant limbs of b */
  cn = (PREC(c)-1)/mp_bits_per_limb + 1;
  EXP(a) = EXP(b)-cancel;
  /* adjust sign to that of b */
  if (SIGN(a)*SIGN(b)<0) CHANGE_SIGN(a);
  /* case 1: diff_exp>=prec(a), i.e. c only affects the last bit
     through rounding */
  dif = PREC(a)-diff_exp;
#ifdef DEBUG
  printf("PREC(a)=%d an=%u PREC(b)=%d bn=%u PREC(c)=%d diff_exp=%u dif=%d cancel=%d\n",
	 PREC(a),an,PREC(b),bn,PREC(c),diff_exp,dif,cancel);
#endif
  if (dif<=0) { /* diff_exp>=PREC(a): c does not overlap with a */
    /* either PREC(b)<=PREC(a), and we can copy the mantissa of b directly 
       into that of a, or PREC(b)>PREC(a) and we have to round b-c */
    if (PREC(b)<=PREC(a)+cancel) {
      if (cancel2) mpn_lshift(ap+(an-bn+cancel1), bp, bn-cancel1, cancel2);
      else MPN_COPY(ap+(an-bn+cancel1), bp, bn-cancel1);
      /* fill low significant limbs with zero */
      MPN_ZERO(ap, an-bn+cancel1);
      /* now take c into account */
      if (rnd_mode==GMP_RNDN) { /* to nearest */
	/* if diff_exp > PREC(a), no change */
	if (diff_exp==PREC(a)) {
	  /* if c is not zero, then as it is normalized, we have to subtract
	     one to the lsb of a if c>1/2, or c=1/2 and lsb(a)=1 (round to
	     even) */
	  if (NOTZERO(c)) { /* c is not zero */
	    /* check whether mant(c)=1/2 or not */
	    cc = *cp - ((mp_limb_t)1<<(mp_bits_per_limb-1));
	    if (cc==0) {
	      bp = cp+(PREC(c)-1)/mp_bits_per_limb;
	      while (cp<bp && cc==0) cc = *++cp;
	    }
	    if (cc || (ap[an-1] & (mp_limb_t)1<<sh)) goto sub_one_ulp;
	      /* mant(c) > 1/2 or mant(c) = 1/2: subtract 1 iff lsb(a)=1 */
	  }
	}
	else if (ap[an-1]==0) { /* case b=2^n */
	  ap[an-1] = (mp_limb_t) 1 << (BITS_PER_MP_LIMB-1);
	  EXP(a)++;
	}
      }
      else if ((ISNONNEG(b) && rnd_mode==GMP_RNDU) || 
	       (ISNEG(b) && rnd_mode==GMP_RNDD)) {
	/* round up: nothing to do */
	if (ap[an-1]==0) { /* case b=2^n */
	  ap[an-1] = (mp_limb_t) 1 << (BITS_PER_MP_LIMB-1);
	  EXP(a)++;
	}
      }
      else {
	/* round down: subtract 1 ulp iff c<>0 */
	if (NOTZERO(c)) goto sub_one_ulp;
      }
    }
    else { /* PREC(b)>PREC(a) : we have to round b-c */
      k=bn-an;
      /* first copy the 'an' most significant limbs of b to a */
      MPN_COPY(ap, bp+k, an);
      if (rnd_mode==GMP_RNDN) { /* to nearest */
	/* first check whether the truncated bits from b are 1/2*lsb(a) */
	if (sh) {
	  cc = *ap & (((mp_limb_t)1<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	  cc -= (mp_limb_t)1<<(sh-1);
	}
	else /* no bit to truncate */
	  cc = bp[--k] - ((mp_limb_t)1<<(mp_bits_per_limb-1));
	if ((long)cc>0) { /* suppose sizeof(long)=sizeof(mp_limb_t) */
	  goto add_one_ulp; /* trunc(b)>1/2*lsb(a) -> round up */
	}
	else if (cc==0) {
	  while (k>1 && cc==0) cc=bp[--k];
	  /* now if the truncated part of b = 1/2*lsb(a), check whether c=0 */
	  if (NOTZERO(c) || (*ap & ((mp_limb_t)1<<sh))) goto sub_one_ulp;
	  /* if trunc(b)-c is exactly 1/2*lsb(a) : round to even lsb */
	}
	/* if cc<0 : trunc(b) < 1/2*lsb(a) -> round down, i.e. do nothing */
      }
      else { /* round towards infinity or zero */
	if (sh) {
	  cc = *ap & (((mp_limb_t)1<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	}
	else cc=0;
	cn--;
        c2 = (dif>-sh) ? cp[cn]>>(mp_bits_per_limb-dif-sh) : 0;
	while (cc==c2 && (k || cn)) {
	  if (k) cc = bp[--k];
	  if (cn) {
	    c2 = cp[cn]<<(dif+sh);
	    if (--cn) c2 += cp[cn]>>(mp_bits_per_limb-dif-sh);
	  }
	}
	dif = ((ISNONNEG(b) && rnd_mode==GMP_RNDU) || 
	       (ISNEG(b) && rnd_mode==GMP_RNDD));
	/* round towards infinity if dif=1, towards zero otherwise */
	if (dif && cc>c2) goto add_one_ulp;
	else if (dif==0 && cc<c2) goto sub_one_ulp;
      }
    }
  }
  else { /* case 2: diff_exp < PREC(a) : c overlaps with a by dif bits */
    /* first copy upper part of c into a (after shift) */
    int overlap;
    dif += cancel;
    k = (dif-1)/mp_bits_per_limb + 1; /* only the highest k limbs from c
					 have to be considered */
    if (k<an) {
      MPN_ZERO(ap+k, an-k); /* do it now otherwise ap[k] may be 
				       destroyed in case dif<0 */
    }
#ifdef DEBUG
    printf("cancel=%d dif=%d k=%d cn=%d sh=%d\n",cancel,dif,k,cn,sh);
#endif
    if (dif<=PREC(c)) { /* c has to be truncated */
      dif = dif % mp_bits_per_limb;
      dif = (dif) ? mp_bits_per_limb-dif-sh : -sh;
      /* we have to shift by dif bits to the right */
      if (dif>0) {
	mpn_rshift(ap, cp+(cn-k), (k<=an) ? k : an, dif);
        if (k>an) ap[an-1] += cp[cn-k+an]<<(mp_bits_per_limb-dif);
      }
      else if (dif<0) {
	cc = mpn_lshift(ap, cp+(cn-k), k, -dif);
	if (k<an) ap[k]=cc;
	/* put the non-significant bits in low limb for further rounding */
	if (cn >= k+1)
	  ap[0] += cp[cn-k-1]>>(mp_bits_per_limb+dif);
      }
      else MPN_COPY(ap, cp+(cn-k), k);
      overlap=1;
    }
    else { /* c is not truncated, but we have to fill low limbs with 0 */
      MPN_ZERO(ap, k-cn);
      overlap = cancel-diff_exp;
#ifdef DEBUG
      printf("0:a="); mpfr_print_raw(a); putchar('\n');
      printf("overlap=%d\n",overlap);
#endif
      if (overlap>=0) {
	cn -= overlap/mp_bits_per_limb;
	overlap %= mp_bits_per_limb;
	/* warning: a shift of zero with mpn_lshift is not allowed */
	if (overlap) {
	  if (an<cn) {
	    mpn_lshift(ap, cp+(cn-an), an, overlap);
	    ap[0] += cp[cn-an-1]>>(mp_bits_per_limb-overlap);
	  }
	  else mpn_lshift(ap+(an-cn), cp, cn, overlap);
	}
	else MPN_COPY(ap+(an-cn), cp, cn);
      }
      else { /* shift to the right by -overlap bits */
	overlap = -overlap;
	k = overlap/mp_bits_per_limb;
	overlap = overlap % mp_bits_per_limb;
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
    if (PREC(b)<=PREC(a)+cancel) { int i, imax;
      overlap += 2;
      /* invert low limbs */
      imax = (int)an-(int)bn+cancel1;
      for (i=0;i<imax;i++) ap[i] = ~ap[i];
      cc = (i) ? mpn_add_1(ap, ap, i, 1) : 1;
      mpn_sub_lshift_n(ap+i, bp, bn-cancel1, cancel2, an);
      mpn_sub_1(ap+i, ap+i, an-i, (mp_limb_t)1-cc);
    }
    else /* PREC(b) > PREC(a): we have to truncate b */
      mpn_sub_lshift_n(ap, bp+(bn-an-cancel1), an, cancel2, an);
    /* remains to do the rounding */
#ifdef DEBUG
      printf("2:a="); mpfr_print_raw(a); putchar('\n');
      printf("overlap=%d\n",overlap);
#endif
    if (rnd_mode==GMP_RNDN) { /* to nearest */
      int kc;
      /* four cases: overlap =
         (0) PREC(b) > PREC(a) and diff_exp+PREC(c) <= PREC(a)
         (1) PREC(b) > PREC(a) and diff_exp+PREC(c) > PREC(a)
         (2) PREC(b) <= PREC(a) and diff_exp+PREC(c) <= PREC(a)
         (3)  PREC(b) <= PREC(a) and diff_exp+PREC(c) > PREC(a) */
      switch (overlap)
	{
        case 1: /* both b and c to round */
	  kc = cn-k; /* remains kc limbs from c */
	  k = bn-an; /* remains k limbs from b */
	  /* truncate last bits and store the difference with 1/2*ulp in cc */
	  cc = *ap & (((mp_limb_t)1<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	  cc -= (mp_limb_t)1<<(sh-1);
	  while ((cc==0 || cc==-1) && k!=0 && kc!=0) {
	    kc--;
	    cc -= mpn_sub_1(&c2, bp+(--k), 1, (cp[kc]>>dif) +
			    (cp[kc+1]<<(mp_bits_per_limb-dif)));
	    if (cc==0 || cc==-1) cc=c2;
	  }
	  if ((long)cc>0) goto add_one_ulp;
	  else if ((long)cc<-1) goto end_of_sub; /* the carry can be at most 1 */
	  else if (kc==0) goto round_b;
	  /* else round c: go through */
	case 3: /* only c to round */
	  bp=cp; k=cn-k; goto to_nearest;
	case 0: /* only b to round */
        round_b:
	  k=bn-an; dif=0; goto to_nearest;
        /* otherwise the result is exact: nothing to do */
	}
    }
    else if ((ISNONNEG(b) && rnd_mode==GMP_RNDU) || 
	     (ISNEG(b) && rnd_mode==GMP_RNDD)) {
      cc = *ap & (((mp_limb_t)1<<sh)-1);
      *ap &= ~cc; /* truncate last bits */
      if (cc) goto add_one_ulp; /* will happen most of the time */
      else { /* same four cases too */
	int kc = cn-k; /* remains kc limbs from c */
	switch (overlap)
	{
        case 1: /* both b and c to round */
	  k = bn-an; /* remains k limbs from b */
          dif = diff_exp % mp_bits_per_limb;
	  while (cc==0 && k!=0 && kc!=0) {
	    kc--;
	    cc = bp[--k] - (cp[kc]>>dif);
	    if (dif) cc -= (cp[kc+1]<<(mp_bits_per_limb-dif));
	  }
	  if (cc) goto add_one_ulp;
	  else if (kc==0) goto round_b2;
	  /* else round c: go through */
	case 3: /* only c to round: nothing to do */
	  /* while (kc) if (cp[--kc]) goto add_one_ulp; */
	  /* if dif>0 : remains to check last dif bits from c */
	  /* if (dif>0 && (cp[0]<<(mp_bits_per_limb-dif))) goto add_one_ulp; */
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
    else {
      cc = *ap & (((mp_limb_t)1<<sh)-1); 
      *ap &= ~cc;
      if (cc==0) { /* otherwise neglected difference cannot be < 0 */
	/* take into account bp[0]..bp[bn-cancel1-1] shifted by cancel2 to left
	   and cp[0]..cp[cn-k-1] shifted by dif bits to right */
	switch (overlap) {
	case 0:
	case 2:
	  break; /* c is not truncated ==> no borrow */
	case 1: /* both b and c are truncated */
	  break;
	case 3: /* only c is truncated */
	  cn -= k; /* take into account cp[0]..cp[cn-1] shifted by dif bits
		      to the right */
	  cc = (dif>0) ? cp[cn]<<(mp_bits_per_limb-dif) : 0;
	  while (cc==0 && cn>0) cc = cp[--cn];
          if (cc) goto sub_one_ulp;
	  break;
	}
      }
    }
  }
  goto end_of_sub;
    
  to_nearest: /* 0 <= sh < mp_bits_per_limb : number of bits of a to truncate
                 bp[k] : last significant limb from b */
#ifdef DEBUG
mpfr_print_raw(a); putchar('\n');
#endif
        if (sh) {
	  cc = *ap & (((mp_limb_t)1<<sh)-1);
	  *ap &= ~cc; /* truncate last bits */
	  c2 = (mp_limb_t)1<<(sh-1);
	}
	else /* no bit to truncate */
	  { if (k) cc = bp[--k]; else cc = 0; c2 = (mp_limb_t)1<<(mp_bits_per_limb-1); }
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
	  if (cc==0 && dif>0) cc=bp[0]<<(mp_bits_per_limb-dif);
	  /* now if the truncated part of b = 1/2*lsb(a), check whether c=0 */
          if (bp!=cp) { 
	    if (cc || (*ap & ((mp_limb_t)1<<sh))) goto add_one_ulp;
	  }
	  else {
	    /* subtract: if cc>0, do nothing */
	    if (cc==0 && (*ap & ((mp_limb_t)1<<sh))) goto add_one_ulp;
	  }
	}
        goto end_of_sub;

 sub_one_ulp:
    cc = mpn_sub_1(ap, ap, an, (mp_limb_t)1<<sh);
  goto end_of_sub;

  add_one_ulp: /* add one unit in last place to a */
    cc = mpn_add_1(ap, ap, an, (mp_limb_t)1<<sh);

 end_of_sub:
#ifdef DEBUG
printf("b-c="); if (SIGN(a)>0) putchar(' '); mpfr_print_raw(a); putchar('\n');
#endif
  TMP_FREE(marker);
  return;
}

void 
#if __STDC__
mpfr_sub(mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, unsigned char rnd_mode)
#else
mpfr_sub(a, b, c, rnd_mode) 
     mpfr_ptr a; 
     mpfr_srcptr b;
     mpfr_srcptr c; 
     unsigned char rnd_mode;
#endif
{
  int diff_exp;

  if (FLAG_NAN(b) || FLAG_NAN(c)) { SET_NAN(a); return; }

  if (!NOTZERO(b)) { mpfr_neg(a, c, rnd_mode); return; }
  if (!NOTZERO(c)) { mpfr_set(a, b, rnd_mode); return; }

  diff_exp = EXP(b)-EXP(c);
  if (SIGN(b) == SIGN(c)) { /* signs are equal, it's a real subtraction */
    if (diff_exp<0) {
      /* exchange rounding modes towards +/- infinity */
      if (rnd_mode==GMP_RNDU) rnd_mode=GMP_RNDD;
      else if (rnd_mode==GMP_RNDD) rnd_mode=GMP_RNDU;
      mpfr_sub1(a, c, b, rnd_mode, -diff_exp);
      CHANGE_SIGN(a);
    }
    else if (diff_exp>0) mpfr_sub1(a, b, c, rnd_mode, diff_exp);
    else { /* diff_exp=0 */
      diff_exp = mpfr_cmp3(b,c,1);
      /* if b>0 and diff_exp>0 or b<0 and diff_exp<0: abs(b) > abs(c) */
      if (diff_exp==0) SET_ZERO(a);
      else if (diff_exp*SIGN(b)>0) mpfr_sub1(a, b, c, rnd_mode, 0);
      else { 
	/* exchange rounding modes towards +/- infinity */
	if (rnd_mode==GMP_RNDU) rnd_mode=GMP_RNDD;
	else if (rnd_mode==GMP_RNDD) rnd_mode=GMP_RNDU;
	mpfr_sub1(a, c, b, rnd_mode, 0); 
	CHANGE_SIGN(a); 
      }
    }
  }
  else /* signs differ, it's an addition */
    if (diff_exp<0) {
      /* exchange rounding modes towards +/- infinity */
      if (rnd_mode==GMP_RNDU) rnd_mode=GMP_RNDD;
      else if (rnd_mode==GMP_RNDD) rnd_mode=GMP_RNDU;
      mpfr_add1(a, c, b, rnd_mode, -diff_exp);
      CHANGE_SIGN(a); 
    }
    else mpfr_add1(a, b, c, rnd_mode, diff_exp);
}

