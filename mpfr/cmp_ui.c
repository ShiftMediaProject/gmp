/* mpfr_cmp_ui -- compare a floating-point number with a machine integer

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
#include "longlong.h"
#include "mpfr.h"

/* returns a positive value if b>i*2^f,
           a negative value if b<i*2^f,
           zero if b=i*2^f
*/

int 
#if __STDC__
mpfr_cmp_ui_2exp ( mpfr_srcptr b, unsigned long int i, int f )
#else
mpfr_cmp_ui_2exp (b, i, f)
     mpfr_srcptr b; 
     unsigned long int i; 
     int f; 
#endif
{
  int e, k, bn; mp_limb_t c, *bp;

  if (SIGN(b)<0) return -1;
  else if (!NOTZERO(b)) return((i) ? -1 : 0);
  else { /* b>0 */
    e = EXP(b); /* 2^(e-1) <= b < 2^e */
    if (e>f+mp_bits_per_limb) return 1;

    c = (mp_limb_t) i;
    count_leading_zeros(k, c);
    k = f+mp_bits_per_limb - k; /* 2^(k-1) <= i*2^f < 2^k */
    if (k!=e) return (e-k);

    /* now k=e */
    c <<= (f+mp_bits_per_limb-k);
    bn = (PREC(b)-1)/mp_bits_per_limb;
    bp = MANT(b) + bn;
    if (*bp>c) return 1;
    else if (*bp<c) return -1;

    /* most significant limbs agree, check remaining limbs from b */
    while (--bn>=0)
      if (*--bp) return 1;
    return 0;
  }
}

/* returns a positive value if b>i*2^f,
           a negative value if b<i*2^f,
           zero if b=i*2^f 
*/

int 
#if __STDC__
mpfr_cmp_si_2exp ( mpfr_srcptr b, long int i, int f )
#else
mpfr_cmp_si_2exp(b, i, f)
     mpfr_srcptr b; 
     long int i; 
     int f; 
#endif
{
  int e, k, bn, si; mp_limb_t c, *bp;

  si = (i<0) ? -1 : 1; /* sign of i */
  if (SIGN(b)*i<0) return SIGN(b); /* both signs differ */
  else if (!NOTZERO(b) || (i==0)) { /* one is zero */
    if (i==0) return ((NOTZERO(b)) ? SIGN(b) : 0);
    else return si; /* b is zero */
      
  }
  else { /* b and i are of same sign */
    e = EXP(b); /* 2^(e-1) <= b < 2^e */
    if (e>f+mp_bits_per_limb) return si;

    c = (mp_limb_t) ((i<0) ? -i : i);
    count_leading_zeros(k, c);
    k = f+mp_bits_per_limb - k; /* 2^(k-1) <= i*2^f < 2^k */
    if (k!=e) return (si*(e-k));

    /* now k=e */
    c <<= (f+mp_bits_per_limb-k);
    bn = (PREC(b)-1)/mp_bits_per_limb;
    bp = MANT(b) + bn;
    if (*bp>c) return si;
    else if (*bp<c) return -si;

    /* most significant limbs agree, check remaining limbs from b */
    while (--bn>=0)
      if (*--bp) return si;
    return 0;
  }
}

