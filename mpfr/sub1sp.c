/* mpfr_sub1sp -- internal function to perform a "real" substraction
   All the op must have the same precision

Copyright 2003 Free Software Foundation.
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

#include <stdio.h>
#include "mpfr-impl.h"
#include "longlong.h"

/* 
   compute sgn(b)*(|b| - |c|) if |b|>|c| else -sgn(b)*(|c| -|b|)
   Returns 0 iff result is exact,
   a negative value when the result is less than the exact value,
   a positive value otherwise.
*/

/* #define DEBUG */

#ifdef DEBUG
# undef DEBUG
# define DEBUG(x) (x)
#else
# define DEBUG(x) /**/
#endif
 
/* Rounding Sub */
      
/* A0...Ap-1
 *          Cp Cp+1 ....
 *             <- C'p+1 ->
 * Cp = -1 if calculated from c mantissa
 * Cp = 0  if 0 from a or c
 * Cp = 1  if calculated from a.
 * C'p+1 = Frist bit not null or 0 if there isn't one 
 *
 * Can't have Cp=-1 and C'p+1=1*/

/* RND = GMP_RNDZ:
 *  + if Cp=0 and C'p+1=0,1,  Truncate.
 *  + if Cp=0 and C'p+1=-1,   SubOneUlp
 *  + if Cp=-1,               SubOneUlp
 *  + if Cp=1,                AddOneUlp
 * RND = GMP_RNDA (Away)
 *  + if Cp=0 and C'p+1=0,-1, Truncate
 *  + if Cp=0 and C'p+1=1,    AddOneUlp
 *  + if Cp=1,                AddOneUlp
 *  + if Cp=-1,               Truncate
 * RND = GMP_RNDN
 *  + if Cp=0,                Truncate
 *  + if Cp=1 and C'p+1=1,    AddOneUlp
 *  + if Cp=1 and C'p+1=-1,   Truncate
 *  + if Cp=1 and C'p+1=0,    Truncate if Ap-1=0, AddOneUlp else
 *  + if Cp=-1 and C'p+1=-1,  SubOneUlp
 *  + if Cp=-1 and C'p+1=0,   Truncate if Ap-1=0, SubOneUlp else
 * 
 * If AddOneUlp:
 *   If carry, then it is 11111111111 + 1 = 10000000000000
 *      ap[n-1]=MPFR_HIGHT_BIT
 * If SubOneUlp:
 *   If we lose one bit, it is 1000000000 - 1 = 0111111111111
 *      Then shift, and put as last bit x which is calculated 
 *              according Cp, Cp-1 and rnd_mode.
 * If Truncate,
 *    If it is a power of 2,
 *       we may have to suboneulp in some special cases.
 *
 * To simplify, we don't use Cp = 1.
 *
 */

int
mpfr_sub1sp (mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, mp_rnd_t rnd_mode)
{
  mp_exp_t bx,cx;
  mp_exp_unsigned_t d;
  mpfr_prec_t p, sh, cnt;
  mp_size_t n;
  mp_limb_t *ap, *bp, *cp;
  mp_limb_t limb;
  int inexact;
  mp_limb_t bcp,bcp1; /* Cp and C'p+1 */
  mp_limb_t bbcp, bbcp1; /* Cp+1 and C'p+2 */
  TMP_DECL(marker);

  TMP_MARK(marker);
  
  MPFR_ASSERTD(MPFR_PREC(a) == MPFR_PREC(b) && MPFR_PREC(b) == MPFR_PREC(c));
  MPFR_ASSERTD(MPFR_IS_PURE_FP(b) && MPFR_IS_PURE_FP(c));

  /* Read prec and num of limbs */
  p = MPFR_PREC(b);
  n = (p-1)/BITS_PER_MP_LIMB+1;

  /* Fast cmp of |b| and |c|*/
  bx = MPFR_GET_EXP (b);
  cx = MPFR_GET_EXP (c);
  if (MPFR_UNLIKELY(bx == cx))
    {
      mp_size_t k = n - 1;
      /* Check mantissa since exponent are equals */
      bp = MPFR_MANT(b);
      cp = MPFR_MANT(c);
      while (k>=0 && bp[k] == cp[k])
        k--;
      if (MPFR_UNLIKELY(k < 0))
        /* b == c ! */
        {
	  /* Return exact number 0 */
	  if (rnd_mode == GMP_RNDD)
	    MPFR_SET_NEG(a);
	  else
	    MPFR_SET_POS(a);
	  MPFR_SET_ZERO(a);
	  MPFR_RET(0);
	}
      else if (bp[k] > cp[k])
        goto BGreater;
      else
        {
	  MPFR_ASSERTD(bp[k]<cp[k]);
	  goto CGreater;
	}
    }   
  else if (MPFR_UNLIKELY(bx < cx))
    {
      /* Swap b and c and set sign */
      mpfr_srcptr t;
      mp_exp_t tx;
    CGreater:
      MPFR_SET_OPPOSITE_SIGN(a,b);
      t  = b;  b  = c;  c  = t;
      tx = bx; bx = cx; cx = tx;
    }
  else
    {
      /* b > c */
    BGreater:
      MPFR_SET_SAME_SIGN(a,b);
    }

  /* Now b > c */
  MPFR_ASSERTD(bx >= cx);
  d = (mp_exp_unsigned_t) bx - cx;
  DEBUG( printf("New with diff=%lu\n", d) );

  /* Transform RNDU and RNDD to RNDA or RNDZ */
  if (MPFR_IS_RNDUTEST_OR_RNDDNOTTEST(rnd_mode, MPFR_IS_NEG(a)))
    rnd_mode = GMP_RNDZ;

  if (d <=1)
    {
      if (d == 0)
	{
	  /* <-- b -->	
	     <-- c --> : exact sub */
	  ap = MPFR_MANT(a);
	  mpn_sub_n (ap, MPFR_MANT(b), MPFR_MANT(c), n);
	  /* Normalize */
	ExactNormalize:
	  limb = ap[n-1];
	  if (limb)
	    {
	      /* First limb is not zero. */
	      count_leading_zeros(cnt, limb);
	      if (MPFR_LIKELY(cnt))
		{
		  mpn_lshift(ap, ap, n, cnt); /* Normalize number */
		  bx -= cnt; /* Update final expo */
		}
	      sh = (-p % BITS_PER_MP_LIMB);
	      if (MPFR_LIKELY(sh))
		ap[0] &= ~((MPFR_LIMB_ONE << sh) - MPFR_LIMB_ONE);
	    }
	  else
	    {
	      /* First limb is zero */
	      mp_size_t k = n-1, len;
	      /* Find the first limb not equal to zero.
		 FIXME:It is assume it exists (since |b| > |c| and same prec)*/
	      do {
		MPFR_ASSERTD( k > 0 );
		limb = ap[--k];
	      } while (limb == 0);
	      MPFR_ASSERTD(limb != 0);
	      count_leading_zeros(cnt, limb);
	      k++;
	      len = n - k; /* Number of last limb */
	      MPFR_ASSERTD(k >= 0);
	      mpn_lshift(ap+len, ap, k, cnt); /* Normalize the High Limb*/
	      MPN_ZERO(ap, len); /* Zeroing the last limbs */
	      bx -= cnt + len*BITS_PER_MP_LIMB; /* Update Expo */
	      sh = (-p % BITS_PER_MP_LIMB);
	      if (MPFR_LIKELY(sh))
		ap[len] &= ~((MPFR_LIMB_ONE << sh) - MPFR_LIMB_ONE);
	    }
	  /* Check expo underflow */
	  if (MPFR_UNLIKELY(bx < __gmpfr_emin))
	    {
	      TMP_FREE(marker);
	      /* inexact=0 */
	      DEBUG( printf("(D==0 Underflow)\n") );
	      if (rnd_mode == GMP_RNDN &&
		  (bx < __gmpfr_emin - 1 ||
		   (/*inexact >= 0 &&*/ mpfr_powerof2_raw (a))))
		rnd_mode = GMP_RNDZ;
	      return mpfr_set_underflow (a, rnd_mode, MPFR_SIGN(a));
	    }
	  MPFR_SET_EXP (a, bx);
	  /* No rounding is necessary since the result is exact */
	  MPFR_ASSERTD(ap[n-1] > ~ap[n-1]);
	  TMP_FREE(marker);
	  return 0;
	}
      else /* if (d == 1) */
	{
	  /* | <-- b -->
	     |  <-- c --> */
	  mp_limb_t c0, mask;
	  mp_size_t k;
	  sh = -p % BITS_PER_MP_LIMB;
	  /* If we lose at least one bit, compute 2*b-c (Exact)
	   * else compute b-c/2 */
	  bp = MPFR_MANT(b);
	  cp = MPFR_MANT(c);
	  k = n-1;
	  limb = bp[k] - cp[k]/2;
	  if (limb > MPFR_LIMB_HIGHBIT)
	    {
	      /* We can't lose precision: compute b-c/2 */
	      /* Shift c in the allocated temporary block */
	    SubD1NoLose:
	      c0 = cp[0] & (MPFR_LIMB_ONE<<sh);
	      cp = (mp_limb_t*) TMP_ALLOC(n * BYTES_PER_MP_LIMB);
	      mpn_rshift(cp, MPFR_MANT(c), n, 1);
	      if (c0 == 0)
		{
		  /* Result is exact: no need of rounding! */
		  ap = MPFR_MANT(a);
		  mpn_sub_n (ap, bp, cp, n);
		  MPFR_SET_EXP(a, bx); /* No expo overflow! */
		  /* No truncate or normalize is needed */
		  MPFR_ASSERTD(ap[n-1] > ~ap[n-1]);
		  /* No rounding is necessary since the result is exact */
		  TMP_FREE(marker);
		  return 0;
		}
	      ap = MPFR_MANT(a);
	      mask = ~((MPFR_LIMB_ONE << sh) - MPFR_LIMB_ONE);
	      cp[0] &= mask; /* Delete last bit of c */
	      mpn_sub_n (ap, bp, cp, n);
	      MPFR_SET_EXP(a, bx);                 /* No expo overflow! */
	      MPFR_ASSERTD( !(ap[0] & ~mask) );    /* Check last bits */
	      /* No normalize is needed */
	      MPFR_ASSERTD(ap[n-1] > ~ap[n-1]);
	      /* Rounding is necessary since c0 = 1*/	  
	      /* Cp =-1 and C'p+1=0 */
	      bcp = 1; bcp1 = 0;
	      switch (rnd_mode)
		{
		case GMP_RNDN:
		  /* Even Rule apply: Check Ap-1 */
		  if ((ap[0] & (MPFR_LIMB_ONE<<sh)) == 0)
		    goto truncate;
		case GMP_RNDZ:
		  goto sub_one_ulp;
		default:
		  goto truncate;
		}
	    }
	  else if (limb < MPFR_LIMB_HIGHBIT)
	    {
	      /* We lose at least one bit of prec */
	      /* Calcul of 2*b-c (Exact) */
	      /* Shift b in the allocated temporary block */
	    SubD1Lose:
	      bp = (mp_limb_t*) TMP_ALLOC(n * BYTES_PER_MP_LIMB);
	      mpn_lshift(bp, MPFR_MANT(b), n, 1);
	      ap = MPFR_MANT(a);
	      mpn_sub_n (ap, bp, cp, n);
	      bx--;
	      goto ExactNormalize;
	    }
	  else
	    {
	      /* Case: limb = 100000000000 */
	      /* Check while b[k] == c'[k] (C' is C shifted by 1) */
	      /* If b[k]<c'[k] => We lose at least one bit*/
	      /* If b[k]>c'[k] => We don't lose any bit */
	      /* If k==-1 => We don't lose any bit 
		 AND the result is 100000000000 0000000000 00000000000 */
	      mp_limb_t carry;
	      do {
		carry = cp[k]&MPFR_LIMB_ONE;
		k--;
	      } while (k>=0 && 
		       bp[k]==(carry=cp[k]/2+(carry<<(BITS_PER_MP_LIMB-1))));
	      if (MPFR_UNLIKELY(k<0))
		{
		  /*If carry then (sh==0 and Virtual c'[-1] > Virtual b[-1]) */
		  if (MPFR_UNLIKELY(carry)) /* carry = cp[0]&MPFR_LIMB_ONE */
		    {
		      MPFR_ASSERTD(sh == 0);
		      goto SubD1Lose;
		    }
		  /* Result is a power of 2 */
		  ap = MPFR_MANT(a);
		  MPN_ZERO(ap, n);
		  ap[n-1] = MPFR_LIMB_HIGHBIT;
		  MPFR_SET_EXP(a, bx); /* No expo overflow! */
		  /* No Normalize is needed*/
		  /* No Rounding is needed */
		  TMP_FREE(marker);
		  return 0;
		}
	      /* carry = cp[k]/2+(cp[k-1]&1)<<(BITS_PER_MP_LIMB-1) = c'[k]*/
	      else if (bp[k]>carry)
		goto SubD1NoLose;
	      else 
		{
		  MPFR_ASSERTD(bp[k]<carry);
		  goto SubD1Lose;
		}
	    }
	}
    }
  else if (d >= p)
    {
      ap = MPFR_MANT(a);
      sh = (-p) % BITS_PER_MP_LIMB;
      /* We can't set A before since we use cp for rounding... */
      /* Perform rounding: check if a=b or a=b-ulp(b) */
      if (MPFR_UNLIKELY(d == p))
	{
	  /* cp == -1 and c'p+1 = ? */
	  bcp  = 1;
	  /* We need Cp+1 later for a very improbable case. */
	  bbcp = (MPFR_MANT(c)[n-1] & (MPFR_LIMB_ONE<<(BITS_PER_MP_LIMB-2)));
	  /* We need also C'p+1 for an even more unprobable case... */
	  if (bbcp)
	    bcp1 = 1;
	  else
	    {
              cp = MPFR_MANT(c);
              if (MPFR_UNLIKELY(cp[n-1] == MPFR_LIMB_HIGHBIT))
                {
                  mp_size_t k = n-1;
                  do {
                    k--;
                  } while (k>=0 && cp[k]==0);
                  bcp1 = (k>=0);      
                }
	      else
		bcp1 = 1;
	    }
	  DEBUG( printf("(D=P) Cp=-1 Cp+1=%lu C'p+1=%lu \n", bbcp, bcp1) );
	  bp = MPFR_MANT(b);
	  switch (rnd_mode)
	    {
	    case GMP_RNDN:
	      if (bcp && bcp1==0)		
		/* Cp=-1 and C'p+1=0: Even rule Apply! */
		/* Check Ap-1 = Bp-1 */
		if ((bp[0] & (MPFR_LIMB_ONE<<sh)) == 0)
		  goto copy_truncate;
	    case GMP_RNDZ:
	      MPN_COPY(ap, bp, n);
	      goto sub_one_ulp;
	    default:
	    copy_truncate:
	      MPN_COPY(ap, bp, n);
	      goto truncate;
	    }
	}
      else
	{
	  /* Cp=0, Cp+1=-1 if d==p+1, C'p+1=-1 */
          bcp = 0; bbcp = (d==p+1); bcp1 = 1;
          DEBUG( printf("(D>P) Cp=%lu Cp+1=%lu C'p+1=%lu\n", bcp,bbcp,bcp1) );
	  /* Need to compute C'p+2 if d==p+1 and if rnd_mode=NEAREST
	     (Because of a very improbable case) */
	  if (MPFR_UNLIKELY(d==p+1 && rnd_mode==GMP_RNDN))
	    {
	      cp = MPFR_MANT(c);
	      if (MPFR_UNLIKELY(cp[n-1] == MPFR_LIMB_HIGHBIT))
		{
		  mp_size_t k = n-1;
		  do {
		    k--;
		  } while (k>=0 && cp[k]==0);
		  bbcp1 = (k>=0);
		}
	      else
		bbcp1 = 1;
	      DEBUG( printf("(D>P) C'p+2=%lu\n", bbcp1) );
	    }
	  /* Copy mantissa B in A */
	  MPN_COPY(ap, MPFR_MANT(b), n);
	  /* Round */
	  if (rnd_mode == GMP_RNDZ)
	    goto sub_one_ulp;
	  else /* rnd_mode = AWAY or NEAREST */
	    goto truncate;
	}
    }
  else
    {
      mpfr_prec_t dm;
      mp_size_t m;
      mp_limb_t mask;

      /* General case: 2 <= d < p */
      sh = (-p) % BITS_PER_MP_LIMB;
      cp = (mp_limb_t*) TMP_ALLOC(n * BYTES_PER_MP_LIMB);

      /* Shift c in temporary allocated place */
      dm = d % BITS_PER_MP_LIMB;
      m = d / BITS_PER_MP_LIMB;
      if (MPFR_UNLIKELY(dm == 0))
	{
	  /* dm = 0 and m > 0: Just copy */
	  MPFR_ASSERTD(m!=0);
	  MPN_COPY(cp, MPFR_MANT(c)+m, n-m);
	  MPN_ZERO(cp+n-m, m);
	}
      else if (MPFR_LIKELY(m == 0))
	{
	  /* dm >=2 and m == 0: just shift */
	  MPFR_ASSERTD(dm >= 2);
	  mpn_rshift(cp, MPFR_MANT(c), n, dm);
	}
      else
	{
	  /* dm > 0 and m > 0: shift and zero  */
	  mpn_rshift(cp, MPFR_MANT(c)+m, n-m, dm);
	  MPN_ZERO(cp+n-m, m);
	}

      DEBUG( mpfr_print_mant_binary("Before", MPFR_MANT(c), p) );
      DEBUG( mpfr_print_mant_binary("B=    ", MPFR_MANT(b), p) );
      DEBUG( mpfr_print_mant_binary("After ", cp, p) );

      /* Compute bcp=Cp and bcp1=C'p+1 */
      if (MPFR_LIKELY(sh))
	{
	  /* Try to compute them from C' rather than C (FIXME: Faster?) */
	  bcp = (cp[0] & (MPFR_LIMB_ONE<<(sh-1))) ;
	  if (MPFR_LIKELY((cp[0]&((MPFR_LIMB_ONE<<(sh-1))-MPFR_LIMB_ONE))))
	    bcp1 = 1;
	  else
	    {
	      /* We can't compute C'p+1 from C'. Compute it from C */
	      mp_limb_t *tp = MPFR_MANT(c);
	      /* Start from bit x=p-d+sh in mantissa C
		 (+sh since we have already looked sh bits in C'!) */
	      mpfr_prec_t x = p-d+sh;
	      if (MPFR_UNLIKELY(x>p))
		/* We are already looked at all the bits of c, so C'p+1 = 0*/
		bcp1 = 0;
	      else
		{
		  mp_size_t kx = n-1 - (x / BITS_PER_MP_LIMB);
		  mpfr_prec_t sx = (-x)%BITS_PER_MP_LIMB;
		  /* Looks at the last bits of limb kx (if sx=0 does nothing)*/
		  if (tp[kx] & ((MPFR_LIMB_ONE<<sx)-MPFR_LIMB_ONE))
		    bcp1 = -1;
		  else
		    {
		      kx += (sx==0); /*If sx==0, tp[kx] hasn't been checked*/
		      do { 
			kx--;
		      } while (kx>=0 && tp[kx]==0);
		      bcp1 = (kx >= 0);
		    }
		}
	    }
	}
      else
	{
          /* Compute Cp and C'p+1 from C with sh=0 */
	  mp_limb_t *tp = MPFR_MANT(c);
	  /* Start from bit x=p-d in mantissa C */
	  mpfr_prec_t  x = p-d;
	  mp_size_t   kx = n-1 - (x / BITS_PER_MP_LIMB);
	  mpfr_prec_t sx = BITS_PER_MP_LIMB-1-(x%BITS_PER_MP_LIMB);
	  MPFR_ASSERTD(p >= d);
	  bcp = (tp[kx] & (MPFR_LIMB_ONE<<sx));
	  /* Looks at the last bits of limb kx (If sx=0, does nothing)*/
	  if (tp[kx] &((MPFR_LIMB_ONE<<sx)-MPFR_LIMB_ONE))
	    bcp1 = 1;
	  else
	    {
	      kx += (sx==0); /*If sx==0, tp[kx] hasn't been checked*/
	      do {
		kx--;
	      } while (kx>=0 && tp[kx]==0);
	      bcp1 = (kx>=0);
	    }
	}
      DEBUG( printf("Cp=%lu C'p+1=%lu\n", bcp, bcp1) );

      /* Check if we can lose a bit, and if so compute Cp+1 and C'p+2 */
      bp = MPFR_MANT(b);
      if (MPFR_UNLIKELY((bp[n-1]-cp[n-1]) <= MPFR_LIMB_HIGHBIT))
	{
	  /* We can lose a bit so we precompute Cp+1 and C'p+2 */
	  /* Test for trivial case: since C'p+1=0, Cp+1=0 and C'p+2 =0 */
	  if (MPFR_UNLIKELY(bcp1 == 0))
	    {
	      bbcp = 0;
	      bbcp1 = 0;
	    }
 	  else /* bcp1 != 0 */
	    {
	      /* We can lose a bit: 
		 compute Cp+1 and C'p+2 from mantissa C */
	      mp_limb_t *tp = MPFR_MANT(c);
	      /* Start from bit x=(p+1)-d in mantissa C */
	      mpfr_prec_t  x = p+1-d;
	      mp_size_t   kx = n-1 - (x/BITS_PER_MP_LIMB);
	      mpfr_prec_t sx = BITS_PER_MP_LIMB-1-(x%BITS_PER_MP_LIMB);
	      MPFR_ASSERTD(p > d);
	      DEBUG( printf("(pre) Kx=%ld Sx=%lu\n", kx, sx));
	      bbcp = (tp[kx] & (MPFR_LIMB_ONE<<sx)) ;
	      /* Looks at the last bits of limb kx (If sx=0, does nothing)*/
	      /* If Cp+1=0, since C'p+1!=0, C'p+2=1 ! */
	      if (!bbcp || (tp[kx] &((MPFR_LIMB_ONE<<sx)-MPFR_LIMB_ONE)))
		bbcp1 = 1;
	      else
		{
		  kx += (sx==0); /*If sx==0, tp[kx] hasn't been checked*/
		  do {
		    kx--;
		  } while (kx>=0 && tp[kx]==0);
		  bbcp1 = (kx>=0);
		}
	    } /*End of Bcp1 != 0*/
	  DEBUG( printf("(Pre) Cp+1=%lu C'p+2=%lu\n", bbcp, bbcp1) );
	} /* End of "can lose a bit" */

      /* Clean shifted C' */
      mask = ~((MPFR_LIMB_ONE << sh) - MPFR_LIMB_ONE);
      cp[0] &= mask;

      /* Substract the mantissa c from b in a */
      ap = MPFR_MANT(a);
      mpn_sub_n (ap, bp, cp, n);
      DEBUG( mpfr_print_mant_binary("Sub=  ", ap, p) );

      /* Normalize: we lose at max one bit*/
      limb = ap[n-1];
      if (MPFR_UNLIKELY(limb < ~limb))
	{
	  /* High bit is not set and we have to fix it! */
	  /* Ap >= 010000xxx001 */
	  mpn_lshift(ap, ap, n, 1);
	  /* Ap >= 100000xxx010 */
	  if (bcp) /* Check if Cp = -1 */
	    /* Since Cp == -1, we have to substract one more */
	    {
	      mpn_sub_1(ap, ap, n, MPFR_LIMB_ONE<<sh);
	      MPFR_ASSERTD(ap[n-1] > ~ap[n-1]);
	    }
	  /* Ap >= 10000xxx001 */
	  /* Final exponent -1 since we have shifted the mantissa */
	  bx--;
	  /* Update bcp and bcp1 */
	  bcp  = bbcp; 
	  bcp1 = bbcp1;
	  /* We dont't have anymore a valid Cp+1!
	     But since Ap >= 100000xxx001, the final sub can't unnormalize!*/
	}
      MPFR_ASSERTD( !(ap[0] & ~mask) );

      /* Rounding */
      switch (rnd_mode)
	{
	case GMP_RNDN:
	  if (bcp==0)
	    goto truncate;
	  else if ((bcp1) || ((ap[0] & (MPFR_LIMB_ONE<<sh)) != 0))
	    goto sub_one_ulp;
	  else
	    goto truncate;
	case GMP_RNDZ:
	  if (MPFR_LIKELY(bcp || bcp1))
	    goto sub_one_ulp;
	default:
	  goto truncate;
	}      
 }
  MPFR_RET_NEVER_GO_HERE();

  /* Sub one ulp to the result */
 sub_one_ulp:
  mpn_sub_1 (ap, ap, n, MPFR_LIMB_ONE << sh);
  /* Result should be smaller than exact value: inexact=-1 */
  inexact = -1;
  /* Check normalisation */
  limb = ap[n-1];
  if (MPFR_UNLIKELY(limb < ~limb))
    {
      /* ap was a power of 2, and we lose a bit */
      /* Now it is 0111111111111111111[00000 */
      mpn_lshift(ap, ap, n, 1);
      bx--;
      /* And the lost bit x depends on Cp+1, and Cp */
      /* Compute Cp+1 if it isn't already compute (ie d==1) */
      if (d == 1)
	bbcp = 0;
      DEBUG( printf("(SubOneUlp)Cp=%lu, Cp+1=%lu C'p+1=%lu\n", bcp,bbcp,bcp1));
      /* Compute the last bit (Since we have shifted the mantissa)
	 we need one more bit!*/
      if ((rnd_mode == GMP_RNDZ && bcp==0) 
	  || (rnd_mode==GMP_RNDN && bbcp ==0))
	{
	  ap[0] |= MPFR_LIMB_ONE<<sh;
	  if (rnd_mode == GMP_RNDN)
	    inexact = 1;
	  DEBUG( printf("(SubOneUlp) Last bit set\n") );
	}
      /* Result could be exact if C'p+1 = 0 and rnd == Zero 
	 since we have had one more bit to the result */
      /* Fixme: rnd_mode == GMP_RNDZ needed ? */
      if (bcp1==0 && rnd_mode == GMP_RNDZ)
	{
	  DEBUG( printf("(SubOneUlp) Exact result\n") );
	  inexact = 0;
	}
    }
  
  goto end_of_sub;

 truncate:
  /* Check if the result is an exact power of 2: 100000000000
     in which cases, we could have to do sub_one_ulp due to some nasty reasons:
     If Result is a Power of 2:
      + If rnd = AWAY, 
      |  If Cp=-1 and C'p+1 = 0, SubOneUlp and the result is EXACT.
	 If Cp=-1 and C'p+1 =-1, SubOneUlp and the result is above.
	 Otherwise truncate
      + If rnd = NEAREST,
         If Cp= 0 and Cp+1  =-1 and C'p+2=-1, SubOneUlp and the result is above
  	 If cp=-1 and C'p+1 = 0, SubOneUlp and the result is exact.
	 Otherwise truncate.
      X bit should always be set if SubOneUlp*/
  if (MPFR_UNLIKELY(ap[n-1] == MPFR_LIMB_HIGHBIT))
    {
      mp_size_t k = n-1;
      do {
	k--;
      } while (k>=0 && ap[k]==0);
      if (MPFR_UNLIKELY(k<0))
	{
	  /* It is a power of 2! */
	  /* Compute Cp+1 if it isn't already compute (ie d==1) */
	  if (d == 1)
	    bbcp=0;
	  DEBUG( printf("(Truncate) Cp=%lu, Cp+1=%lu C'p+1=%lu C'p+2=%lu\n", \
		 bcp, bbcp, bcp1, bbcp1) );
	  if (( ((rnd_mode == GMP_RNDN) || (rnd_mode != GMP_RNDZ)) && bcp) 
	      ||
	      ((rnd_mode == GMP_RNDN) && (bcp == 0) && (bbcp) && (bbcp1)))
	    {
	      DEBUG( printf("(Truncate) Do sub\n") );
	      mpn_sub_1 (ap, ap, n, MPFR_LIMB_ONE << sh);
	      mpn_lshift(ap, ap, n, 1);
	      ap[0] |= MPFR_LIMB_ONE<<sh;
	      bx--;
	      /* FIXME: Explain why it works (or why not)... */
	      inexact = (bcp1 == 0) ? 0 : (rnd_mode==GMP_RNDN) ? -1 : 1;
	      goto end_of_sub;
	    }
	}
    }
  
  /* Calcul of Inexact flag.*/
  inexact = MPFR_LIKELY(bcp || bcp1) ? 1 : 0;

 end_of_sub:
  /* Update Expo */
  if (MPFR_UNLIKELY(bx < __gmpfr_emin))
    {
      DEBUG( printf("(Final Underflow)\n") );
      /* Update rnd_mode */
      if (rnd_mode == GMP_RNDN &&
	  (bx < __gmpfr_emin - 1 ||
	   (inexact >= 0 && mpfr_powerof2_raw (a))))
	rnd_mode = GMP_RNDZ;
      TMP_FREE(marker);
      return mpfr_set_underflow (a, rnd_mode, MPFR_SIGN(a));
    }
  MPFR_SET_EXP (a, bx);

  TMP_FREE(marker);
  return inexact*MPFR_INT_SIGN(a);
}

