/* mpfr_sin_cos -- sine and cosine of a floating-point number

Copyright (C) 2000 Free Software Foundation.

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
#include <stdlib.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"
#include "mpfr-impl.h"

int mpfr_sin_aux _PROTO ((mpfr_ptr, mpz_srcptr, int, int));
int mpfr_cos_aux _PROTO ((mpfr_ptr, mpz_srcptr, int, int));

#undef A
#undef B
#define C
#define C1  3
#define C2  2
#define GENERIC mpfr_sin_aux 
#include "generic.c" 
#undef C
#undef C1
#undef C2
#undef GENERIC

#undef A
#undef B
#define C
#define C1  1
#define C2  2
#define GENERIC mpfr_cos_aux 
#include "generic.c" 

#define shift (BITS_PER_MP_LIMB / 2)

int
#if __STDC__
mpfr_sin_cos (mpfr_ptr sinus, mpfr_ptr cosinus, mpfr_srcptr x, mp_rnd_t rnd_mode)
#else
mpfr_sin_cos (sinus, cosinus, x, rnd_mode)
     mpfr_ptr sinus;
     mpfr_ptr cosinus;
     mpfr_srcptr x; 
     mp_rnd_t rnd_mode;
#endif
{
  mpfr_t t_sin, t_cos;
  mpfr_t x_copy;
  int i,k;
  mpz_t uk;
  mpz_t square;
  mpfr_t tmp_sin, tmp_cos;
  mpfr_t tmp;
  mpfr_t inter;
  int ttt;
  int twopoweri;
  int Prec;
  int loop;
  int prec_x;
  int shift_x = 0;
  int good = 0;
  int realprec = 0, target_prec;
  int iter;
  int factor;
  int logn;
  int tmp_factor;
  int tmpi;

  if (sinus == cosinus) {
    fprintf (stderr, "Error in mpfr_sin_cos: 1st and 2nd operands must be different\n");
    exit (1);
  }

  if (MPFR_IS_NAN(x) || MPFR_IS_INF(x)) {
    MPFR_SET_NAN(sinus);
    MPFR_SET_NAN(cosinus);
    return 1; /* inexact */
  }

  if (!MPFR_NOTZERO(x)) { 
    mpfr_set_ui(sinus, 0, GMP_RNDN); 
    mpfr_set_ui(cosinus, 1, GMP_RNDN); 
    return 0; /* exact results */
  }

  prec_x = _mpfr_ceil_log2 ((double) MPFR_PREC(x) / BITS_PER_MP_LIMB);
  ttt = MPFR_EXP(x);
  mpfr_init2(x_copy, MPFR_PREC(x));
  mpfr_set(x_copy,x,GMP_RNDD);
  mpz_init(square);
  /* on fait le shift pour que le nombre soit inferieur a 1 */
  if (ttt > 0) 
    {
      shift_x = ttt;
      mpfr_mul_2exp(x_copy,x,-ttt, GMP_RNDN); 
      ttt = MPFR_EXP(x_copy);
    }
  target_prec = MPFR_PREC(sinus);
  if (MPFR_PREC(cosinus) > target_prec) target_prec = MPFR_PREC(cosinus);
  logn =  _mpfr_ceil_log2 ((double) target_prec);
  if (logn < 2) logn = 2;
  factor = logn;
  realprec = target_prec + logn;
  mpz_init (uk);
  while (!good) {
    Prec = realprec + 2*shift + 2 + shift_x + factor;
    k = _mpfr_ceil_log2 ((double) Prec / BITS_PER_MP_LIMB);

    /* now we extract */
    mpfr_init2(t_sin, Prec);
    mpfr_init2(t_cos, Prec);
    mpfr_init2(tmp, Prec);
    mpfr_init2(tmp_sin, Prec);
    mpfr_init2(tmp_cos, Prec);
    mpfr_init2(inter, Prec);
    mpfr_set_ui(tmp_sin,0,GMP_RNDN);
    mpfr_set_ui(tmp_cos,1,GMP_RNDN);
    twopoweri = BITS_PER_MP_LIMB;   
    if (k <= prec_x) iter = k; else iter= prec_x;
    for(i = 0; i <= iter; i++){
      mpfr_extract (uk, x_copy, i);
	if (i)
	  {
	    mpz_mul (square, uk, uk);
	    mpz_neg (square, square);
	    mpfr_sin_aux (t_sin, square, 2*(twopoweri - ttt) + 2, k - i + 1);
	    mpfr_cos_aux (t_cos, square, 2*(twopoweri - ttt) + 2, k - i + 1);
	    mpfr_set_z (tmp, uk, GMP_RNDD);
	    mpfr_mul (t_sin, t_sin, tmp, GMP_RNDD);
	    mpfr_div_2exp (t_sin, t_sin, twopoweri - ttt, GMP_RNDD);
	  }
	else
	  {
	    /* cas particulier : on est oblige de faire les calculs avec x/2^. 
	       puis elever au carre (plus rapide) */    
	    mpz_set (square, uk);
	    mpz_mul(square, square, square);
	    mpz_neg(square, square);
	    /* pour l'instant, shift = 0 ... */
	    /* ATTENTION, IL FAUT AUSSI MULTIPLIER LE DENOMINATEUR */
	    mpfr_sin_aux(t_sin,square, 2*(shift + twopoweri - ttt) + 2, k+1);
	    mpfr_cos_aux(t_cos,square, 2*(shift + twopoweri - ttt) + 2, k+1);
	    mpfr_set_z (tmp, uk, GMP_RNDD);
	    mpfr_mul(t_sin, t_sin, tmp,GMP_RNDD);
	    /* LA AUSSI, IL FAUT PENSER A DECALER DE twopoweri - ttt) */
	    mpfr_div_2exp(t_sin,t_sin, twopoweri - ttt + shift, GMP_RNDD);
       	    for (loop= 0 ; loop < shift; loop++){
	      /* t_sin = sin(a)
		 t_cos = cos(a) */
	      /* on veut t_sin = 2 sin a cos a
		 et t_cos = 2 * cos^2 - 1 */
	      mpfr_mul(t_sin, t_sin, t_cos, GMP_RNDD);
	      mpfr_mul_2exp(t_sin, t_sin, 1, GMP_RNDD);
	      
	      mpfr_mul(t_cos, t_cos, t_cos, GMP_RNDD);
	      mpfr_mul_2exp(t_cos, t_cos, 1, GMP_RNDD);
      	      mpfr_sub_ui(t_cos, t_cos, 1,  GMP_RNDD);
	    }
	  }
	/* on utilise cos(a+b) = cos a cos b - sin a sin b 
	   sin(a+b) = sin a cos b + cos a sin b */
	/* Donnees : 
	   tmp = cos(a) 
	   tmp_sin = sin(a)
	   t_sin = sin(b)
	   t_cos = cos(b) */	  
	mpfr_set(tmp, tmp_cos,GMP_RNDD);
	/* inter = sin a sin b */
	mpfr_mul(inter, tmp_sin, t_sin, GMP_RNDD);
	/* tmp_cos = cos a cos b */
	mpfr_mul(tmp_cos, tmp_cos, t_cos, GMP_RNDD);
	/* tmp_cos = cos (a+b) */
	mpfr_sub(tmp_cos, tmp_cos, inter, GMP_RNDD);
	/* inter = cos a sin b */
	mpfr_mul(inter, tmp, t_sin, GMP_RNDD);
	/* tmp_sin = sin a cos b */
	mpfr_mul(tmp_sin, tmp_sin, t_cos, GMP_RNDD);
	/* tmp_sin = sin (a+b) */
	mpfr_add(tmp_sin, tmp_sin, inter, GMP_RNDD);
	twopoweri <<= 1;
    }
    tmp_factor= factor;
    for (loop= 0 ; loop < shift_x; loop++){
      mpfr_mul(tmp_sin, tmp_sin, tmp_cos, GMP_RNDD);
      mpfr_mul_2exp(tmp_sin, tmp_sin, 1, GMP_RNDD);
      mpfr_mul(tmp_cos, tmp_cos, tmp_cos, GMP_RNDD);
      mpfr_mul_2exp(tmp_cos, tmp_cos, 1, GMP_RNDD);
      tmpi = -MPFR_EXP(tmp_cos);
      mpfr_set_ui(tmp, 1, GMP_RNDN);
      mpfr_sub(tmp_cos, tmp_cos, tmp,  GMP_RNDD);
      /* rep\'erer si le nombre de chiffres obtenu est suffisant pour 
	 avoir la bonne pr\'ecision. Le probl\`eme : comment faire ? 
         la pr\'ecision s'obtient en comparant 
	 (Prec-factor) a la pr\'ecision obtenue r\'eellement, celle-ci
	 \'etant donn\'ee par Prec + MPFR_EXP(tmp_cos). 
	 il faut donc comparer MPFR_EXP(tmp_cos) a factor */
      tmp_factor -= -MPFR_EXP(tmp_cos) + tmpi;    
      if (tmp_factor <= 0)
	{
	  factor += -tmp_factor  + 5;
	  goto try_again;
	}      
    }
    if (mpfr_can_round(tmp_sin, realprec, GMP_RNDD, rnd_mode, MPFR_PREC(sinus)) &&
	mpfr_can_round(tmp_cos, realprec, GMP_RNDD, rnd_mode, MPFR_PREC(cosinus))) {
	mpfr_set(sinus, tmp_sin, rnd_mode);
	mpfr_set(cosinus, tmp_cos, rnd_mode);
	good = 1;
    }
    else {
      realprec += 3*logn;
    }
  try_again:
    mpfr_clear(t_sin);
    mpfr_clear(t_cos);
    mpfr_clear(tmp);
    mpfr_clear(tmp_sin);
    mpfr_clear(tmp_cos);
    mpfr_clear(inter);
  }
  mpz_clear (uk);
  mpz_clear (square);
  mpfr_clear (x_copy);
  return 1; /* inexact result */
} 



