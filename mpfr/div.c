/* mpfr_div -- divide two floating-point numbers

Copyright 1999, 2001 Free Software Foundation.

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
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int
mpfr_div (mpfr_ptr q, mpfr_srcptr u, mpfr_srcptr v, mp_rnd_t rnd_mode)
{
  mp_srcptr up, vp, bp;
  mp_size_t usize, vsize;  

  mp_ptr ap, qp, rp; 
  mp_size_t asize, bsize, qsize, rsize; 
  mp_exp_t qexp;

  mp_rnd_t rnd_mode1, rnd_mode2; 

  mp_size_t err, k;
  mp_limb_t near; 
  int inex, sh, can_round, can_round2, sign_quotient;
  unsigned int cc = 0, rw; 

  TMP_DECL (marker);


  /**************************************************************************
   *                                                                        *
   *              This part of the code deals with special cases            *
   *                                                                        *
   **************************************************************************/

  if (MPFR_IS_NAN(u) || MPFR_IS_NAN(v))
    {
      MPFR_SET_NAN(q);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(q);

  sign_quotient = MPFR_SIGN(u) * MPFR_SIGN(v);
  if (MPFR_SIGN(q) != sign_quotient)
    MPFR_CHANGE_SIGN(q);

  if (MPFR_IS_INF(u))
    {
      if (MPFR_IS_INF(v))
	{
	  MPFR_SET_NAN(q);
          MPFR_RET_NAN;
	}
      else
	{
          MPFR_SET_INF(q);
          MPFR_RET(0);
	}
    }
  else 
    if (MPFR_IS_INF(v)) 
      {
        MPFR_CLEAR_INF(q);
        MPFR_SET_ZERO(q);
        MPFR_RET(0);
      }

  MPFR_CLEAR_INF(q); /* clear Inf flag */

  if (MPFR_IS_ZERO(v))
    {
      if (MPFR_IS_ZERO(u))
	{
          MPFR_SET_NAN(q);
          MPFR_RET_NAN;
	}
      else
	{
          MPFR_SET_INF(q);
          MPFR_RET(0);
	}
    }

  if (MPFR_IS_ZERO(u))
    {
      MPFR_SET_ZERO(q);
      MPFR_RET(0);
    }

  /**************************************************************************
   *                                                                        *
   *              End of the part concerning special values.                *
   *                                                                        *
   **************************************************************************/


  up = MPFR_MANT(u);
  vp = MPFR_MANT(v);

  TMP_MARK (marker);
  usize = MPFR_ESIZE(u); 
  vsize = MPFR_ESIZE(v); 

  /**************************************************************************
   *                                                                        *
   *   First try to use only part of u, v. If this is not sufficient,       *
   *   use the full u and v, to avoid long computations eg. in the case     *
   *   u = v.                                                               *
   *                                                                        *
   **************************************************************************/

  /* The dividend is a, length asize. The divisor is b, length bsize. */

  qsize = (MPFR_PREC(q) + 3)/BITS_PER_MP_LIMB + 1; 
  if (vsize < qsize)
    {
      bsize = vsize; 
      bp = vp; 
    }
  else 
    {
      bsize = qsize; 
      bp = (mp_srcptr)vp + vsize - qsize; 
    }
  
  asize = bsize + qsize; 
  ap = (mp_ptr) TMP_ALLOC(asize * BYTES_PER_MP_LIMB); 
  if (asize > usize)
    {
      MPN_COPY(ap + asize - usize, up, usize); 
      MPN_ZERO(ap, asize - usize); 
    }
  else
    MPN_COPY(ap, up + usize - asize, asize); 
  
  /* Allocate limbs for quotient and remainder. */
  qp = (mp_ptr) TMP_ALLOC ((qsize + 1) * BYTES_PER_MP_LIMB);
  rp = (mp_ptr) TMP_ALLOC (bsize * BYTES_PER_MP_LIMB);
  rsize = bsize; 

  mpn_tdiv_qr(qp, rp, 0, ap, asize, bp, bsize);
  
  /* Estimate number of correct bits. */

  err = qsize * BITS_PER_MP_LIMB; 
  if (bsize < vsize) err -= 2; else if (asize < usize) err --; 

  /* We want to check if rounding is possible, but without normalizing
     because we might have to divide again if rounding is impossible, or
     if the result might be exact. We have however to mimic normalization */

  if (qp[qsize] != 0) { sh = -1; } 
  else { count_leading_zeros(sh, qp[qsize - 1]); } 
  
  /* 
     To detect asap if the result is inexact, so as to avoid doing the 
     division completely, we perform the following check :

     - if rnd_mode == GMP_RNDN, and the result is exact, we are unable
     to round simultaneously to zero and to infinity ; 

     - if rnd_mode == GMP_RNDN, and if we can round to zero with one extra
     bit of precision, we can decide rounding. Hence in that case, check
     as in the case of GMP_RNDN, with one extra bit. Note that in the case
     of close to even rounding we shall do the division completely, but
     this is necessary anyway : we need to know whether this is really
     even rounding or not.
  */

  if (rnd_mode == GMP_RNDN)
    { 
      rnd_mode1 = GMP_RNDZ; 
      near = 1; 
    }
  else 
    { 
      rnd_mode1 = rnd_mode; 
      near = 0; 
    }

  sh += near; 
  can_round = mpfr_can_round_raw(qp, qsize + 1, sign_quotient, err + sh + 
				 BITS_PER_MP_LIMB, GMP_RNDN, rnd_mode1, 
				 MPFR_PREC(q) + sh + BITS_PER_MP_LIMB); 

  switch (rnd_mode1) 
    {
    case GMP_RNDU : rnd_mode2 = GMP_RNDD; break; 
    case GMP_RNDD : rnd_mode2 = GMP_RNDU; break; 
    case GMP_RNDZ : rnd_mode2 = sign_quotient == 1 ? GMP_RNDU : GMP_RNDD; 
      break;
    default : rnd_mode2 = GMP_RNDZ; 
    }

  can_round2 = mpfr_can_round_raw(qp, qsize + 1, sign_quotient, err + sh + 
				  BITS_PER_MP_LIMB, GMP_RNDN, rnd_mode2, 
				  MPFR_PREC(q) + sh + BITS_PER_MP_LIMB); 
  
  sh -= near; 
  
  /* If either can_round or can_round2 is 0, either we cannot round or
     the result might be exact. If asize >= usize and bsize >= vsize, we
     can just check this by looking at the remainder. Otherwise, we
     have to correct our first approximation. */

  if ((!can_round || !can_round2) && (asize < usize || bsize < vsize)) 
    {
      int b = 0; 
      mp_ptr rem, rem2; 

  /**************************************************************************
   *                                                                        *
   *   The attempt to use only part of u and v failed. We first compute a   *
   *   correcting term, then perform the full division.                     *
   *   Put u = uhi + ulo, v = vhi + vlo. We have uhi = vhi * qp + rp,       *
   *   thus u - qp * v = rp + ulo - qp * vlo, that we shall divide by v.    *
   *                                                                        *
   **************************************************************************/

      rsize = qsize + 1 + 
	      (usize - asize > vsize - bsize  
	       ? usize - asize  
	       : vsize - bsize); 

      /*
	TODO : One operand is probably enough, but then we have to 
	perform one further comparison (compute first vlo * q, 
	try to substract r, try to substract ulo. Which is best ? 
	NB : ulo and r do not overlap. Draw advantage of this 
	[eg. HI(vlo*q) = r => compare LO(vlo*q) with b.]
      */

      rem = TMP_ALLOC(rsize * BYTES_PER_MP_LIMB);
      rem2 = TMP_ALLOC(rsize * BYTES_PER_MP_LIMB);

      rem[rsize - 1] = rem2 [rsize - 1] = 0; 

      if (bsize < vsize)
	{
	  /* Compute vlo * q */
	  if (qsize + 1 > vsize - bsize)
	    mpn_mul(rem + rsize - vsize - qsize - 1 + bsize, 
		    qp, qsize + 1, vp, vsize - bsize);
	  else
	    mpn_mul(rem + rsize - vsize - qsize - 1 + bsize, 
		    vp, vsize - bsize, qp, qsize + 1);

	  MPN_ZERO(rem, rsize - vsize - qsize - 1 + bsize); 
	}
      else MPN_ZERO(rem, rsize); 
     
      /* Compute ulo + r. The two of them do not overlap. */
      MPN_COPY(rem2 + rsize - 1 - qsize, rp, bsize);
      
      if (qsize + 1 > bsize)
	MPN_ZERO(rem2 + rsize - 1 - qsize + bsize, qsize + 1 - bsize); 
    
      if (asize < usize) 
	{
	  MPN_COPY(rem2 + rsize - 1 - qsize - usize + asize, 
		   up, usize - asize); 
	  MPN_ZERO(rem2, rsize - 1 - qsize - usize + asize); 
	}
      else 
	MPN_ZERO(rem2, rsize - 1 - qsize); 

      b = 0;       
      if (mpn_cmp(rem2, rem, rsize) >= 0)
	{
	  /* Positive correction is at most 1. */

	  mpn_sub_n(rem, rem2, rem, rsize); 
	  if (rem[rsize - 1] != 0 || 
	      mpn_cmp(rem + rsize - vsize - 1, vp, vsize) >= 0)
	    {
	      rem[rsize - 1] -= 
		mpn_sub_n(rem + rsize - vsize - 1, 
			  rem + rsize - vsize - 1, 
			  vp, vsize); 
	      qp[qsize] -= mpn_add_1(qp, qp, qsize, 1); 
	    }
	}
      else 
	{ 
	  /* Negative correction is at most 3 */
	  do 
	    {
	      b++; 
	      rem2[rsize - 1] += 
		mpn_add_n(rem2 + rsize - vsize - 1, 
			  rem2 + rsize - vsize - 1, vp, vsize); 
	    }
	  while (mpn_cmp(rem2, rem, rsize) < 0);
 
	  qp[qsize] -= mpn_sub_1(qp, qp, qsize, b); 
	  mpn_sub_n(rem, rem2, rem, rsize); 
	}
	  
      if (qp[qsize] != 0) 
	sh = -1; 
      else 
	count_leading_zeros(sh, qp[qsize - 1]); 

      err = BITS_PER_MP_LIMB * qsize; 
      rp = rem; 
    }
  
  /**************************************************************************
   *                                                                        *
   *                       Final stuff (rounding and so.)                   *
   *  From now on : qp is the quotient [size qsize], rp the remainder       *
   *  [size rsize].                                                         *
   **************************************************************************/

  qexp = MPFR_EXP(u) - MPFR_EXP(v);

  if (qp[qsize] != 0) 
    /* Hack : qp[qsize] is 0, 1 or 2, hence if not 0, = 2^(qp[qsize] - 1). */
    {
      near = mpn_rshift(qp, qp, qsize, qp[qsize]);
      qp[qsize - 1] |= GMP_LIMB_HIGHBIT; qexp += qp[qsize]; 
    }
  else
    {
      near = 0; 
      if (sh != 0) 
	{ 
	  mpn_lshift(qp, qp, qsize, sh); 
	  qexp -= sh; 
	}
    }
  
  cc = mpfr_round_raw_generic(qp, qp, err, (sign_quotient == -1 ? 1 : 0),
			      MPFR_PREC(q), rnd_mode, &inex, 1);      

  qp += qsize - MPFR_ESIZE(q); /* 0 or 1 */
  qsize = MPFR_ESIZE(q); 

  /* 
     At that point, either we were able to round from the beginning, 
     and know thus that the result is inexact. 

     Or we have performed a full division. In that case, we might still
     be wrong if both
     - the remainder is nonzero ; 
     - we are rounding to infinity or to nearest (the nasty case of even 
     rounding).
     - inex = 0, meaning that the non-significant bits of the quotients are 0,
     except when rounding to nearest (the nasty case of even rounding again).
  */
  
  if (!can_round || !can_round2) /* Lazy case. */
    {
      if (inex == 0) 
	{
	  k = rsize - 1; 

	  /* If a bit has been shifted out during normalization, hence 
	     the remainder is nonzero. */
	  if (near == 0) 
	    while (k >= 0) { if (rp[k]) break; k--; }

	  if (k >= 0) /* Remainder is nonzero. */ 
	    {
	      if ((rnd_mode == GMP_RNDD && sign_quotient == -1) 
		  || (rnd_mode == GMP_RNDU && sign_quotient == 1))
		/* Rounding to infinity. */
		{
		  inex = sign_quotient; 
		  cc = 1; 
		}
	      /* rounding to zero. */
	      else inex = -sign_quotient; 
	    }
	}
      else /* We might have to correct an even rounding if remainder
	      is nonzero and if even rounding was towards 0. */
	if (rnd_mode == GMP_RNDN && (inex == MPFR_EVEN_INEX 
				     || inex == -MPFR_EVEN_INEX))
	  {
	    k = rsize - 1; 

	  /* If a bit has been shifted out during normalization, hence 
	     the remainder is nonzero. */
	    if (near == 0)
	      while (k >= 0) 
		{ 
		  if (rp[k]) 
		    break; 
		  k--; 
		}
	    
	    if (k >= 0) /* In fact the quotient is larger than expected */
	      {
		inex = sign_quotient; /* To infinity, finally. */
		cc = 1; 
	      }
	  }
    }
	
  /* Final modification due to rounding */
  if (cc) 
    {
      sh = MPFR_PREC(q) & (BITS_PER_MP_LIMB - 1); 
      if (sh)
	cc = mpn_add_1 (qp, qp, qsize, 
                        MP_LIMB_T_ONE << (BITS_PER_MP_LIMB - sh));
      else
	cc = mpn_add_1 (qp, qp, qsize, MP_LIMB_T_ONE);
  
      if (cc)
        {
          mpn_rshift (qp, qp, qsize, 1);
          qp[qsize-1] |= GMP_LIMB_HIGHBIT;
          qexp++;
        }
    }
  
  rw = qsize * BITS_PER_MP_LIMB - MPFR_PREC(q);
  MPN_COPY(MPFR_MANT(q), qp, qsize); 
  TMP_FREE (marker);

  MPFR_MANT(q)[0] &= ~((MP_LIMB_T_ONE << rw) - MP_LIMB_T_ONE);
  MPFR_EXP(q) = qexp;

  MPFR_RET(inex); 
}
