/* mpfr_set -- copy of a floating-point number

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
#include "gmp.h"
#include "gmp-impl.h"
#include "mpfr.h"

void 
#if __STDC__
mpfr_set4(mpfr_ptr a, mpfr_srcptr b, unsigned char rnd_mode, int signb)
#else
mpfr_set4(a, b, rnd_mode, signb) 
     mpfr_ptr a; 
     mpfr_srcptr b; 
     unsigned char rnd_mode;
     int signb;
#endif
{
  int carry, an, preca = PREC(a), sh; mp_limb_t *ap = MANT(a);

  carry = mpfr_round_raw(ap, MANT(b), PREC(b), (signb<0), preca, rnd_mode);
  EXP(a) = EXP(b);
  if (carry) {
    an = (preca-1)/BITS_PER_MP_LIMB + 1;
    sh = an * BITS_PER_MP_LIMB - preca;
    if ((*ap >> sh) & 1) {
      fprintf(stderr, "unable to round in mpfr_set\n"); exit(1);
    }
    mpn_rshift(ap, ap, an, 1);
    ap[an-1] |= (mp_limb_t) 1 << (BITS_PER_MP_LIMB-1);
    EXP(a)++;
  }
  if (SIGN(a) != signb) CHANGE_SIGN(a);
}
