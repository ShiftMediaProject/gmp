/* mpfr_log -- natural logarithm of a floating-point number

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
#include <math.h>
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"


  /* The computation of log(a) is done using the formula :
     if we want p bits of the result,
                       pi
	  log(a) ~ ------------  -   m log 2
		    2 AG(1,4/s)

     where s = x 2^m > 2^(p/2)

     More precisely, if F(x) = int(1/sqrt(1-(1-x^2)*sin(t)^2), t=0..PI/2),
     then for s>=1.26 we have log(s) < F(4/s) < log(s)*(1+4/s^2)
     from which we deduce pi/2/AG(1,4/s)*(1-4/s^2) < log(s) < pi/2/AG(1,4/s)
     so the relative error 4/s^2 is < 4/2^p i.e. 4 ulps.
  */


#define MON_INIT(xp, x, p, s) xp = (mp_ptr) TMP_ALLOC(s*BYTES_PER_MP_LIMB);    x -> _mp_prec = p; x -> _mp_d = xp; x -> _mp_size = s; x -> _mp_exp = 0; 

/* #define DEBUG */

int
#if __STDC__
mpfr_log(mpfr_ptr r, mpfr_srcptr a, unsigned char rnd_mode) 
#else
mpfr_log(r, a, rnd_mode)
     mpfr_ptr r;
     mpfr_srcptr a;
     unsigned char rnd_mode; 
#endif
{
  int p, m, q, bool, size, cancel;
  mpfr_t cst, rapport, agm, tmp1, tmp2, s, mm;
  mp_limb_t *cstp, *rapportp, *agmp, *tmp1p, *tmp2p, *sp, *mmp;
  double ref;
  TMP_DECL(marker);

  /* If a is NaN or a is negative or null, the result is NaN */
  if (FLAG_NAN(a) || (SIGN(a)<=0))
    { SET_NAN(r); return 1; }

  /* If a is 1, the result is 0 */
  if (mpfr_cmp_ui_2exp(a,1,0)==0){
    SET_ZERO(r);
    return 0; /* only case where the result is exact */
  }

  q=PREC(r);
  
  ref=mpfr_get_d(a)-1.0;
  if (ref<0)
    ref=-ref;

  p=q+4;
  /* adjust to entire limb */
  if (p%BITS_PER_MP_LIMB) p += BITS_PER_MP_LIMB - (p%BITS_PER_MP_LIMB);

  bool=1;

  while (bool==1) {
#ifdef DEBUG
    printf("a="); mpfr_print_raw(a); putchar('\n');
    printf("p=%d\n", p);
#endif
    /* Calculus of m (depends on p) */
    m=(int) ceil(((double) p)/2.0) -EXP(a)+1;

    /* All the mpfr_t needed have a precision of p */
    TMP_MARK(marker);
    size=(p-1)/BITS_PER_MP_LIMB+1;
    MON_INIT(cstp, cst, p, size);  
    MON_INIT(rapportp, rapport, p, size);
    MON_INIT(agmp, agm, p, size);
    MON_INIT(tmp1p, tmp1, p, size);  
    MON_INIT(tmp2p, tmp2, p, size);  
    MON_INIT(sp, s, p, size);
    MON_INIT(mmp, mm, p, size);

    mpfr_set_si(mm,m,GMP_RNDN);           /* I have m, supposed exact */
    mpfr_set_si(tmp1,1,GMP_RNDN);         /* I have 1, exact */
    mpfr_set_si(tmp2,4,GMP_RNDN);         /* I have 4, exact */
    mpfr_mul_2exp(s,a,m,GMP_RNDN);        /* I compute s=a*2^m, err <= 1 ulp */
    mpfr_div(rapport,tmp2,s,GMP_RNDN);    /* I compute 4/s, err <= 2 ulps */
    mpfr_agm(agm,tmp1,rapport,GMP_RNDN);  /* AG(1,4/s), err<=3 ulps */
    mpfr_mul_2exp(tmp1,agm,1,GMP_RNDN);   /* 2*AG(1,4/s), still err<=3 ulps */
    mpfr_pi(cst, GMP_RNDN);               /* I compute pi, err<=1ulp */
    mpfr_div(tmp2,cst,tmp1,GMP_RNDN);     /* pi/2*AG(1,4/s), err<=5ulps */
    mpfr_log2(cst,GMP_RNDN);              /* I compute log(2), err<=1ulp */
    mpfr_mul(tmp1,cst,mm,GMP_RNDN);       /* I compute m*log(2), err<=2ulps */
    cancel = EXP(tmp2); 
    mpfr_sub(cst,tmp2,tmp1,GMP_RNDN);     /* log(a), err<=7ulps+cancel */ 
    cancel -= EXP(cst);
#ifdef DEBUG
    printf("cancelled bits=%d\n", cancel);
    printf("approx="); mpfr_print_raw(cst); putchar('\n');
#endif
    if (cancel<0) cancel=0;

    /* If we can round the result, we set it and go out of the loop */

    /* we have 7 ulps of error from the above roundings,
       4 ulps from the 4/s^2 second order term,
       plus the cancelled bits */
    if (mpfr_can_round(cst,p-cancel-4,GMP_RNDN,rnd_mode,q)==1) {
      mpfr_set(r,cst,rnd_mode);
#ifdef DEBUG
      printf("result="); mpfr_print_raw(r); putchar('\n');
#endif
      bool=0;
    }
    /* else we increase the precision */
    else {
      p += BITS_PER_MP_LIMB+cancel;
      TMP_FREE(marker);
    }

    /* We clean */
    TMP_FREE(marker);
    
  }
  return 1; /* result is inexact */
}


