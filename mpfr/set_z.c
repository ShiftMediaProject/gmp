/* mpfr_set_z -- set a floating-point number from a multiple-precision integer

Copyright (C) 1999, 2001 Free Software Foundation, Inc.

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
#include "mpfr.h"
#include "mpfr-impl.h"
#include "gmp-impl.h"
#include "longlong.h"

/* set f to the integer z */
int 
mpfr_set_z (mpfr_ptr f, mpz_srcptr z, mp_rnd_t rnd)
{
  mp_size_t fn, zn, dif, sh;
  int k, sign_z;
  mp_limb_t *fp = MPFR_MANT(f), *zp, cc, c2;

  MPFR_CLEAR_FLAGS (f); /* z cannot be NaN nor Inf */

  sign_z = mpz_cmp_ui (z, 0);

  if (sign_z==0) {
    MPFR_SET_ZERO(f);
    return 0;
  }

  fn = 1 + (MPFR_PREC(f)-1)/BITS_PER_MP_LIMB;
  zn = ABS(SIZ(z));
  dif = zn-fn;
  zp = PTR(z);
  count_leading_zeros(k, zp[zn-1]);
  MPFR_EXP(f) = zn*BITS_PER_MP_LIMB-k;
  if (MPFR_SIGN(f)*sign_z<0) MPFR_CHANGE_SIGN(f);
  if (dif>=0) { /* number has to be truncated */
    if (k) {
      mpn_lshift(fp, zp + dif, fn, k);
      if (dif) *fp += zp[dif-1] >> (BITS_PER_MP_LIMB-k);
    }
    else MPN_COPY(fp, zp + dif, fn);
    sh = fn*BITS_PER_MP_LIMB-MPFR_PREC(f);
    cc = *fp & ((MP_LIMB_T_ONE << sh) - 1);
    *fp = *fp & ~cc;
    if (rnd==GMP_RNDN) {
      if (sh) c2 = MP_LIMB_T_ONE << (sh - 1);
      else { /* sh=0 */
	c2 = MP_LIMB_T_HIGHBIT;
	dif--;
	cc = (dif>=0) ? ((zp[dif])<<k) : 0;
	if (dif>0 && k) cc += zp[dif-1] >> (BITS_PER_MP_LIMB-k);
      }
      /* now compares cc to c2 */
      if (cc>c2) { mpfr_add_one_ulp(f); return cc; }
      else if (cc<c2) goto towards_zero;
      else {
	cc=0;
	while (dif>0 && (cc=zp[dif-1])==0) dif--;
	if (cc) { mpfr_add_one_ulp(f); return cc; }
	else /* exactly in middle: inexact in both cases */
	  if (*fp & (MP_LIMB_T_ONE << sh)) { mpfr_add_one_ulp(f); return 1; }
	  else return 1;
      }
    }
    else if ((sign_z>0 && rnd==GMP_RNDU) || (sign_z<0 && rnd==GMP_RNDD)) {
      /* round towards infinity */
      /* result is exact iff all remaining bits are zero */
      if (dif>0 && cc==0) cc=zp[--dif]<<k;
      while (cc==0 && dif>0) cc=zp[--dif];
      if (cc) { mpfr_add_one_ulp(f); return 1; }
      else return 0;
    }
    else { /* round towards zero */
      /* result is exact iff all remaining bits are zero */
    towards_zero:
      if (cc==0 && dif>0) cc=zp[--dif]<<k;
      while (cc==0 && dif>0) cc=zp[--dif];
      return cc;
    }
  }
  else {
    if (k) mpn_lshift(fp-dif, zp, zn, k);
    else MPN_COPY(fp-dif, zp, zn);
    /* fill with zeroes */
    MPN_ZERO(fp, -dif);
    return 0; /* result is exact */
  }
}



