/* mpfr_exp -- exponential of a floating-point number

Copyright 1999, 2001, 2002 Free Software Foundation, Inc.

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
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

static int mpfr_exp_rational _PROTO ((mpfr_ptr, mpz_srcptr, int, int));

static int
mpfr_exp_rational (mpfr_ptr y, mpz_srcptr p, int r, int m)
{
  int n,i,k,j,l;
  mpz_t* P,*S;
  mpz_t* ptoj;
  int diff,expo;
  int precy = MPFR_PREC(y);
  int * mult;
  int prec_i_have;
  int *nb_terms;
  int accu;
  TMP_DECL (marker);

  TMP_MARK (marker);
  n = 1 << m;
  P = (mpz_t*) TMP_ALLOC((m+1) * sizeof(mpz_t));
  S = (mpz_t*) TMP_ALLOC((m+1) * sizeof(mpz_t));
  ptoj = (mpz_t*) TMP_ALLOC((m+1) * sizeof(mpz_t)); /* ptoj[i] = mantissa^(2^i) */
  mult = (int*) TMP_ALLOC((m+1) * sizeof(int)); 
  nb_terms = (int*) TMP_ALLOC((m+1) * sizeof(int)); 
  mult[0] = 0;
  for (i=0;i<=m;i++) { mpz_init(P[i]); mpz_init(S[i]); mpz_init(ptoj[i]); }
  mpz_set(ptoj[0], p);
  for (i=1;i<m;i++) mpz_mul(ptoj[i], ptoj[i-1], ptoj[i-1]);
  mpz_set_ui(P[0], 1);
  mpz_set_ui(S[0], 1);
  k = 0;
  nb_terms[0] = 1;
   prec_i_have = 0; 
   for (i=1;(prec_i_have < precy) && (i < n) ;i++) {
    k++;
    nb_terms[k] = 1;
    mpz_set_ui(P[k], i+1);
    mpz_set(S[k], P[k]);;
    j=i+1; l=0; while ((j & 1) == 0) {      
      mpz_mul(S[k], S[k], ptoj[l]);
      mpz_mul(S[k-1], S[k-1], P[k]);
      mpz_mul_2exp(S[k-1], S[k-1], r*(1<<l));
      mpz_add(S[k-1], S[k-1], S[k]);
      mpz_mul(P[k-1], P[k-1], P[k]);
      nb_terms[k-1] = nb_terms[k-1]+ nb_terms[k];
      mult[k] = mult[k-1] + (1 << l)*(r >> 2) + mpz_sizeinbase(P[k],2) - 1;
      prec_i_have = mult[k];
      l++; j>>=1; k--;
    }
  }
   l = 0;
   accu = 0;
   while (k > 0){
     mpz_mul(S[k], S[k], ptoj[_mpfr_ceil_log2((double) nb_terms[k])]);
     mpz_mul(S[k-1], S[k-1], P[k]);
     accu += nb_terms[k];
     mpz_mul_2exp(S[k-1], S[k-1], r* accu);
     mpz_add(S[k-1], S[k-1], S[k]);
     mpz_mul(P[k-1], P[k-1], P[k]);     
     l++; k--;
   }
   
  diff = mpz_sizeinbase(S[0],2) - 2*precy;
  expo = diff;
  if (diff >=0)
    {
      mpz_div_2exp(S[0],S[0],diff);
    } else 
      {
	mpz_mul_2exp(S[0],S[0],-diff);
      }
  diff = mpz_sizeinbase(P[0],2) - precy;
  expo -= diff;
  if (diff >=0)
    {
      mpz_div_2exp(P[0],P[0],diff);
    } else
      {
	mpz_mul_2exp(P[0],P[0],-diff);
	}

  mpz_tdiv_q(S[0], S[0], P[0]);
  mpfr_set_z(y,S[0], GMP_RNDD);
  MPFR_EXP(y) += expo;

  mpfr_div_2ui(y, y, r*(i-1),GMP_RNDN); 
  for (i=0;i<=m;i++) { mpz_clear(P[i]); mpz_clear(S[i]); mpz_clear(ptoj[i]); }
  TMP_FREE (marker);
  return 0;
}

#define shift (BITS_PER_MP_LIMB/2)

int
mpfr_exp3 (mpfr_ptr y, mpfr_srcptr x, mp_rnd_t rnd_mode)
{
  mpfr_t t;
  mpfr_t x_copy;
  int i,k;
  mpz_t uk;
  mpfr_t tmp;
  int ttt;
  int twopoweri;
  int Prec;
  int loop;
  int prec_x;
  int shift_x = 0;
  int good = 0;
  int realprec = 0;
  int iter;
  int logn, inexact = 0;

  /* decompose x */
  /* we first write x = 1.xxxxxxxxxxxxx
     ----- k bits -- */
  prec_x = _mpfr_ceil_log2 ((double) (MPFR_PREC(x)) / BITS_PER_MP_LIMB);
  if (prec_x < 0) prec_x = 0;
  logn =  _mpfr_ceil_log2 ((double) prec_x + MPFR_PREC(y));
  if (logn < 2) logn = 2;
  ttt = MPFR_EXP(x);
  mpfr_init2(x_copy,MPFR_PREC(x));
  mpfr_set(x_copy,x,GMP_RNDD);
  /* we shift to get a number less than 1 */
  if (ttt > 0) 
    {
      shift_x = ttt;
      mpfr_div_2ui(x_copy, x, ttt, GMP_RNDN);
      ttt = MPFR_EXP(x_copy);
    }
  realprec = MPFR_PREC(y)+logn;
  mpz_init (uk);
  while (!good){      
    Prec = realprec+shift+2+shift_x;
    k = _mpfr_ceil_log2 ((double) Prec / BITS_PER_MP_LIMB);

    /* now we have to extract */
    mpfr_init2 (t, Prec);
    mpfr_init2 (tmp, Prec);
    mpfr_set_ui(tmp,1,GMP_RNDN);
    twopoweri = BITS_PER_MP_LIMB;
    if (k <= prec_x) iter = k; else iter= prec_x;
    for(i = 0; i <= iter; i++){
      mpfr_extract (uk, x_copy, i);
	if (i)
	    mpfr_exp_rational (t, uk, twopoweri - ttt, k  - i + 1);
	else
	  {
	    /* particular case: we have to compute with x/2^., then
               do squarings (this is faster) */    
	      mpfr_exp_rational (t, uk, shift + twopoweri - ttt, k+1);
	    for (loop= 0 ; loop < shift; loop++)
	      mpfr_mul(t,t,t,GMP_RNDD);

	  }
	mpfr_mul(tmp,tmp,t,GMP_RNDD); 
	twopoweri <<= 1;
    }
      mpfr_clear (t);
      for (loop= 0 ; loop < shift_x; loop++)
	mpfr_mul (tmp, tmp, tmp, GMP_RNDD);
      if (mpfr_can_round (tmp, realprec, GMP_RNDD, rnd_mode, MPFR_PREC(y)))
	{
	  inexact = mpfr_set (y, tmp, rnd_mode);
	  good = 1;
	}
      else
	realprec += 3*logn;
      mpfr_clear (tmp);
  }
  mpz_clear (uk);
  mpfr_clear(x_copy);
  return inexact;
} 
