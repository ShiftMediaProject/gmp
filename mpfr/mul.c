/* mpfr_mul -- multiply two floating-point numbers

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

/* Remains to do:
- do not use all bits of b and c when MPFR_PREC(b)>MPFR_PREC(a) or MPFR_PREC(c)>MPFR_PREC(a)
  [current complexity is O(MPFR_PREC(b)*MPFR_PREC(c))]
*/

void 
#if __STDC__
mpfr_mul(mpfr_ptr a, mpfr_srcptr b, mpfr_srcptr c, mp_rnd_t rnd_mode) 
#else
mpfr_mul(a, b, c, rnd_mode) 
     mpfr_ptr a;
     mpfr_srcptr b;
     mpfr_srcptr c;
     mp_rnd_t rnd_mode;
#endif
{
  unsigned int bn, cn, an, tn, k; int cc;
  mp_limb_t *ap=MPFR_MANT(a), *bp=MPFR_MANT(b), *cp=MPFR_MANT(c), *tmp, b1;
  long int sign_product;
  TMP_DECL(marker); 

  /* deal with NaN and zero */
  if (MPFR_IS_NAN(b) || MPFR_IS_NAN(c)) 
    { MPFR_CLEAR_FLAGS(a); MPFR_SET_NAN(a); return; }
  if (MPFR_IS_INF(b)) 
    {
      if (!MPFR_NOTZERO(c)) { MPFR_CLEAR_FLAGS(a); MPFR_SET_NAN(a); return; }
      else 
	{ 
	  if (MPFR_SIGN(a) != MPFR_SIGN(b) * MPFR_SIGN(c)) MPFR_CHANGE_SIGN(a);
	  MPFR_CLEAR_FLAGS(a); 
	  MPFR_SET_INF(a); return; 
	}
    }
  else if (MPFR_IS_INF(c)) 
    {
      if (!MPFR_NOTZERO(b)) { MPFR_CLEAR_FLAGS(a); MPFR_SET_NAN(a); return; }
      else 
	{ 
	  if (MPFR_SIGN(a) != MPFR_SIGN(b) * MPFR_SIGN(c)) MPFR_CHANGE_SIGN(a);
	  MPFR_CLEAR_FLAGS(a); MPFR_SET_INF(a); return; 
	}
    }

  if (!MPFR_NOTZERO(b) || !MPFR_NOTZERO(c)) 
    { MPFR_CLEAR_FLAGS(a); MPFR_SET_ZERO(a); return; }

  sign_product = MPFR_SIGN(b) * MPFR_SIGN(c);

  MPFR_CLEAR_FLAGS(a); 
  bn = (MPFR_PREC(b)-1)/BITS_PER_MP_LIMB+1; /* number of significant limbs of b */
  cn = (MPFR_PREC(c)-1)/BITS_PER_MP_LIMB+1; /* number of significant limbs of c */
  tn = (MPFR_PREC(c)+MPFR_PREC(b)-1)/BITS_PER_MP_LIMB+1; 
  k = bn+cn; /* effective nb of limbs used by b*c */
  TMP_MARK(marker); 
  tmp = (mp_limb_t*) TMP_ALLOC(k*BYTES_PER_MP_LIMB);

  /* multiplies two mantissa in temporary allocated space */
  b1 = (bn>=cn) ? mpn_mul(tmp, bp, bn, cp, cn) : mpn_mul(tmp, cp, cn, bp, bn);

  /* now tmp[0]..tmp[k-1] contains the product of both mantissa,
     with tmp[k-1]>=2^(BITS_PER_MP_LIMB-2) */
  an = (MPFR_PREC(a)-1)/BITS_PER_MP_LIMB+1; /* number of significant limbs of a */
  b1 >>= BITS_PER_MP_LIMB-1; /* msb from the product */

  if (b1==0) mpn_lshift(tmp, tmp, k, 1);
  cc = mpfr_round_raw(ap, tmp+bn+cn-tn, 
		      MPFR_PREC(b)+MPFR_PREC(c), (sign_product<0), MPFR_PREC(a), rnd_mode);
  if (cc) { /* cc = 1 ==> result is a power of two */
    ap[an-1] = (mp_limb_t) 1 << (BITS_PER_MP_LIMB-1);
  }
  MPFR_EXP(a) = MPFR_EXP(b) + MPFR_EXP(c) + b1 - 1 + cc;
  if (sign_product * MPFR_SIGN(a)<0) MPFR_CHANGE_SIGN(a);
  TMP_FREE(marker); 
  return;
}
