/*  kara_sqrtrem -- Karatsuba square root

Copyright (C) 1999-2000 PolKA project, Inria Lorraine and Loria

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

/* Reference: Karatsuba Square Root, Paul Zimmermann, Research Report 3805,
   INRIA, November 1999. */

#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"

#define SQRT_LIMIT KARATSUBA_MUL_THRESHOLD /* must be at least 3, should be
					      near from optimal */

/* n must be even */
mp_size_t kara_sqrtrem(mp_limb_t *s, mp_limb_t *r, mp_limb_t *op, mp_size_t n)
{
  if (n<SQRT_LIMIT) return mpn_sqrtrem(s, r, op, n);
  else {
    mp_size_t nn, rn, rrn, sn, qn; mp_limb_t *q, tmp;
    TMP_DECL (marker);

    TMP_MARK (marker);
    nn = n/4; /* block size 'b' corresponds to nn limbs */
    rn = kara_sqrtrem(s+nn, r+nn, op+2*nn, n-2*nn);
    /* rn <= ceil(n-2*nn, 2) + 1 <= ceil(2*nn+3, 2) + 1 <= nn+3 */
    /* to divide by 2*s', first divide by 2, to ensure the dividend is
       less than b^2 */
    sn=(n-2*nn+1)/2; /* sn >= nn */
    MPN_COPY(r, op+nn, nn); /* copy a_1 */
    tmp = mpn_rshift(r, r, nn+rn, 1);
    if (r[nn+rn-1]==0) rn--;
    q = (mp_limb_t*) TMP_ALLOC(2*(sn+1)*sizeof(mp_limb_t));
    if (nn+rn < 2*sn) MPN_ZERO(r+nn+rn, 2*sn-nn-rn);
    qn = sn; if (mpn_cmp(r+sn, s+nn, sn)>=0) { 
      q[qn++]=1; mpn_sub_n(r+sn, r+sn, s+nn, sn);
    }
#if 0
    mpn_divrem(q, 0, r, 2*sn, s+nn, sn);
#else
    mpn_divrem_n(q, r, s+nn, sn);
#endif
    while (qn>nn && q[qn-1]==0) qn--;
    MPN_COPY(s, q, nn);
    if (nn+rn > 2*sn) {
      tmp=mpn_add_n(s+sn, s+sn, q+sn, nn+rn-2*sn);
      if (tmp) mpn_add_1(s+nn+rn-sn, s+nn+rn-sn, (n+1)/2-nn-rn+sn, tmp);
    }
    /* multiply remainder by two and add low bit of a_1 */
    rrn = nn+sn; /* size of output remainder */
    rrn += mpn_lshift(r+nn, r, sn, 1);
    r[nn] |= (op[nn] & 1);
    sn += nn;
    if (qn>nn) {
      MPN_COPY(r, s+nn, qn-nn); /* save the qn-nn limbs from s */
      MPN_COPY(s+nn, q+nn, qn-nn); /* replace by those of q */
    }
    mpn_mul_n(q, s, s, qn);
    if (qn>nn) { /* restore the limbs from s, adding them to those of q */
      mp_limb_t cy;

      cy = mpn_add_n(s+nn, s+nn, r, qn-nn);
      if (qn<sn) cy = mpn_add_1(s+qn, s+qn, sn-qn, cy);
      if (cy) s[sn++]=1; 
    }
    MPN_COPY(r, op, nn); /* copy a_0 */
    qn = 2*qn; 
    if (qn<sn) MPN_ZERO(q+qn, sn-qn);
    if (rrn<sn) MPN_ZERO(r+rrn, sn-rrn);
    if (mpn_sub_n(r, r, q, sn) || (qn>sn)) {
      if (rrn>sn) rrn=sn;
      else {
	/* one shift and one add is faster than two add's */
	r[sn] = mpn_lshift(q, s, sn, 1) + mpn_add_n(r, r, q, sn)
	  - mpn_sub_1(r, r, sn, 1) - 1;
	rrn = sn + r[sn];
	mpn_sub_1(s, s, sn, 1);
      }
    }
    else if (rrn>sn) r[sn]=1;
    TMP_FREE (marker);
    MPN_NORMALIZE(r, rrn);
    return rrn;
  }
}
