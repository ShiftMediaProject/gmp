/* mpfr_div_ui -- divide a floating-point number by a machine integer

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
#include "longlong.h"
#include "mpfr.h"
#include "mpfr-impl.h"

/* #define DEBUG */

/* returns 0 if result exact, non-zero otherwise */
int
#ifdef __STDC__
mpfr_div_ui(mpfr_ptr y, mpfr_srcptr x, unsigned long int u, mp_rnd_t rnd_mode)
#else
mpfr_div_ui(y, x, u, rnd_mode)
     mpfr_ptr y;
     mpfr_srcptr x;
     unsigned long int u;
     mp_rnd_t rnd_mode;
#endif
{
  int xn, yn, dif, sh, i; mp_limb_t *xp, *yp, *tmp, c, d;
  TMP_DECL(marker);

  if (MPFR_IS_NAN(x)) { MPFR_SET_NAN(y); return 1; }

  MPFR_CLEAR_NAN(y); /* clear NaN flag */

  if (MPFR_IS_INF(x)) 
    { 
      MPFR_SET_INF(y); 
      if (MPFR_SIGN(y) * MPFR_SIGN(x) < 0) /* consider u=0 as +0 */
	MPFR_CHANGE_SIGN(y); 
      return 0; 
      /*    TODO: semantique de la division par un zero entier ? signe ? */
    }

  if (u==0) 
    {
      if (MPFR_IS_ZERO(x)) { MPFR_SET_NAN(y) ; return 1; }
      else
	{ 
	  MPFR_SET_INF(y); MPFR_SET_SAME_SIGN(y, x); return 0; 
	  /* TODO: semantique de la division dans ce cas-la aussi ? */
	}
    }

  MPFR_CLEAR_INF(y);

  TMP_MARK(marker);
  xn = (MPFR_PREC(x)-1)/BITS_PER_MP_LIMB + 1;
  yn = (MPFR_PREC(y)-1)/BITS_PER_MP_LIMB + 1;

  xp = MPFR_MANT(x);
  yp = MPFR_MANT(y);
  MPFR_EXP(y) = MPFR_EXP(x);
  if (MPFR_SIGN(x) * MPFR_SIGN(y) < 0) MPFR_CHANGE_SIGN(y);

  dif = yn+1-xn;
#ifdef DEBUG
  printf("dif=%d u=%lu xn=%d\n",dif,u,xn);
  printf("x="); mpfr_print_raw(x); putchar('\n');
#endif

  /* we need to store yn+1 = xn + dif limbs of the quotient */
  /* don't use tmp=yp since the mpn_lshift call below requires yp >= tmp+1 */
  tmp=TMP_ALLOC((yn+1)*BYTES_PER_MP_LIMB);

  c = (mp_limb_t) u;
  if (dif>=0) {
#if (__GNU_MP_VERSION < 3) && (UDIV_NEEDS_NORMALIZATION==1)
    /* patch for bug in mpn_divrem_1 for GMP 2.xxx */
    count_leading_zeros(sh, c);
    c <<= sh;
    MPFR_EXP(y) += sh;
#endif
    c = mpn_divrem_1(tmp, dif, xp, xn, c);
  }
  else /* dif < 0 i.e. xn > yn */
    c = mpn_divrem_1(tmp, 0, xp-dif, yn, c);

  if (tmp[yn]==0) { tmp--; sh=0; MPFR_EXP(y) -= BITS_PER_MP_LIMB; }
  /* shift left to normalize */
  count_leading_zeros(sh, tmp[yn]);
  if (sh) {
    mpn_lshift(yp, tmp+1, yn, sh);
    yp[0] += tmp[0] >> (BITS_PER_MP_LIMB-sh);
    MPFR_EXP(y) -= sh; 
  }
  else MPN_COPY(yp, tmp+1, yn);
#ifdef DEBUG
  printf("y="); mpfr_print_raw(y); putchar('\n');
#endif

  sh = yn*BITS_PER_MP_LIMB - MPFR_PREC(y);
  /* it remains sh bits in less significant limb of y */

  d = *yp & (((mp_limb_t)1 << sh) - 1);
  *yp ^= d; /* set to zero lowest sh bits */

  TMP_FREE(marker);
  if ((c | d)==0) {
    for (i=0; i<-dif && xp[i]==0; i++);
    if (i>=-dif) return 0; /* result is exact */
  }

  switch (rnd_mode) {
  case GMP_RNDZ:
    return 1; /* result is inexact */
  case GMP_RNDU:
    if (MPFR_SIGN(y)>0) mpfr_add_one_ulp(y);
    return 1; /* result is inexact */
  case GMP_RNDD:
    if (MPFR_SIGN(y)<0) mpfr_add_one_ulp(y);
    return 1; /* result is inexact */
  case GMP_RNDN:
    if (d < ((mp_limb_t)1 << (sh-1))) return 1;
    else if (d > ((mp_limb_t)1 << (sh-1))) {
      mpfr_add_one_ulp(y);
    }
    else { /* d = (mp_limb_t)1 << (sh-1) */
      if (c) mpfr_add_one_ulp(y);
      else {
	for (i=0; i<-dif && xp[i]==0; i++);
	if (i<-dif) mpfr_add_one_ulp(y);
	else { /* exactly in the middle */
	  if (*yp & ((mp_limb_t)1 << sh)) mpfr_add_one_ulp(y);
	}
      }
    }
    return 1;
  }
  return 0; /* to prevent warning from gcc */
}



