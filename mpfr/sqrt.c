/* mpfr_sqrt -- square root of a floating-point number

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "longlong.h"

/* #define DEBUG */

int
mpfr_sqrt (mpfr_ptr r, mpfr_srcptr u, unsigned char rnd_mode)
{
  mp_ptr up, rp, tmp, remp;
  mp_size_t usize, rrsize;
  mp_size_t rsize;
  mp_size_t prec, err;
  mp_limb_t q_limb;
  long rw, nw, k; 
  int exact = 0; 
  unsigned long cc = 0; 
  char can_round = 0; 
  TMP_DECL (marker); TMP_DECL(marker0); 

  if (FLAG_NAN(u) || SIGN(u) == -1) { SET_NAN(r); return 0; }
  
  prec = PREC(r);

  if (!NOTZERO(u))
    {
      EXP(r) = 0; 
      MPN_ZERO(MANT(r), ABSSIZE(r)); 
      return 1; 
    }

  up = MANT(u);

#ifdef DEBUG
      printf("Entering square root : "); 
      for(k = usize - 1; k >= 0; k--) { printf("%lu ", up[k]); }
      printf(".\n"); 
#endif

  /* Compare the mantissas */
  
  usize = (PREC(u) - 1)/BITS_PER_MP_LIMB + 1; 
  rsize = ((PREC(r) + 2 + (EXP(u) & 1))/BITS_PER_MP_LIMB + 1) << 1; 
  rrsize = (PREC(r) + 2 + (EXP(u) & 1))/BITS_PER_MP_LIMB + 1;
  /* One extra bit is needed in order to get the square root with enough
     precision ; take one extra bit for rrsize in order to solve more 
     easily the problem of rounding to nearest.
     Need to have 2*rrsize = rsize...
     Take one extra bit if the exponent of u is odd since we shall have
     to shift then.
  */

  TMP_MARK(marker0); 
  if (EXP(u) & 1) /* Shift u one bit to the right */
    {
      if (PREC(u) & (BITS_PER_MP_LIMB - 1))
	{
	  up = TMP_ALLOC(usize*BYTES_PER_MP_LIMB);
	  mpn_rshift(up, u->_mp_d, usize, 1); 
	}
      else
	{
	  up = TMP_ALLOC((usize + 1)*BYTES_PER_MP_LIMB);
	  if (mpn_rshift(up + 1, u->_mp_d, ABSSIZE(u), 1))
	    up [0] = ((mp_limb_t) 1) << (BITS_PER_MP_LIMB - 1); 
	  else up[0] = 0; 
	  usize++; 
	}
    }

  EXP(r) = ((EXP(u) + (EXP(u) & 1)) / 2) ;  
  
  do
    {
      TMP_MARK (marker);

      err = rsize*BITS_PER_MP_LIMB; 
      if (rsize < usize) { err--; }
      if (err > rrsize * BITS_PER_MP_LIMB) 
	{ err = rrsize * BITS_PER_MP_LIMB; }
      
      tmp = (mp_ptr) TMP_ALLOC (rsize * BYTES_PER_MP_LIMB);  
      rp = (mp_ptr) TMP_ALLOC (rrsize * BYTES_PER_MP_LIMB); 
      remp = (mp_ptr) TMP_ALLOC (rsize * BYTES_PER_MP_LIMB); 

      if (usize >= rsize) { 
	MPN_COPY (tmp, up + usize - rsize, rsize);
      }
      else { 
	MPN_COPY (tmp + rsize - usize, up, usize);
	MPN_ZERO (tmp, rsize - usize); 
      }

      /* Do the real job */
 
#ifdef DEBUG
      printf("Taking the sqrt of : "); 
      for(k = rsize - 1; k >= 0; k--) { 
	printf("+%lu*2^%lu",tmp[k],k*mp_bits_per_limb); }
      printf(".\n"); 
#endif

      q_limb = kara_sqrtrem (rp, remp, tmp, rsize);

#ifdef DEBUG
      printf("The result is : \n"); 
      printf("sqrt : "); 
      for(k = rrsize - 1; k >= 0; k--) { printf("%lu ", rp[k]); }
      printf("(q_limb = %lu)\n", q_limb); 
#endif
      
      can_round = (mpfr_can_round_raw(rp, rrsize, 1, err, 
				      GMP_RNDZ, rnd_mode, PREC(r))); 

      /* If we used all the limbs of both the dividend and the divisor, 
	 then we have the correct RNDZ rounding */

      if (!can_round && (rsize < 2*usize)) 
	{ 
#ifdef DEBUG
	  printf("Increasing the precision.\n"); 
#endif
	  TMP_FREE(marker); 
	}
    }
  while (!can_round && (rsize < 2*usize) 
	 && (rsize += 2) && (rrsize ++)); 


  /* This part may be deplaced upper to avoid a few mpfr_can_round_raw */
  /* when the square root is exact. It is however very unprobable that */
  /* it would improve the behaviour of the present code on average.    */

  if (!q_limb) /* possibly exact */
    {
      /* if we have taken into account the whole of up */
      for (k = usize - rsize - 1; k >= 0; k ++)
	if (up[k]) break; 
      
      if (k < 0) { exact = 1; goto fin; }
    }

  if (can_round) 
    {
      cc = mpfr_round_raw(rp, rp, err, 0, PREC(r), rnd_mode);  
      rrsize = (PREC(r) - 1)/BITS_PER_MP_LIMB + 1; 
    }
  else
    /* Use the return value of sqrtrem to decide of the rounding         */
    /* Note that at this point the sqrt has been computed                */
    /* EXACTLY. If rounding = GMP_RNDZ, do nothing [comes from           */
    /* the fact that the exact square root can end with a bunch of ones, */
    /* and in that case we indeed cannot round if we do not know that    */
    /* the computation was exact.                                        */
    switch (rnd_mode)
      {
      case GMP_RNDZ : 
      case GMP_RNDD : break; 

      case GMP_RNDN : 
	/* Not in the situation ...0 111111 */
	rw = (PREC(r) + 1) & (BITS_PER_MP_LIMB - 1);
	if (rw) { rw = BITS_PER_MP_LIMB - rw; nw = 0; } else nw = 1; 
	if ((rp[nw] >> rw) & 1 &&                     /* Not 0111111111 */
	    (q_limb ||                                /* Nonzero remainder */
	    (rw ? (rp[nw] >> (rw + 1)) & 1 : 
	     (rp[nw] >> (BITS_PER_MP_LIMB - 1)) & 1))) /* or even rounding */ 
	  cc = mpn_add_1(rp + nw, rp + nw, rrsize, ((mp_limb_t)1) << rw); 
	break;
 
      case GMP_RNDU : 
	if (q_limb)
	  cc = mpn_add_1(rp, rp, rrsize, 1 << (BITS_PER_MP_LIMB - 
					       (PREC(r) & 
						(BITS_PER_MP_LIMB - 1))));
      }

  if (cc) {
    mpn_rshift(rp, rp, rrsize, 1);
    rp[rrsize-1] |= (mp_limb_t) 1 << (BITS_PER_MP_LIMB-1);
    r->_mp_exp++; 
  }

 fin:
  rsize = rrsize; 
  rrsize = (PREC(r) - 1)/BITS_PER_MP_LIMB + 1;  
  MPN_COPY(r->_mp_d, rp + rsize - rrsize, rrsize); 

  if (PREC(r) & (BITS_PER_MP_LIMB - 1))
    MANT(r) [0] &= ~(((mp_limb_t)1 << (BITS_PER_MP_LIMB - 
				   (PREC(r) & (BITS_PER_MP_LIMB - 1)))) - 1) ; 
  
  TMP_FREE(marker0); TMP_FREE (marker);
  return exact; 
}
