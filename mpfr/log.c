/* mpfr_log -- natural logarithm of a floating-point number

Copyright 1999, 2000, 2001, 2002 Free Software Foundation.

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

/* #define DEBUG */

int
mpfr_log (mpfr_ptr r, mpfr_srcptr a, mp_rnd_t rnd_mode) 
{
  int m, bool, size, cancel, inexact = 0;
  mp_prec_t p, q;
  mpfr_t cst, rapport, agm, tmp1, tmp2, s, mm;
  mp_limb_t *cstp, *rapportp, *agmp, *tmp1p, *tmp2p, *sp, *mmp;
  double ref;
  TMP_DECL(marker);

  /* If a is NaN, the result is NaN */
  if (MPFR_IS_NAN(a))
    {
      MPFR_SET_NAN(r);
      MPFR_RET_NAN;
    }

  MPFR_CLEAR_NAN(r);

  /* check for infinity before zero */
  if (MPFR_IS_INF(a))
    {
      if (MPFR_SIGN(a) < 0) /* log(-Inf) = NaN */
	{
	  MPFR_SET_NAN(r);
          MPFR_RET_NAN;
	}
      else /* log(+Inf) = +Inf */
	{
	  MPFR_SET_INF(r);
          MPFR_SET_POS(r);
          MPFR_RET(0);
	}
    }

  /* Now we can clear the flags without damage even if r == a */
  MPFR_CLEAR_INF(r);

  if (MPFR_IS_ZERO(a))
    {
      MPFR_SET_INF(r);
      MPFR_SET_NEG(r);
      MPFR_RET(0); /* log(0) is an exact -infinity */
    }

  /* If a is negative, the result is NaN */
  if (MPFR_SIGN(a) < 0)
    {
      MPFR_SET_NAN(r);
      MPFR_RET_NAN;
    }

  /* If a is 1, the result is 0 */
  if (mpfr_cmp_ui (a, 1) == 0)
    {
      MPFR_SET_ZERO(r);
      MPFR_SET_POS(r);
      MPFR_RET(0); /* only "normal" case where the result is exact */
    }

  q=MPFR_PREC(r);
  
  ref = mpfr_get_d1 (a) - 1.0;
  if (ref<0)
    ref=-ref;

  /* use initial precision about q+lg(q)+5 */
  p=q+5; m=q; while (m) { p++; m >>= 1; }

  /* adjust to entire limb */
  if (p%BITS_PER_MP_LIMB) p += BITS_PER_MP_LIMB - (p%BITS_PER_MP_LIMB);

  bool=1;

  while (bool==1) {
#ifdef DEBUG
    printf("a="); mpfr_print_binary(a); putchar('\n');
    printf("p=%d\n", p);
#endif
    /* Calculus of m (depends on p) */
    m = (p + 1) / 2 - MPFR_EXP(a) + 1;

    /* All the mpfr_t needed have a precision of p */
    TMP_MARK(marker);
    size=(p-1)/BITS_PER_MP_LIMB+1;
    MPFR_INIT(cstp, cst, p, size);  
    MPFR_INIT(rapportp, rapport, p, size);
    MPFR_INIT(agmp, agm, p, size);
    MPFR_INIT(tmp1p, tmp1, p, size);  
    MPFR_INIT(tmp2p, tmp2, p, size);  
    MPFR_INIT(sp, s, p, size);
    MPFR_INIT(mmp, mm, p, size);

    mpfr_set_si (mm, m, GMP_RNDN);        /* I have m, supposed exact */
    mpfr_set_si (tmp1, 1, GMP_RNDN);      /* I have 1, exact */
    mpfr_set_si (tmp2, 4, GMP_RNDN);      /* I have 4, exact */
    mpfr_mul_2si (s, a, m, GMP_RNDN);    /* I compute s=a*2^m, err <= 1 ulp */
    mpfr_div (rapport, tmp2, s, GMP_RNDN);/* I compute 4/s, err <= 2 ulps */
    mpfr_agm (agm, tmp1, rapport, GMP_RNDN); /* AG(1,4/s), err<=3 ulps */
    mpfr_mul_2ui (tmp1, agm, 1, GMP_RNDN); /* 2*AG(1,4/s), still err<=3 ulps */
    mpfr_const_pi (cst, GMP_RNDN);        /* compute pi, err<=1ulp */
    mpfr_div (tmp2, cst, tmp1, GMP_RNDN); /* pi/2*AG(1,4/s), err<=5ulps */
    mpfr_const_log2 (cst, GMP_RNDN);      /* compute log(2), err<=1ulp */
    mpfr_mul(tmp1,cst,mm,GMP_RNDN);       /* I compute m*log(2), err<=2ulps */
    cancel = MPFR_EXP(tmp2); 
    mpfr_sub(cst,tmp2,tmp1,GMP_RNDN);     /* log(a), err<=7ulps+cancel */ 
    cancel -= MPFR_EXP(cst);
#ifdef DEBUG
    printf("canceled bits=%d\n", cancel);
    printf("approx="); mpfr_print_binary(cst); putchar('\n');
#endif
    if (cancel<0) cancel=0;

    /* If we can round the result, we set it and go out of the loop */

    /* we have 7 ulps of error from the above roundings,
       4 ulps from the 4/s^2 second order term,
       plus the canceled bits */
    if (mpfr_can_round (cst, p - cancel - 4, GMP_RNDN, rnd_mode, q) == 1) {
      inexact = mpfr_set (r, cst, rnd_mode);
#ifdef DEBUG
      printf("result="); mpfr_print_binary(r); putchar('\n');
#endif
      bool=0;
    }
    /* else we increase the precision */
    else {
      p += BITS_PER_MP_LIMB + cancel;
    }

    /* We clean */
    TMP_FREE(marker);
    
  }
  return inexact; /* result is inexact */
}


